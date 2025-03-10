#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "proto.h"
#include <string.h>
int main(int argc, char* argv[]){
    struct msg_st *sbufp;
    int sd;
    struct sockaddr_in raddr;
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("socket()");
        exit(1);
    }
    if(argc <3){
        fprintf(stderr, "usage..\n");
        exit(1);
    }
    if(strlen(argv[2])>NAMEMAX);
    {
        fprintf(stderr, "name too long..\n");
        exit(1);
    }
    int size = sizeof(struct msg_st) + strlen(argv[2]);
    sbufp = malloc(size);
    // bind 可以不要Bind
    strcpy(sbuf->name , "alle");
    sbuff->math = htonl(rand()%100);
    sbuff->chinese = htonl(rand()%100);

    raddr.sin_family= AF_INET;
    raddr.sin_port = htons(atoi(RCVPORT));
    inet_pton(AF_INET, argv[1], &raddr.sin_addr); // ip->大整数
    // 发送sbuf内容到raddr的ip和端口
    if(sendto(sd, sbufp, size, 0,(void *)&raddr, sizeof(raddr))<0)
        {
            perror("sendto()");
            exit(1);
        }
    close();
    exit(0);
}