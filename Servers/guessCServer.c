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

int randInt;
int c;

void time_out(int semnal){
    int32_t r = -1;
    r = htonl(r);
    printf("Time out.\n");
    send(c, &r, sizeof(int32_t), 0);
    close(c);
    exit(1);
}

void generateRandom(){
    srand(time(0));
    randInt = rand() % 256;
}

void serveClient() {
    int cod;
    int32_t r;

    struct sockaddr_in server;

    if (c < 0) {
        fprintf(stderr, "Eroare la stabilirea conexiunii cu clientul.\n");
        exit(1);
    }
    else printf("Un nou client s-a conectat cu succes.\n");

    signal(SIGALRM, time_out);
    alarm(10);

    int nrGuesses = 0;
    while(1){
        recv(c, &r, sizeof(r), MSG_WAITALL);
        alarm(10);
        r = ntohl(r);
        nrGuesses++;
        printf("guess: %d, nr: %d", r, randInt);
        if(r == randInt){
          send(c, "W", sizeof(char), 0);
          //break;
        }
        else if(r < randInt) send(c, "L", sizeof(char), 0); 
        else send(c, "S", sizeof(char), 0);
    }
    close(c);
    exit(0);
}

int main() {
    int s, l, cod;
    struct sockaddr_in client, server;  

    s = socket(PF_INET, SOCK_STREAM, 0);

    if (s < 0) {
        fprintf(stderr, "Eroare la creare socket server.\n");
        return 1;
    }

    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(1507);
    server.sin_addr.s_addr = inet_addr("192.168.1.13");

    cod = bind(s, (struct sockaddr *) &server, sizeof(struct sockaddr_in));

    if (cod < 0) {
        fprintf(stderr, "Eroare la bind. Portul este deja folosit.\n");
        return 1;
    }

    listen(s, 5);
    
    generateRandom();
    while (1) { // deserveste oricati clienti
        memset(&client, 0, sizeof(client));
        l = sizeof(client);

        printf("Astept sa se conecteze un client.\n");
        c = accept(s, (struct sockaddr *) &client, &l);
        printf("S-a conectat clientul cu adresa %s si portul %d.\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        if (fork() == 0) { // server concurent, conexiunea va fi tratata de catre un proces fiu separat
            serveClient();
        }
        // parintele continua bucla while asteptand un nou client
    }
}
