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

int main(int argc, char *argv[]){
    int sd;
    struct sockaddr_in raddr;
    long long stamp;
    FILE *fp;
    if(argc <2){
        printf("usage:..\n");
        exit(1);
    }
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd<0){
        perror("socket()");
        exit(1);
    }

    //bind()
    raddr.sin_family = AF_INET;
    // 接受服务器数据从网络端到主机端
    raddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, argv[1],&raddr.sin_addr);
    if(connect(sd, (void *)&raddr, sizeof(raddr))<0){
        perror("connect()");
        exit(1);
    }
    fp = fdopen(sd, "r+");
    if(fp == NULL){
        perror("fdopen()");
        exit(1);
    }
    if(fscanf(fp, FMT_STAMP, &stamp)<1){

    }
    else
        fprintf(stdout, "stamp = %lld\n", stamp);
    fclose(fp);
    //rcev()
    //close();
    exit(0);

}