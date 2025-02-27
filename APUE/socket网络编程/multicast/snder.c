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
    struct ip_mreqn mreq;
    inet_pton(AF_INET, MULTGROUT, &mreq.imr_multiaddr);
    inet_pton(AF_INET, "0.0.0.0",&mreq.imr_address);
    mreq.imr_ifindex = if_nametoindex("eth0");
    if(setsockopt(sd,IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0){
        perror("setsockopt()");
        exit(1);
    }
    // bind 可以不要Bind
    strcpy(sbuf.name , "alle");
    sbuf.math = htonl(rand()%100);
    sbuf.chinese = htonl(rand()%100);

    raddr.sin_family= AF_INET;
    raddr.sin_port = htons(atoi(RCVPORT));
    inet_pton(AF_INET, MULTGROUT, &raddr.sin_addr); // ip->大整数
    // 发送sbuf内容到rad.dr的ip和端口
    if(sendto(sd, &sbuf, sizeof(sbuf), 0,(void *)&raddr, sizeof(raddr))<0)
        {
            perror("sendto()");
            exit(1);
        }
    puts("OK");
    close(sd);
    exit(0);
}