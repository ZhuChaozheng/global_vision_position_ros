#ifndef TCP_H
#define TCP_H

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#define MAX_SIZE 80
#define PORT 8234
typedef struct _car
{
    int connfd;
    float velocity;
    float angular_velocity;
}car;

car car_[10];

void read_func(int sockfd);

void update_status(int sockfd, unsigned char buff[], int size);

void write_func(int marker, unsigned char buff[], int size);

unsigned char check_num(unsigned char buff[], int num);

void *usethread(void *arg);

int create_server_and_update_data();
#endif // TCP_H	