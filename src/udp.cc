#include "udp.h"

int udp::udp_init(const string ip, const int port)
{
    /* Setup udp socket */ 
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  
    if(sock_fd < 0)  
    {  
        perror("socket");  
        exit(1);  
    }  
    /* Assign address */  
    memset(&addr_serv_, 0, sizeof(addr_serv_));
    addr_serv_.sin_family = AF_INET;  
    addr_serv_.sin_addr.s_addr = inet_addr(ip.data());  
    // addr_serv_.sin_port = htons(DEST_PORT_INTERFACE);  
    addr_serv_.sin_port = htons(port); 
    
    len_ = sizeof(addr_serv_); 
    return sock_fd;  
}

int udp::send_data(const int sock_fd, 
            const char* send_buf, const int len) 
{
    int send_num; 
    // cout << "send_buf: " << sizeof(send_buf) << endl;
    send_num = sendto(sock_fd, send_buf, 
        len, 0, (struct sockaddr *)&addr_serv_, len_);  

    if(send_num < 0)  
    {  
        perror("sendto error:");  
        exit(1);  
    }
    close(sock_fd);
    return 1;  
}

void char_2_vector_float(char *src, vector<float> &float_set)
{
    for(int i = 0; i < 200; i = i + 4)
    {
        char temp[4];
        temp[0] = src[i];
        temp[1] = src[i + 1];
        temp[2] = src[i + 2];
        temp[3] = src[i + 3];

        float *target = (float*)(&temp);
        float_set.push_back(*target);
        // cout <<"target " << *target << endl;
        // cout << "i " << i << endl;
    }
}

void udp::handle_udp_msg(int fd)
{
    char buf[BUFF_LEN];  //buff 1024byte
    socklen_t len;
    int count;
    struct sockaddr_in clent_addr;  //clent_addr, record the sender address
    while(1)
    {
        memset(buf, 0, BUFF_LEN);
        len = sizeof(clent_addr);
        //recvfrom is congestion function, until the data coming
        count = recvfrom(fd, buf, BUFF_LEN, 0, 
                (struct sockaddr*)&clent_addr, &len);  
        if(count == -1)
        {
            printf("recieve data fail!\n");
            return;
        }
        float *w;
        vector<float> float_set;
        char_2_vector_float(buf, float_set);
        // cout << "float_set.size(): " << float_set.size() << endl; // 50
        // convert vector float to vector car
        vector<Car> temp_set;
        for (auto iter = float_set.begin(); iter != float_set.end();)
        {
            Point2f target_point;
            target_point.x = (*iter);
            iter ++;
            target_point.y = (*iter);
            iter ++;
            float target_speed = (*iter);
            iter ++;
            float target_slope = (*iter);
            iter ++;
            float stop_flag = (*iter);
            iter ++;
            Car car;
            car.set_target_point(target_point);
            car.set_target_speed(target_speed);
            car.set_target_slope(target_slope);
            car.set_stop_flag(stop_flag);    
            temp_set.push_back(car);
        }
        cars_control_set_.swap(temp_set); // exchange vector
        cout << "cars_control_set.size(): " << cars_control_set_.size() << endl;
    }
}


void udp::udp_server_init()
{
    int server_fd, ret;
    struct sockaddr_in ser_addr;

    server_fd = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if(server_fd < 0)
    {
        printf("create socket fail!\n");
        return;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP
    ser_addr.sin_port = htons(SERVER_PORT);  //port

    ret = bind(server_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(ret < 0)
    {
        printf("socket bind fail!\n");
        return;
    }

    handle_udp_msg(server_fd);   // handle message

    close(server_fd);
}
