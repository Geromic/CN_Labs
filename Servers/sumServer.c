#include <stdio.h>

// this section will only be compiled on NON Windows platforms
#ifndef WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>

#define closesocket close
typedef int SOCKET;

#else

// on Windows include and link these things
#include<WinSock2.h>
#include<cstdint>

#endif

int main() {
    SOCKET sock;
    struct sockaddr_in server, client;
    int c, l, err;
    
#ifdef WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) < 0) {
        printf("Error initializing the Windows Sockets Library\n");
        return -1;
    }
#endif

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        printf("Error creating socket\n");
        return 1;
    }
    memset(&server, 0, sizeof(server));
    server.sin_port = htons(1507);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("192.168.1.13");

    if(bind(sock, (struct sockaddr *) &server, sizeof(server)) < 0){
        perror("Bind error\n");
        return 1;
    }

    listen(sock, 5);

    l = sizeof(client);
    memset(&client, 0, sizeof(client));
    while(1){
        uint16_t a, b, sum;
        printf("Waiting for connection...\n");
        c = accept(sock, (struct sockaddr *) &client, &l);
        err = errno;

#ifdef WIN32
        err = WSAGetLastError();
#endif

        if(c < 0){
            printf("accept error: %d", err);
            continue;
        }

        printf("Client has connected: %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        int res = recv(c, (char*)&a, sizeof(a), 0);
        if(res != sizeof(a)) printf("Error receiving operand!\n");
        res = recv(c, (char*)&b, sizeof(b), 0);
        if(res != sizeof(b)) printf("Error receiving operand!\n");

        a = ntohs(a);
        b = ntohs(b);

        sum = a+b;
        sum = htons(sum);
        res = send(c, (char*)&sum, sizeof(sum), 0);
        
        if(res != sizeof(sum))
            printf("Error sending result\n");
        
        closesocket(c);
    }
#ifdef WIN32
    WSACleanup();
#endif
}