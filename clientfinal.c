#include<jni.h>
#include "FileBr.h"
#include<stdio.h>
#include<stdlib.h>
#include<winsock2.h>
#include<winsock.h>
#include<string.h>
#include<WS2tcpip.h>
#include<WtsApi32.h>
#define MAX 2000
#define PORT 7067
int sum = 0;
jobjectArray createArray(JNIEnv* env, jsize count, char* elements)
{
    //printf("\n%s", elements);
    jclass stringClass = (*env)->FindClass(env, "java/lang/String");
    jobjectArray row = (*env)->NewObjectArray(env, count, stringClass, 0);
    jsize i = 0;
    (*env)->SetObjectArrayElement(env, row, i, (*env)->NewStringUTF(env, elements));
    return row;
}
JNIEXPORT jobjectArray JNICALL Java_FileBr_filebrowser(JNIEnv* env, jobject thisObj, jstring ip, jstring dir)
{
    char recbuf[MAX];
    char a[MAX];
    char* arrPointer;
    jobjectArray Arrofobj[40] = { 0 };
    struct sockaddr_in cli;
    WSADATA data;
    SOCKET clisock;
    int closesoc, ccon, i = 0, csend, crec = 0, clean, start;
    char ipc[MAX];
    char dirc[MAX];
    strcpy_s(ipc, sizeof(ipc), (*env)->GetStringUTFChars(env, ip, 0));
    strcpy_s(dirc, sizeof(dirc), (*env)->GetStringUTFChars(env, dir, 0));
    start = WSAStartup(MAKEWORD(2, 2), &data);
    if (start != 0)
    {
        printf("\nstart failed");
    }
    else
        printf("\nClient started");
    cli.sin_family = AF_INET;
    inet_pton(AF_INET, ipc, &cli.sin_addr.s_addr);
    cli.sin_port = htons(PORT);
    clisock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clisock == INVALID_SOCKET)
    {
        printf("socket error");
    }
    else
        printf(" client socket created");
    ccon = connect(clisock, (SOCKADDR*)&cli, sizeof(cli));
    if (ccon == SOCKET_ERROR)
    {
        printf("\nConnect failed");
    }
    else
        printf("\nConnection with server success");
    csend = send(clisock, dirc, sizeof(dirc), 0);
    if (csend == SOCKET_ERROR)
    {
        printf("\nSend error");
    }
    else
        printf("\nDirectory Details sent=%dbytes", csend);
    for (;;)
    {
        crec = recv(clisock, recbuf, sizeof(recbuf) - 1, 0);
        if (crec == SOCKET_ERROR || crec == 0)
        {
            printf("\n%d", crec);
            printf("\nreceived failed");
            break;
        }
        recbuf[crec] = '\0';
        strcpy_s(a, sizeof(a), recbuf);
        arrPointer = &a;
        //arrPointer = recbuf;
        Arrofobj[i++] = createArray(env, crec, arrPointer);
        //printf("%s", recbuf);
        sum = sum + crec;
    }
    closesoc = closesocket(clisock);
    if (closesoc == SOCKET_ERROR)
    {
        printf("\nclose error");
    }
    else
        printf("\nclient closed");
    clean = WSACleanup();
    if (clean == SOCKET_ERROR)
    {
        printf("\nclean error");
    }
    else
        printf("\nclean success");
    printf("\nLine=%d", i);
    printf("\n TOtal bytes received=%d", sum);
    jobjectArray rows = (*env)->NewObjectArray(env, i, (*env)->GetObjectClass(env, Arrofobj[0]), 0);
    for (int k = 0; k < i; k++) {
        (*env)->SetObjectArrayElement(env, rows, k, Arrofobj[k]);
    }
    return rows;
    (*env)->ReleaseStringUTFChars(env, ip, ipc);
    (*env)->ReleaseStringUTFChars(env, dir, dirc);
    _getch();
}
