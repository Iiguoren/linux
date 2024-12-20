#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <sys/mman.h>
#include <errno.h>

#include "proto.h"
#define IPSIZE 1024
#define BUFSIZE 1024

#define MINSPARESERVER 5
#define MAXSPARESERVER 10
#define MAXCLIENT 20
/*设置自定义信号*/
#define SIG_NOTIFY SIGUSR1
#define IPSTRSIZE 1024
enum
{
    STATE_IDEL = 0,
    STATE_BUZY
};
struct service_st
{
    pid_t pid;
    int state;
    //int reuse;
};
static struct service_st *serverpool;
static int sd;
static int idle_count = 0, buzy_count = 0;
static void server_job(int pos){
    int ppid;
    int client_sd;
    char ip[IPSIZE];
    struct sockaddr_in raddr;
    socklen_t raddr_len = sizeof(raddr);
    ppid = getppid(); // 获取父进程PID
    while(1){
        serverpool[pos].state = STATE_IDEL;
        kill(ppid, SIG_NOTIFY);
        client_sd = accept(sd, (void *)&raddr, &raddr_len);
        if(client_sd < 0){
            if(errno != EINTR||errno !=EAGAIN)
            {
                perror("accept()");
                exit(1);
            }
        }
        serverpool[pos].state = STATE_BUZY;
        kill(ppid, SIG_NOTIFY); // 为什么要查看状态？
        inet_ntop(AF_INET, &raddr.sin_addr, ip, IPSIZE);
        //printf("[%d]client:%s:%d\n",getpid(),ntohs(raddr.sin_addr));
        long long stamp = time(NULL);
        int len;
        char buf[BUFSIZE];
        len = snprintf(buf, BUFSIZE, FMT_STAMP, stamp);
        

        
        if (send(client_sd,buf,len,0) < 0){
            perror("send()");
            exit(1);
        }
        /*if error*/
        sleep(5);
        close(client_sd);
    }

}

static int add_one_server(void){
    int slot;
    pid_t pid;
    if(idle_count + buzy_count >= MAXCLIENT)
        return -1;
    // 还有client空位，找到空位
    for(slot = 0; slot < MAXCLIENT; slot++){
        if(serverpool[slot].pid == -1)
            break;
    }
    // 空位赋值
    serverpool[slot].state = STATE_IDEL;
    pid= fork();
    if(pid<0){
        perror("fork()");
        exit(1);
    }
    if(pid==0){
       server_job(slot);
       exit(0);
    }
    else
    // 父进程
    {
       serverpool[slot].pid = pid;
        idle_count++;
    }
    return 0;
}
static void scan_pool(void){
    int i;
    int idle = 0, buzy = 0;
    
    for(i = 0; i<MAXCLIENT;i++){
        // 如果当前空间没有启用
        if(serverpool[i].pid == -1){
            continue;
        }
        // 如果当前进程已经销毁，修改pid状态
        if(kill(serverpool[i].pid, 0)){
            serverpool[i].pid =-1;
            continue;
        }
        //统计进程池的状态
        if(serverpool[i].state == STATE_IDEL)
          idle++;
        else if(serverpool[i].state == STATE_BUZY)
          buzy++;
        else{
            fprintf(stderr,"未知状态!\n");
            abort();
            //或者用_exit(1);
        }
    }
    idle_count = idle;
    buzy_count = buzy;
}
static int del_one_server(void){
    int i;
    // 没有成员
    if(idle_count == 0){
        return -1;
    }
    for(i = 0; i<MAXCLIENT;i++){
        if(serverpool[i].pid != -1 && serverpool[i].state==STATE_IDEL){
            kill(serverpool[i].pid, SIGTERM); //?
            serverpool[i].pid = -1;
            idle_count --;
            break;
        }
    }
}

static void usr2_handler(int s){
	//printf("1111\n");
	return;
}

int main(){
    int val = 1;
    int i;
    struct sockaddr_in laddr;

    // 子进程结束自行消亡，不去通知父进程
    struct sigaction sa, osa; 
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_NOCLDWAIT;
    sa.sa_handler = SIG_IGN;
    //SIGCHLD用于通知父进程：子进程已经终止或停止执行，关闭
    sigaction(SIGCHLD, &sa, &osa); // 定义SIGCHLD新行为，就行为放到osa

    // 先屏蔽NOTIFY，在初始化完成之前不要打扰，在SUSPEND函数中会自动将OSET屏蔽将NOTIFY激活
    sigset_t set, oset;
    sigemptyset(&set);
    sigaddset(&set, SIG_NOTIFY);
    sigprocmask(SIG_BLOCK, &set, &oset);

    sa.sa_handler = usr2_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIG_NOTIFY, &sa, &osa);

    serverpool = mmap(NULL, sizeof(struct service_st) * MAXCLIENT, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    for(i = 0;i<MAXCLIENT;i++){
        serverpool[i].pid = -1;
    }
    if(serverpool == MAP_FAILED){
        perror("mmap()");
        exit(1);
    }
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd<0){
        perror("socket()");
        exit(1);
    }
    
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)))
    {
        perror("setsockopt()");
        exit(1);
    }
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVERPORT));
    inet_pton(AF_INET, "0.0.0.0",&laddr.sin_addr);
    if(bind(sd, (void *)&laddr, sizeof(laddr))<0){
	perror("bind()");
	exit(1);
    }
    if(listen(sd, 100)<0){
        perror("listen()");
        exit(1);
    }

    for(int i = 0; i<MINSPARESERVER;i++){
        add_one_server();
    }
    while(1){
    // 信号驱动，当子进程状态变化，通知父进程
        sigsuspend(&oset);
        //printf("!\n");
        scan_pool();
        if(idle_count > MAXSPARESERVER){
            for(i = 0; i<idle_count-MAXSPARESERVER;i++)
                del_one_server();
        }
        else if(idle_count < MAXSPARESERVER){
            for(i = 0; i<MAXSPARESERVER-idle_count;i++)
                add_one_server();
        }
        //输出池状态
        for( i =0;i<MAXCLIENT; i++)
        {
            if(serverpool[i].pid == -1){
                putchar(' ');
            }
            else if(serverpool[i].state == STATE_IDEL)
                    putchar('.');
                else
                    putchar('x');
            fflush(NULL);
        }
        putchar('\n');
        //ctrl_pool();

    }

    sigprocmask(SIG_SETMASK, &oset, NULL);
    exit(0);
}