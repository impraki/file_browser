#include<stdio.h>
#include<winsock2.h>
#include<winsock.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#define MAX 2000
#define PORT 7067
int ssend = 0;
int sum = 0;
char sendbuff[MAX];
struct element    // LIST
{
	char* value;
	char* location;
	char* type;
	char* size;
	struct element* next;
};
struct folder           // FOLDERS AS STRUCTURE
{
	struct folder** child_folders;
	int no_of_child_files;
	int no_of_child_folders;
	struct element* first_child;
	char* path;
};
struct folder* root;
char* memory_alloc(int size, int no)
{
	return calloc(no, size);
}
char* constructPath(char* src, char* entry)
{
	char* temp = memory_alloc(sizeof(char), strlen(src) + strlen(entry) + 2);
	strcpy_s(temp, strlen(src) + 1, src);
	strcat_s(temp, strlen(temp) + strlen("/") + 1, "/");
	strcat_s(temp, strlen(temp) + strlen(entry) + 1, entry);
	return temp;
}
struct folder* getFolderPointer(char* name)
{
	struct folder* current_folder = (struct folder*) memory_alloc(sizeof(struct folder), 1);
	current_folder->no_of_child_folders = 0;
	current_folder->no_of_child_files = 0;
	current_folder->path = memory_alloc(sizeof(char), strlen(name) + 1);
	strcpy_s(current_folder->path, strlen(name) + 1, name);
	current_folder->first_child = (struct element*) memory_alloc(sizeof(struct element), 1);
	return current_folder;
}
int addToChildList(struct folder* dir, char* child,char* source)
{
	char* path;
	char* fil = "File";
	char* fol = "Folder";
	char temp[10];
	struct element* root = dir->first_child;
	struct stat stats;
	if (root->value == NULL)
	{
		root->value = memory_alloc(sizeof(char), strlen(child) + 1);
		strcpy_s(root->value, strlen(child) + 1, child);
		printf("\n%s", root->value);
		path = memory_alloc(sizeof(char), strlen(source) + strlen(root->value) + 2);
		path=constructPath(source, root->value);
		if (stat(path, &stats) != 0)
		{
			printf("Error");
		}
		//printf("\n%d", stats.st_size);
		snprintf(temp, sizeof(temp), "%d", stats.st_size);
		root->size = memory_alloc(sizeof(char), strlen(temp) + 1);
		strcpy_s(root->size, strlen(temp) + 1,temp);
		if (S_ISDIR(stats.st_mode))
		{
			root->type = memory_alloc(sizeof(char), strlen(fol) + 1);
			strcpy_s(root->type, strlen(fol) + 1, fol);
		}
		else {
			root->type = memory_alloc(sizeof(char), strlen(fil) + 1);
			strcpy_s(root->type, strlen(fil) + 1,fil);
		}
		root->location = memory_alloc(sizeof(char), strlen(source) + 1);
		strcpy_s(root->location, strlen(source) + 1, source);
		//printf("%s\n",root->value);
		root->next = (struct element*) memory_alloc(sizeof(struct element), 1);
		return 0;
	}
	while (root->next->value != NULL)
	{
		//printf("addToChildList inside while running %s\n", child);
		root = root->next;
	}
	root->next->value = memory_alloc(sizeof(char), strlen(child) + 1);
	strcpy_s(root->next->value, strlen(child) + 1, child);
	path = memory_alloc(sizeof(char), strlen(source) + strlen(root->next->value) + 2);
	path = constructPath(source, root->next->value);
	stat(path, &stats);
	snprintf(temp, sizeof(temp), "%d", stats.st_size);
	root->next->size = memory_alloc(sizeof(char), strlen(temp) + 1);
	strcpy_s(root->next->size, strlen(temp) + 1,temp);
	if (S_ISDIR(stats.st_mode))
	{
		root->next->type = memory_alloc(sizeof(char), strlen(fol) + 1);
		strcpy_s(root->next->type, strlen(fol) + 1, fol);
	}
	else {
		root->next->type = memory_alloc(sizeof(char), strlen(fol) + 1);
		strcpy_s(root->next->type, strlen(fil) + 1, fil);
	}
	root->next->location = memory_alloc(sizeof(char), strlen(source) + 1);
	strcpy_s(root->next->location, strlen(source) + 1, source);
	root->next->next = (struct element*) memory_alloc(sizeof(struct element), 1);
	return 0;
}
struct folder* getAsNode(char* source_folder)
{
	DIR* dir = opendir(source_folder);
	if (dir == NULL)
	{
		//printf("CANT OPEN THE DIRECTORY");
		return NULL;
	}
	struct folder* current_folder = getFolderPointer(source_folder);
	struct dirent* entry;

