#ifndef SOCKET_H
#define SOCKET_H


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>  

#define PORT 4242
#define MAX_CLIENT 3

void ConnectToServ(int sock, const char *address);
void SendMessage(int sock, unsigned char *msg, int size);
int  ReadMessage(int sock, unsigned char **buf);
void bind_to_port(int sock);

int CreateSocket(void);

#endif