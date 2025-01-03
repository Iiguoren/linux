#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../../include/proto.h"
#include "../../include/site_type.h"
#include <getopt.h>
#include <net/if.h>
#include "client.h"
/*
-M --mgroup指定多播组
-P --port指定接受端口
-p --player指定播放器
-H --help显示帮助
*/
struct client_conf_st client_conf = {.rcvport = DEFAULT_RCVPORT,
                                     .mgroup = DEFAULT_MGROUP,
                                     .player_cmd = DEFAULT_PLAYERCMD};
static void printhelp(void){
    printf("-P --port 指定接收端口\n-M --mgroup 指定多播组\n\
    -p --player 指定播放器命令行\n -H --help 显示帮助\n");
}
// 向缓冲区写入n个字节,这个很重要
static ssize_t writen(int fd, const char *buf, size_t count){
    int ret;
    int pos = 0;
    while(count>0){
        ret = write(fd, buf+pos, count);
        if(ret< 0)
        {
            if(errno == EINTR)
                continue;
            perror("writen()");
            return -1;
            
        }
        count -= ret;
        pos += ret;
    }
    return pos;
}
int main(int argc, char **argv){
/* 初始化 
级别：默认值<配置文件<环境变量<命令行参数
*/
    int val=1;
    int index = 0;
    int c;
    int sd;

    int pd[2];
    int len;
    int ret;
    pid_t pid;
    struct sockaddr_in laddr, serveraddr,raddr;
    socklen_t serveraddr_len, raddr_len;
    struct ip_mreqn mreqn;
    int chosen_id;
    // 定义一个用于存储命令行选项的结构体数组
    // 最后一个元素 {NULL, 0, NULL, 0} 是一个特殊的标记，通常用于表示选项数组的结束。
    struct option argarr[] = {{"port",1,NULL, 'P'},{"mgroup",1,NULL,'M'},\
                                {"player",1,NULL,'p'},{"help",0,NULL,'H'},{NULL,0,NULL,0}};
    while(1){
        int c = getopt_long(argc, argv, "P:M:p:H", argarr,&index);
        if(c < 0 )
            break;
        switch(c){
            case'P':
                client_conf.rcvport = optarg;
                break;
            case'M':
                client_conf.mgroup = optarg;
                break;
            case'p':
                client_conf.player_cmd = optarg;
                break;
            case'H':
                printhelp();
                exit(0);
                break;
            default:
                // 结束自己
                abort();
                break;
            }
    }

    sd = socket(AF_INET, SOCK_DGRAM, 0);/*UDP*/
    if(sd<0){
        perror("socket()");
        exit(1);
    }
    // 将多播组地址放入结构体中
    if(inet_pton(AF_INET, client_conf.mgroup, &mreqn.imr_multiaddr)<0)
    {
        perror("inet_pton()");
        exit(1);
    }
    // 加入多播组的地址放入结构体
    if(inet_pton(AF_INET, "0.0.0.0",&mreqn.imr_address)<0)
    {
        perror("inet_pton()");
        exit(1);
    }
    mreqn.imr_ifindex=if_nametoindex("eth0");
    // 加入多播组成员
    if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreqn,sizeof(mreqn))<0)
    {
        perror("setsockopt()");
        exit(1);
    }
    if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP,&val,sizeof(val))<0)
    {
        perror("setsockopt()");
        exit(1);
    }
    // 配置本地addr
    laddr.sin_family =AF_INET;
    // 进行格式转换,host to network short
    laddr.sin_port = htons((atoi(client_conf.rcvport)));
    inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
    // bind需要一个addr_in的结构体，放入绑定的端口和IP
    if(bind(sd, (void *)&laddr, sizeof(laddr)))
    {
        perror("bind()");
        exit(1);
    }
    // pipe函数创建一个pipe
    if(pipe(pd)<0)
    {
        perror("pipe()");
        exit(1);
    }

    pid = fork();
    if(pid<0){
        perror("fork()");
        exit(1);
    }
    if(pid == 0){
    // 如果子进程，负责调解码器
    // 不需要使用套接字描述符sd和管道描述符的输入端
        close(sd);
        close(pd[1]);//0读1写
        dup2(pd[0], 0); // 将文件描述符 pd[0] 复制到文件描述符 0 (标准输入)
        if(pd[0]>0)
            close(pd[0]);
            // 使用player_cmd接受标准输入的流文件处理
        if(execl("/bin/sh", "sh", "-c", client_conf.player_cmd,NULL)<0){
        // 如果execl失败
        perror("execl()");
        exit(1);
        }
    }   
    else{
    // 如果是父进程，从网络收包，发送给子进程
    // 收节目单->选择频道->收频道包->发送给子进程
        struct msg_list_st *msg_list;
        msg_list = malloc(MSG_LIST_MAX);
        if(msg_list == NULL)
        {
            perror("malloc()");
            exit(1);
        }
        while(1){
            // 接收,存入msg_list
            len = recvfrom(sd, msg_list, MSG_LIST_MAX, 0, (void *)&serveraddr, &serveraddr_len);
            if(len < sizeof(struct msg_list_st))
            {
                fprintf(stderr, "message is too small.\n");
                continue;
            }
            if(msg_list->chnid != LISTCHNID){
                fprintf(stderr, "chnid is not match.\n");
                continue;
            }
            break;
        }
        struct msg_listentry_st *pos;
        // 强转当作单字节,会以字节单位处理内存地址,pos为msg_list的entry头地址((char *)msg_list) + len)为尾地址
        for(pos = msg_list->entry; (char *)pos< (((char *)msg_list) + len);pos = (void *)(((char *)pos)+ntohs(pos->len))){
            printf("%d:%s\n", pos->chnid, pos->desc);
        }
        free(msg_list);
        while(1)
        {   
            ret = scanf("%d", &chosen_id);
            if(ret != 1)
                exit(1);
        };
        
        // 收频道包发送给子进程
        struct msg_channel_st *msg_channel;
        msg_channel = malloc(MSG_CHANNEL_MAX);
        if(msg_channel == NULL){
            perror("malloc");
            exit(1);
        }
        while(1){
            len = recvfrom(sd, msg_channel, MSG_CHANNEL_MAX, 0, (void *)&raddr, &raddr_len);
            if(raddr.sin_addr.s_addr != serveraddr.sin_addr.s_addr||
            raddr.sin_port != serveraddr.sin_port) // 判断地址
            {
                fprintf(stderr, "Ignore:address not match\n");
                continue;
            }
            if(len < sizeof(struct msg_channel_st))
            {
                fprintf(stderr, "Ignore:message is too small.\n");
                continue;
            }
            if(msg_channel->chnid == chosen_id)
            {
                if(writen(pd[1], msg_channel->data, len-sizeof(chnid_t))<0)
                    exit(1);
            }        
        }
        free(msg_channel);
        close(sd);

        exit(0);
    }
    
}
