#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include "proto.h"
int main(int argc, char* argv[]){
    struct msg_st sbuf;
    int sd;
    struct sockaddr_in raddr;
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("socket()");
        exit(1);
    }
    // bind 可以不要Bind
    strcpy(sbuf.name , "alle");
    sbuf.math = htonl(rand()%100);
    sbuf.chinese = htonl(rand()%100);

    raddr.sin_family= AF_INET;
    raddr.sin_port = htons(atoi(RCVPORT));
    inet_pton(AF_INET, argv[1], &raddr.sin_addr); // ip->大整数
    // 发送sbuf内容到raddr的ip和端口
    if(sendto(sd, &sbuf, sizeof(sbuf), 0,(void *)&raddr, sizeof(raddr))<0)
        {
            perror("sendto()");
            exit(1);
        }
    puts("OK");
    close(sd);
    exit(0);
}