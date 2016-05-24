/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <string>

using std::queue;
using std::string;

sem_t command_sem;
queue<string> command_q;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void *command_dispatcher(void *socket_desc)
{
    int n;
    char buffer[256];
    int newsockfd = *(int*)socket_desc;
    sem_wait(&command_sem);
    string command = command_q.front();
    command_q.pop();
    n = write(newsockfd, command.c_str(), command.size());
    if (n < 0)
    {
        printf("ERROR writing to device\n");
        close(newsockfd);
        return NULL;
    }
    printf("delievered command: %s\n", command.c_str());
    n = read(newsockfd, buffer, 255);
    close(newsockfd);
}

void *connection_handler(void *socket_desc)
{
    int n;
    char buffer[256];
    int newsockfd = *(int*)socket_desc;
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0)
    {
        printf("ERROR reading from socket\n");
        close(newsockfd);
        return NULL;
    }
    buffer[n] = '\0';
    string new_command(buffer);
    command_q.push(new_command);
    sem_post(&command_sem);
    printf("command = %s\n", buffer);
    n = write(newsockfd,"Command received",16);
    if (n < 0)
    {
        printf("ERROR writing to socket\n");
    }
    close(newsockfd);
}

int main(int argc, char *argv[])
{
    char buffer[256];
    int sockfd, newsockfd, portno, n;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sem_init(&command_sem, 0, 0);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    while(sockfd < 0)
    {
        printf("ERROR opening socket\n");
        sleep(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    while (bind(sockfd, (struct sockaddr *) &serv_addr,
        sizeof(serv_addr)) < 0){
        printf("ERROR on binding\n");
        sleep(60);
    } 
    listen(sockfd,5);
    printf("initialized\n");
    pthread_t thread_id_ctl;
    pthread_t thread_id_dev;
    while(1)
    {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, 
            (struct sockaddr *) &cli_addr, 
            &clilen);
        while (newsockfd < 0) 
        {
            printf("ERROR on accept\n");
            sleep(1);
            continue;
        }
        n = read(newsockfd,buffer,255);
        if (n <= 0)
        {
            close(newsockfd);
            continue;
        }
        buffer[n] = '\0';
        if (strcmp(buffer, "device") == 0)
        {
            printf("get device\n");
            while( pthread_create( &thread_id_dev , NULL ,  command_dispatcher , (void*) &newsockfd) < 0)
            {
                printf("could not create thread\n");
                sleep(1);
            }
        } else if (strcmp(buffer, "controller") == 0)
        {
            printf("get controller\n");
            while( pthread_create( &thread_id_dev , NULL ,  connection_handler , (void*) &newsockfd) < 0)
            {
                printf("could not create thread\n");
                sleep(1);
            }
        }
    }
    close(sockfd);
    return 0; 
}
