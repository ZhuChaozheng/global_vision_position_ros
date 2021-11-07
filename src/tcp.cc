#include "tcp.h"

void read_func(int connfd)
{
    // printf("%d\n", connfd);
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
        printf("connfd %d receive %d\n",connfd, buff[1]);
        update_status(connfd, buff, ret);
    }
}

void update_status(int connfd, unsigned char buff[], int size)
{
    // 7B 7B 01(id) 1B 00 00(5) 00(6) 00 00 00(9) 00(10) FB 98 01 
    // 31 40 30 FF FA FF FF FF FD 2E DF CF 7D 
    int id = buff[2];
   // printf("%d \n",buff[2]);
    car_[id].connfd = connfd;
    car_[id].velocity = (float)((buff[5] << 8) + buff[6]) / 1000;
    car_[id].angular_velocity = (float) (buff[9] << 8 + buff[10]) 
            / 1000;
}

void write_func(int marker, unsigned char buff[], int size)
{
    int connfd = car_[marker].connfd;
    if (connfd == 0)
        return;
    write(connfd, buff, size);
   // std::cout << "send sucess!!" << std::endl;
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;
    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
}

int create_server_and_update_data()
{
    int listenfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    void sig_chld(int);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    listen(listenfd, LISTENQ);
    signal(SIGCHLD, sig_chld);/*must call waitpid()*/
    for( ; ; )
    {
        int connfd;
        clilen = sizeof(cliaddr);
        if((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, 
                &clilen)) < 0 )
        {
            if(errno == EINTR)
                continue;/*back to for()*/
            else
                printf("accept error\n");
        }
        if((childpid = fork()) == 0)
        {   /*child process*/
            printf("accept a new connection\n");
            close(listenfd);/*close listening socket*/
            read_func(connfd);/*process the request*/
            exit(0);
        }
        // close(connfd);/*parent closes connected socket*/
    }
    return 0;
}
