#ifndef CLIENT_H
#define CLIENT_H
#define DEFAULT_PLAYERCMD "mpg123 > /dev/null"
struct  client_conf_st
{
    char *rcvport;
    char *mgroup; //多播地址
    char *player_cmd; //命令行传输
};
extern struct client_conf_st client_conf; //给其他.c使用


#endif