#include "tcp.h"

void read_func(int connfd)
{
    printf("%d\n", connfd);
    unsigned char buff[MAX_SIZE];
    while(1) 
    {
        memset(buff, 0, sizeof(buff));
        int ret = read(connfd, buff, sizeof(buff));
        if(ret == 0) 
        {  
            printf("client close\n");
            break;
        }
        else if(ret == -1) 
        {
            return;
        }
        //printf("%d \n",buff[1]);
        update_status(connfd, buff, ret);
    }
}

// implement the check num algorithm
unsigned char check_num(unsigned char buff[], int num)
{
    unsigned char check_sum = 0;
    //Verify the data received
    for(int k = 0; k < num; k++)
    {
        check_sum = check_sum ^ buff[k];
    }
    return check_sum;
}

void update_status(int connfd, unsigned char buff[], int size)
{
    // checking the data verification
    if (size != 27)
        return;
    // head 7B 7B, tail 7D
    if ((buff[0] != 123) | (buff[1] != 123) | buff[26] != 125)
        return;

    // 7B 7B 01(id) 1B 00 00(5) 00(6) 00 00 05(9) 4F(10) FB 98 01 
    // 31 40 30 FF FA FF FF FF FD 2E DF CF 7D 
    int id = buff[2];
    car_[id].connfd = connfd;
    int temp = ((buff[9] << 8) + buff[10]);
    car_[id].velocity = (float)(((buff[5] << 8) + buff[6]) / 1000.0);
    // printf("temp %d \n", temp);
    car_[id].angular_velocity = (float)(((buff[9] << 8) + buff[10]) 
            / 1000.0);
    // printf("angular_velocity %f \n", car_[id].angular_velocity);
}

// void *write_test(void *arg)
// {
//     free(arg);
//     int marker = 3;
//     unsigned char buff[6]; // unsigned char is equal to hex
//     buff[0] = 125; // 7D
//     buff[1] = 122; // 7A
//     buff[2] = marker;
//     buff[3] = 0;
//     buff[4] = 0;
//     buff[5] = 123; // 7B
//     while(1)
//     {
//         for (int i = 0; i < 10; i++)
//         {
//             sleep(1);
//             int connfd = car_[i].connfd;
//             printf("connfd: %d\n", connfd);
//             if (connfd == 0)
//                 continue;
//             write(connfd, buff, sizeof(buff));
//             printf("accomplish reply!\n");
//         }
//     }
// }

void write_func(int marker, unsigned char buff[], int size)
{
    int connfd = car_[marker].connfd;
    if (connfd == 0)
        return;
    write(connfd, buff, size);
   // std::cout << "send sucess!!" << std::endl;
}

void *usethread(void *arg)
{
    int connfd=*(int *)arg;
    pthread_detach(pthread_self());
    free(arg);
    read_func(connfd);
    close(connfd);
    pthread_exit((void *)0);
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
    // pthread_t write_pid;
    // pthread_create(&write_pid, NULL, write_test, NULL); 
    pthread_t pid;
    while(1)
    {
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
    }
    return 0;
}