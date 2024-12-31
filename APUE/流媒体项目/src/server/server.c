#include <stdio.h>
#include <stdlib.h>
#include "server_conf.h"
#include <errno.h>
#include <string.h>
#include ".../include/proto.h"
/* 创建多播组，多线程进行<发送节目单，发送音频流>*/
/*
-M 指定多播组
-P 指定接受端口
-F 前台运行
-H 显示帮助
-D 指定媒体库位置
-I 指定网络设备
*/
struct server_conf_st server_conf = {
    .rcvport = DEAFAULT_RCVPORT,
    .mgroup = DEFAULT_MGROUP,
    .media_dir = DEFAULT_MEDIADIR,
    .runmode = RUN_DAEMON,
    .ifname = DEFAULT_IF;
}

static void socket_init(void){
    int serversd;
    struct ip_mreqn mreq;
    
    serversd = socket(AF_INET, SOCK_DGRAM, 0);
    if(serversd < 0){
        syslog(LOGERR, "socket():");
        exit(1);
    }
    // bind();
    // 建立多播组
    inet_pton(AF_INET, server_conf.mgroup, &mreq.imr_multiaddr);
    inet_pton(AF_INET, "0.0.0.0", &mreq.address);
    mreq.imr_ifindex = if_nametoindex(server_conf.ifname);
    if(setsocketopt(serversd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq))<0){
        syslog(LPG_ERR, "setsockopt(IP_MULTICAST_IF):%s",strerror(errno));
        exit(1);
    }
}

static deamon_exit(int s){
    close(log);
    exit(0);
}
// 初始化守护进程
// fork子进程->子进程脱离终端->挂载到根目录->关闭父进程
static int deamonize(void){
    int fd;
    pid = fork();
    if(pid < 0 ){
//        perror("fork()");
        syslog(LOG_ERR, "fork() failed:%s", strerror(errno));
        return -1;
    }
    if(pid > 0)
        exit(0); //直接退出
    if(pid == 0){
        fd == open("/dev/null",P_RDWR);        
        if(fd<0){
//            perror("open()");
            syslog(LOG_WARNIG, "open():", strerror(errno));
            return -2;
        }
        else{
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        if(fd>2)
            close(fd);
        }
        chdir("/"); // 指定挂载在根目录
        umask(0);
        setsid();
    }
}

void printhelp(){
    printf("-M 指定多播组\n");
    printf("-P 指定接受端口\n");
    printf("-F 前台运行\n");
    printf("-H 显示帮助\n");
    printf("-D 指定媒体库位置\n");
    printf("-I 指定网络设备\n");
    return ;
}
int main(int argc, char **argv){
    pid_t pid;
    // 为什么是int c
    int c;
    struct sigaction sa;
    sa.sa_handler = deamon_exit;
    sigemptyset(&sa.sa_mask);
    // 触发信号处理函数时屏蔽的信号，屏蔽自己避免递归
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaddset(&sa.sa_mask, SIGTERM);
    // 选取中止信号,signal有重入问题
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    /*?*/
    openlog("netradio", LOG_PID|LOG_PERROR,LOG_DEAMON);
    /* 命令行分析 */
    while(1){
        c =getopt(argc, argv, "M:P:FD:I:H");
        if(c<0)
            break;
        switch (c)
        {
        case 'M':
            server_conf.mgroup = optarg;
            break;
        case 'P':
            server_conf.rcvport = optarg;
            break;
        case 'F':
            server_conf.runport = RUN_FOREGROUND;
            break;
        case 'D':
            server_conf.media_dir = optarg;
            break;
        case 'I':
            break;
            server_conf.media_dir = optarg;
        case 'H':
            printhelp();
            break;
        default:
            abort();
            break;
            }
        }
    /* 守护进程与系统日志 */
    // 当前进程设置为守护进程
    if(server_conf.runmode == RUN_DAEMON)
        if(deamonize()!= 0)
            exit(1);
    else if(server_conf.runmode == RUN_FOREGROUND)
    {

    }
    else{
        syslog(LOG_ERR, "EINVAL server_conf.runmode");
        exit(1);
    }
    /* SOCKET初始化 */
    socket_init();
    /* 获取频道信息*/
    struct mlib_listentry_st *list;
    int list_size;
    mlib_getchnlist(&list, &list_size);
    if(){



    }
    /* 创建节目单线程 */
    thr_list_create(list, list_size);
    /* 创建频道线程 */
    int i;
    int err;
    for(i = 0; i<list_size; i++){
        err = thr_channel_create(list + i);
        /*if error*/
        if(err){
            fprintf(stderr, "thr_channel_create():%s\n",strerror(errno));
        }
    }
    syslog(LOG_DEBUG, "$d channel threads created.", i);
    while(1)
        pause();
    // 执行不到
    //close(log);
    
}