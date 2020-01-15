#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<string.h>
#include<dirent.h>
#define MAX 2000
struct element
{
	char* value;
	struct element* next;
};
struct folder
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
struct folder** memory_realloc_folder(struct folder** p, int size)
{
	return realloc(p, size);
}
char* constructLocation(char* src, char* entry)
{
	char* temp = memory_alloc(sizeof(char), strlen(src) + strlen(entry) + 2);
	strcpy_s(temp,strlen(src)+1, src);
	strcat_s(temp,strlen(temp)+strlen("/")+1, "/");
	strcat_s(temp,strlen(temp)+strlen(entry)+1, entry);
	return temp;
}
struct folder* getFolderPointer(char* name)
{
	struct folder* current_folder = (struct folder*) memory_alloc(sizeof(struct folder), 1);
	current_folder->no_of_child_folders = 0;
	current_folder->no_of_child_files = 0;
	current_folder->path = memory_alloc(sizeof(char), strlen(name) + 1);
	strcpy_s(current_folder->path,strlen(name)+1, name);
	current_folder->first_child = (struct element*) memory_alloc(sizeof(struct element), 1);
	return current_folder;
}
int addToChildList(struct folder* dir, char* child)
{
	struct element* root = dir->first_child;
	if (root->value == NULL)
	{
		root->value = memory_alloc(sizeof(char), strlen(child) + 1);
		strcpy_s(root->value,strlen(child)+1,child);
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
	strcpy_s(root->next->value,strlen(child)+1, child);
	root->next->next = (struct element*) memory_alloc(sizeof(struct element), 1);
	return 0;
}
struct folder* getAsNode(char* source_folder)
{
	DIR* dir = opendir(source_folder);
	if (dir == NULL)
	{
		//printf("not a folder -> %s\n", source_folder);
		return NULL;
	}
	struct folder* current_folder = getFolderPointer(source_folder);

	struct dirent* entry;
	int index = 0;

	while ((entry = readdir(dir)) != NULL)
	{
		char* entry_name = (char*)memory_alloc(sizeof(char), strlen(entry->d_name) + 1);
		strcpy_s(entry_name, strlen(entry->d_name) + 1, entry->d_name);
		//printf("%s\n", entry_name);
		if (strcmp(entry_name, ".") == 0 || strcmp(entry_name, "..") == 0) {
			continue;
		}
		addToChildList(current_folder, entry_name);
		index++;
	}
	closedir(dir);
	return current_folder;
}
int constructTreeChilds(struct folder* node)
{
	if (node == NULL)
		return 0;
	char* location = memory_alloc(sizeof(char), strlen(node->path) + 1);
	strcpy_s(location,strlen(node->path)+1, node->path);
	struct element* temp_child = node->first_child;
	node->child_folders = (struct folder**) memory_alloc(sizeof(struct folder), 1);
	while (temp_child != NULL && temp_child->value != NULL) {
		//printf("printing as Node %s - %s\n", location, temp_child->value);
		temp_child = temp_child->next;
	}
	temp_child = node->first_child;
	while (temp_child != NULL && temp_child->value != NULL)
	{
		struct folder* temp = getAsNode(constructLocation(location, temp_child->value));
		if (temp != NULL)
		{
			//printf("getting as Node %s - %s \n", location, temp_child->value);
			node->no_of_child_folders++;
			node->child_folders = memory_realloc_folder(node->child_folders, sizeof(struct folder) * node->no_of_child_folders);
			node->child_folders[node->no_of_child_folders - 1] = temp;
			constructTreeChilds(temp);
		}
		else
		{
			node->no_of_child_files++;
		}
		temp_child = temp_child->next;
	}
	return 0;
}
char* getSpaces(char* spaces, int spacelength)
{
	int index;
	for ( index = 0; index < spacelength; index++)
	{
		spaces[index] = ' ';
	}
	spaces[spacelength] = '\0';
	char* ptr = spaces;
	return ptr;
}
int printResults(struct folder* root)
{
	printf("\n%s", root->path);
	int spacelength = 87 - strlen(root->path);
	char spaces[100];
	printf("%s%d\t\t%d\n", getSpaces(spaces, spacelength), root->no_of_child_files, root->no_of_child_folders);
	printf("------------------------------------------------------------------------------------------------------------");
	int index;
	for (index = 0; index < root->no_of_child_folders; index++)
	{
		printResults(root->child_folders[index]);
	}
	return 0;
}
int constructTree()
{
	char source_folder[MAX];
	printf("Enter the Directory");
	scanf_s("%s", source_folder,sizeof(source_folder));
	char* p = memory_alloc(sizeof(char), strlen(source_folder) + 1);
	strcpy_s(p,strlen(source_folder)+1, source_folder);

	root = getAsNode(p);
	struct element* temp = root->first_child;
	/*while(temp->next!=NULL)
	{
		printf("\n%s",temp->value);
		temp=temp->next;
	}*/
	printf("Source location - %s\n", root->path);
	constructTreeChilds(root);
	return 0;
}
int main()
{
	constructTree();
	char spaces[MAX];
	printf("\n\n\nParent Folder%sNo. Of Files\tNo. Of Folders\n", getSpaces(spaces, 70));
	printResults(root);
	_getch();
	return 0;
}
