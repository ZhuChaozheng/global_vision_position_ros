#include "tcp.h"

void read_func(int sockfd)
{
    char buff[MAX_SIZE];
    while(1) 
    {
        memset(buff, 0, sizeof(buff));
        int ret = read(sockfd, buff, sizeof(buff));
        printf("wait for ...");
        if(ret == 0) 
        {  
            printf("client close\n");
            break;
        }
        else if(ret == -1) 
        {
            exit(0);
        }
        fputs(buff, stdout);
        update_status(sockfd, buff, ret);
        // write(conn, recvbuf, ret);
    }
}

void update_status(int sockfd, char buff[], int size)
{
    printf("update_status size: %d \t", size);
    int id = buff[2];
    car_[id].sockfd = sockfd;
    car_[id].velocity = float((buff[5] << 8 + buff[6]) / 1000);
    car_[id].angular_velocity = float((buff[9] << 8 + buff[10]) / 1000);
    printf("update_status data: %f \t", car_[id].angular_velocity);
}

void write_func(int sockfd, char buff[])
{
    write(sockfd, buff, strlen(buff));
}

void *usethread(void *arg)
{
    int connfd=*(int *)arg;
    printf("From client usethread A");
    // pthread_detach(pthread_self());
    free(arg);
    printf("From client usethread");
    read_func(connfd);
    close(connfd);
    return NULL;
}

int create_server_and_update_data()
{
    int sockfd;
    socklen_t len;
    struct sockaddr_in servaddr, cli, cliaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");    
    pthread_t pid;
    // while(1)
    // {
        int *connfd=new int();
        // Accept the data packet from client and verification
        len = sizeof(cliaddr);
        *connfd = accept(sockfd, (struct sockaddr*) &cliaddr, &len);
        // *connfd = accept(sockfd, (SA*)&cli, &len);
        if (*connfd < 0) {
            printf("server accept failed...\n");
            exit(0);
        }
        else
            printf("server accept the client...\n");
        pthread_create(&pid, NULL, usethread, connfd); 
    // }
    // return 0;
}
