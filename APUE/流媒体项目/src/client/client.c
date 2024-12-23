#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
/*
-M --mgroup指定多播组
-P --port指定接受端口
-p --player指定播放器
-H --help显示帮助
*/
struct client_conf_st client_conf;
client_conf.rcvport = DEFAULT_RCVPORT;
client_conf.mgroup = DEFAYKT_MGROUP; //多播地址
.player_cmd = DEFAULT_PLAYERCMD; //命令行传输
static void printhelp(void){
    printf("-P --port 指定接收端口\n-M --mgroup 指定多播组\n
    -p --player 指定播放器命令行\n -H --help 显示帮助\n");
}
int main(int argc, char **argv){
/* 初始化 
级别：默认值<配置文件<环境变量<命令行参数
*/
    int index = 0;
    int c;
    struct option argarr[] = {{"port",1,NULL.'P'},{"mgroup",1,NULL,'M'},\
                                {"player",1,NULL,'p'},{"help",0,NULL,'H'},{NULL,0,NULL,0}};
    while(1){
        int c = getopt_long(argc, argv, "P:M:p:H", argarr,&index);
        if(c < 0 )
            break;
        swith(c){
            case'P':
                client_conf.rcvport = optarg;
                break;
            case'M':
                client_conf.mgroup = oprarg;
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
    socket();
    pipe();

    fork();
    // 如果子进程，负责调解码器
    // 如果是父进程，从网络收包，发送给子进程

    exit(0);
}