	while ((entry = readdir(dir)) != NULL)
	{
		char* entry_name = entry->d_name;
		//printf("%s\n", entry_name);
		if (strcmp(entry_name, ".") == 0 || strcmp(entry_name, "..") == 0)
		{
			continue;
		}
		addToChildList(current_folder, entry_name,source_folder);
	}
	closedir(dir);
	return current_folder;
}
int printChilds(struct folder* root, SOCKET sacps)
{
	struct element* head = root->first_child;
	while (head->next != NULL)
	{
		strcpy_s(sendbuff, strlen(head->value) + 1,head->value);
		strcat_s(sendbuff, strlen(sendbuff) + strlen("###") + 1, "###");
		strcat_s(sendbuff, strlen(sendbuff) + strlen(head->location) + 1, head->location);
		strcat_s(sendbuff, strlen(sendbuff) + strlen("###") + 1, "###");
		strcat_s(sendbuff, strlen(sendbuff) + strlen(head->type) + 1,head->type);
		strcat_s(sendbuff, strlen(sendbuff) + strlen("###") + 1, "###");
		strcat_s(sendbuff, strlen(sendbuff) + strlen(head->size) + 1,head->size);
		//printf("\n%s", sendbuff);
		ssend = send(sacps,sendbuff, strlen(sendbuff), 0);
		sum = sum + ssend;
		head = head->next;
	}
	return 0;
}
int constructTree(char* p)
{
	root = getAsNode(p);
	//printf("Source location - %s\n", root->path);
	return 0;
}
int main()
{
	char recbuf[MAX];
	struct sockaddr_in ser, cli;
	int ccli = sizeof(cli);
	WSADATA data;
	SOCKET sersock;
	int sbind, slis;
	SOCKET sacceptsocket;
	int srec, start, closesoc, clean;
	do {
		start = WSAStartup(MAKEWORD(2, 2), &data);
		if (start != 0)
		{
			printf("\nstart failed");
		}
		else
			printf("\nServer started");
		ser.sin_family = AF_INET;
		ser.sin_addr.s_addr = htonl(INADDR_ANY);
		ser.sin_port = htons(PORT);

		sersock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sersock == INVALID_SOCKET)
		{
			printf("socket error");
		}
		else
			printf("\nSERVER socket created");
		sbind = bind(sersock, (SOCKADDR*)&ser, sizeof(ser));
		if (sbind == SOCKET_ERROR)
		{
			printf("\nbind failed");
		}
		else
			printf("\nbinded successfully");
		slis = listen(sersock, 128);
		if (slis == SOCKET_ERROR)
		{
			printf("\nlistening failed");
		}
		else
			printf("\nSERVER LISTENING");
		sacceptsocket = accept(sersock, (SOCKADDR*)&cli, &ccli);
		if (sacceptsocket == INVALID_SOCKET)
		{
			printf("\n Accept failed");
		}
		else
			printf("\nClient Connection accepted");
		srec = recv(sacceptsocket, recbuf, sizeof(recbuf) - 1, 0);
		if (srec == SOCKET_ERROR)
		{
			printf("\nrecv error");
		}
		printf("\nDirectory received=%s", recbuf);
		constructTree(recbuf);
		printChilds(root, sacceptsocket);
			closesoc = closesocket(sacceptsocket);
			if (closesoc == SOCKET_ERROR)
			{
				printf("\nclose error");
			}
			else
				printf("\nserver closed");
			clean = WSACleanup();
			if (clean == SOCKET_ERROR)
			{
				printf("\nclean error");
			}
			else
				printf("\nclean success");
		printf("\nbytes send=%d", sum);
	} while (1);
	system("PAUSE");
	return 0;
}