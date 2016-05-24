#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "handle.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    if (handle_init() == -1)
    {
        error("failed to init GPIO\n");
        exit(0);
    }
    while(1)
    {
        portno = atoi(argv[2]);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) 
        {
            printf("ERROR opening socket\n");
            sleep(1);
            continue;
        }
        server = gethostbyname(argv[1]);
        if (server == NULL) 
        {
            printf("ERROR, no such host\n");
            close(sockfd);
            sleep(1);
            continue;
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr, 
             (char *)&serv_addr.sin_addr.s_addr,
             server->h_length);
        serv_addr.sin_port = htons(portno);
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        {
            printf("ERROR connecting\n");
            close(sockfd);
            sleep(2);
            continue;
        }
        n = write(sockfd, "device", 6);
        if (n <= 0)
        {
            printf("error transmiting\n");
            close(sockfd);
            sleep(1);
            continue;
        }
        n = read(sockfd, buffer, 255);
        if (n <= 0)
        {
            printf("error transmiting\n");
            close(sockfd);
            sleep(1);
            continue;
        }
        buffer[n] = '\0';
        n = write(sockfd, "received", 8);
        if (n <= 0)
        {
            printf("error transmiting\n");
            close(sockfd);
            sleep(1);
            continue;
        }
        close(sockfd);
        if (handle(buffer) == 0)
            break;
    }
    return 0;
}
