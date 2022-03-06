#include "tcp.h"

void read_func(int connfd) {
  printf("%d\n", connfd);
  unsigned char buff[MAX_SIZE];
  while (1) {
    memset(buff, 0, sizeof(buff));
    int ret = read(connfd, buff, sizeof(buff));
    if (ret == 0) {
      printf("client close\n");
      break;
    } else if (ret == -1) {
      return;
    }
    // printf("%d \n",buff[1]);
    update_status(connfd, buff, ret);
  }
}

// implement the check num algorithm
unsigned char check_num(unsigned char buff[], int num) {
  unsigned char check_sum = 0;
  // Verify the data received
  for (int k = 0; k < num; k++) {
    check_sum = check_sum ^ buff[k];
  }
  return check_sum;
}
/*
 * update status from 35 bytes data
 */
void update_status(int connfd, unsigned char buff[], int size) {
  // checking the data verification
  // if (size != 27)
  if (size != 35) return;
  // head 7B 7B, tail 7D
  // if ((buff[0] != 123) | (buff[1] != 123) | buff[26] != 125
  //     | buff[34] != 125)
  if ((buff[0] != 123) | (buff[1] != 123) | buff[34] != 125) return;

  // 7B 7B 01(id) 1B 00 00(5) 00(6) 00 00 05(9) 4F(10)
  // FB 98 01 31 40 30 FF FA FF FF FF FD 2E DF CF 7D

  int id = buff[2];
  car_[id].connfd = connfd;
  int temp56 = (buff[5] << 8) + buff[6];
  if (temp56 > 32768) temp56 = temp56 - 65525;
  car_[id].velocity = (float)(temp56 / 1000.0);
  // printf("temp %d \n", temp);
  // 9 10 angular_velocity by calculation
  int temp910 = (buff[9] << 8) + buff[10];
  if (temp910 > 32768) temp910 = temp910 - 65525;
  car_[id].angular_velocity = (float)(temp910 / 1000.0);
  // printf("angular_velocity %f \n", car_[id].angular_velocity);
  // 25-26 left wheel velocity
  int temp2526 = (buff[25] << 8) + buff[26];
  if (temp2526 > 32768) temp2526 = temp2526 - 65525;
  car_[id].left_wheel_velocity = (float)(temp2526 / 1000.0);
  // 27-28 right wheel velocity
  int temp2728 = (buff[27] << 8) + buff[28];
  if (temp2728 > 32768) temp2728 = temp2728 - 65525;
  car_[id].right_wheel_velocity = (float)(temp2728 / 1000.0);
  // 11 12 acc_x
  int temp1112 = (buff[11] << 8) + buff[12];
  if (temp1112 > 32768) temp1112 = temp1112 - 65525;
  car_[id].acc_x = (float)(temp1112 / 1672.0);
  // 13 14 acc_y
  int temp1314 = (buff[13] << 8) + buff[14];
  if (temp1314 > 32768) temp1314 = temp1314 - 65525;
  car_[id].acc_y = (float)(temp1314 / 1672.0);
  // 21 22 gyro_z
  int temp2122 = (buff[21] << 8) + buff[22];
  if (temp2122 > 32768) temp2122 = temp2122 - 65525;
  car_[id].gyro_z = (float)(temp2122 / 3753.0);
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

void write_func(int marker, unsigned char buff[], int size) {
  int connfd = car_[marker].connfd;
  if (connfd == 0) return;
  write(connfd, buff, size);
  // std::cout << "send sucess!!" << std::endl;
}

void *usethread(void *arg) {
  int connfd = *(int *)arg;
  pthread_detach(pthread_self());
  free(arg);
  read_func(connfd);
  close(connfd);
  pthread_exit((void *)0);
  return NULL;
}

int create_server_and_update_data() {
  int sockfd;
  socklen_t len;
  struct sockaddr_in servaddr, cli, cliaddr;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
    printf("socket bind failed...\n");
    exit(0);
  } else
    printf("Socket successfully binded..\n");

  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  } else
    printf("Server listening..\n");
  // pthread_t write_pid;
  // pthread_create(&write_pid, NULL, write_test, NULL);
  pthread_t pid;
  while (1) {
    int *connfd = new int();
    // Accept the data packet from client and verification
    len = sizeof(cliaddr);
    *connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
    // *connfd = accept(sockfd, (SA*)&cli, &len);
    if (*connfd < 0) {
      printf("server accept failed...\n");
      exit(0);
    } else
      printf("server accept the client...\n");
    pthread_create(&pid, NULL, usethread, connfd);
  }
  return 0;
}