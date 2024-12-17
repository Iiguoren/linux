#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "proto.h"

int main(){
    int sd; // 文件描述符
    struct sockaddr_in laddr; // 本地地址和端口
    struct sockaddr_in raddr; // 网络地址和端口
    struct msg_st rbuf;
    char ip[1024];
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sd < 0){
        perror("socket()");
        exit(1);
    }
    int val = 1;
    if(setsockopt(sd,SOL_SOCKET,SO_BROADCAST,&val,sizeof(val)) < 0){
        perror("setsockopt()");
        exit(1);
    }
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(RCVPORT)); // 字符串转Int同时port需要传到network端，将本地字节序转网络字节序
    // "0.0.0.0"通用地址，转换为自己的IP
    
    inet_pton(AF_INET, "0.0.0.0" ,&laddr.sin_addr); // 要求大整数形式，将ipv4下的地址转为大整数
    // 将地址和端口绑定到文件描述符中
    if(bind(sd, (void *)&laddr, sizeof(laddr))<0) // strcut类型不一致需要强制转换
    {
        perror("bind()");
        exit(1);
    }
    socklen_t raddr_len = sizeof(raddr); //初始化必须
    while (1)
    {
        // 用套接字sd接受raddr传输的数据，存储在rbuf中,接受其ip和端口存储在raddr
        // 报式套接字每次通信都需要知道对方是谁
       recvfrom(sd, &rbuf,sizeof(rbuf),0, (void *)&raddr, &raddr_len );
       inet_ntop(AF_INET,&raddr.sin_addr,ip,1024);
       printf("massage from %s:%d\n",ip, ntohs(raddr.sin_port));
       printf("name = %s\n",rbuf.name);
       printf("math = %d\n", ntohl(rbuf.math));
       printf("chinese = %d\n", ntohl(rbuf.chinese));
    }
    close(sd);
    exit(0);
}