#include <stdio.h>

// this section will only be compiled on NON Windows platforms
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define closesocket close
typedef int SOCKET;

#else

// on Windows include and link these things
#include<WinSock2.h>
#include<cstdint>

#endif

int main(){
    SOCKET c;
    struct sockaddr_in server;
    uint16_t a, b, sum;
    
#ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
        printf("Error initializing the Windows Sockets Library\n");
        return -1;
    }
#endif

    c = socket(AF_INET, SOCK_STREAM, 0);
    if (c < 0){
        printf("Error creating client socket.\n");
        return 1;
    }

    memset(&server, 0, sizeof(server));
    server.sin_port = htons(1507);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("192.168.1.13");

    if (connect(c, (struct sockaddr *) &server, sizeof(server)) < 0){
        printf("Error connecting to server...\n");
        return 1;
    }

    printf("a= ");
    scanf("%hu", &a);
    printf("b= ");
    scanf("%hu", &b);
    a = htons(a);
    b = htons(b);
    send(c, &a, sizeof(a), 0);
    send(c, &b, sizeof(b), 0);
    recv(c, &sum, sizeof(sum), 0);
    sum = ntohs(sum);
    printf("The sum is %hu\n", sum);

}

