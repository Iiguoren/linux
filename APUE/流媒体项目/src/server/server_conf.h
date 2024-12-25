#ifndef SERVER_CONF_H
#define SERVER_CONF_H

#define DEFAULT_MEDIADIR "/var/media/"
#define DEFAULT_IF "eth0"
enum{
    RUN_DAEMON = 1,
    RUN_FOREGROUND 
};

struct server_conf_st{
    char *rcvport; //接受端口
    char *mgroup; // 多播组地址
    char *media_dir; // 媒体库
    char runmode; // 前后台运行位
    char *ifname;
}
extern server_conf_st server_conf;
#endif