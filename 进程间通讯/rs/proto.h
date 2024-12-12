#ifndef PROTO_H__
#define PROTO_H__
#define KEYPATH "/etc/services"
#define KEYPROJ 'g'
#define NAMESIZE 32
struct mag_st
{
    /* data */
    long mtype; //当前消息类型
    char name[NAMESIZE];
    int math;
    int chinese;
};
#endif