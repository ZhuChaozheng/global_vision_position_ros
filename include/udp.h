#ifndef UDP_H
#define UDP_H

#include <stdio.h>   
#include <string.h>   
#include <errno.h>   
#include <stdlib.h>   
#include <unistd.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h> 
#include <string>  
#include <iostream>
#include <vector>
#include "car.h"
  
#define DEST_PORT 10000
#define SERVER_PORT 8888
#define BUFF_LEN 1024

using namespace std;

class udp
{
public:
    int udp_init(const string ip, const int port);
    int send_data(const int sock_fd, 
            const char* send_buf, 
            const int len);
    void udp_server_init();
    void handle_udp_msg(int fd);
    vector<Car> get_cars_control_set() { return cars_control_set_; }
    void erase_cars_control_set() { cars_control_set_.erase(cars_control_set_.begin(), cars_control_set_.end()); }
private:     
    struct sockaddr_in addr_serv_;
    int len_;
    vector<Car> cars_control_set_;
};
#endif // UDP_H	
