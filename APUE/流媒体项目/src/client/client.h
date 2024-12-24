#ifndef CLIENT_H
#define CLIENT_H
/* 将标准输出重定向到 /dev/null，避免播放时在控制台显示冗余信息。*/
// - 播放标准输入
#define DEFAULT_PLAYERCMD "mpg123 - > /dev/null"


struct  client_conf_st
{
    char *rcvport;
    char *mgroup; //多播地址
    char *player_cmd; //命令行传输
};
extern struct client_conf_st client_conf; //给其他.c使用


#endif