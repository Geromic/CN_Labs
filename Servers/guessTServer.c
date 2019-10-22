/*
    Multi-threaded TCP server for number guessing game
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>

int winnerThread = -1;
int randInt;

void generateTag(int i, char buff[10]){
    sprintf(buff, "Guest%04d", i);
}

int generateRandom(){
    srand(time(0));
    return rand() % 65536;
}

struct node;

typedef struct{
    int sock;
    int tid;
    struct sockaddr_in address;
    int addr_len;
    char tag[10];
}connection_t;

void * connectionHandler(void *ptr){
    if (!ptr) pthread_exit(0);

    connection_t * conn = (connection_t *)ptr;
    int32_t nrGuesses = 1, r;

    while(winnerThread == -1){
        read(conn->sock, &r, sizeof(int));
        r = htonl(r);
        printf("%s: %d\n", conn->tag, r);
        if(r == randInt){
            char buffer[64];
            sprintf(buffer, "You won - %d tries", nrGuesses);
            send(conn->sock, buffer, sizeof(char)*strlen(buffer), 0);
            winnerThread = conn->tid;
        }
        else if(r < randInt) send(conn->sock, "Larger", sizeof(char)*6, 0); 
        else send(conn->sock, "Smaller", sizeof(char)*7, 0);
        nrGuesses++;
    }

    //if not winner thread
    if(winnerThread != conn->tid){
        char buffer[64];
        sprintf(buffer, "You lost - %d tries", nrGuesses);
        send(conn->sock, buffer, sizeof(char)*strlen(buffer), 0);
    }

    close(conn->sock);
    free(conn);
    pthread_exit(0);
}

int main() {
    int s;
    struct sockaddr_in server;  
    s = socket(PF_INET, SOCK_STREAM, 0);

    if (s < 0) {
        fprintf(stderr, "Error creating server socket\n");
        return 1;
    }

    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_port = htons(1507);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("192.168.1.9");

    if (bind(s, (struct sockaddr *) &server, sizeof(struct sockaddr_in)) < 0) {
        fprintf(stderr, "Bind error. Port already in use\n");
        return 1;
    }

    listen(s, 20);

    //server loop
    while(1){
        pthread_t thds[100];
        connection_t* conn;

        printf("Starting new session...\n");
        randInt = generateRandom();
        printf("Selecting random number: %d\n", randInt);

        int i=0;
        //new session loop
        while (1) {
            if(winnerThread != -1){
                printf("Ending session\n\n");
                winnerThread = -1;
                break;
            }

            conn = (connection_t *)malloc(sizeof(connection_t));
            conn->addr_len = sizeof(conn->address);
            conn->sock = accept(s, (struct sockaddr *)&conn->address, &conn->addr_len);
            conn->tid = i;
            fcntl(s, F_SETFL, O_NONBLOCK);

            if(conn->sock < 0){
                free(conn);
            }
            else{
                //assign tag to client
                char tag[10];
                generateTag(i+1, tag);
                strcpy(conn->tag, tag);
                printf("Client %s:%d has connected, assigning tag: %s\n",
                inet_ntoa(conn->address.sin_addr), ntohs(conn->address.sin_port), tag);

                //create thread
                pthread_create(&thds[i], 0, connectionHandler, (void*) conn);

                //send tag to client
                char buffer[128];
                sprintf(buffer, "Your tag is %s", tag);
                send(conn->sock, buffer, sizeof(char)*strlen(buffer), 0);
                i++;
            }
            sleep(0.1);
        }
    }
    
    close(s);
    return 0;
}
