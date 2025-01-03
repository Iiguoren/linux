#ifndef PROTO_H__
#define PROTO_H__
#include "site_type.h"
// 默认组IP
#define DEFAULT_MGROUP "224.2.2.2"
// 默认端口
#define DEFAULT_RCVPORT "1989"
// 定义频道数量
#define CHNNR 100
// 定义节目单为channel 0
#define LISTCHNID 0
// 最小频道1
#define MINCHNID 1
// 最大频道
#define MAXCHNID (MINCHNID+CHNNR-1)
#define MSG_CHANNEL_MAX (65536-20-8) // 包长-IP包报头-UDP报头
#define MAX_DATA (MSG_CHANNEL_MAX-sizeof(chnid_t))
// channel 0 节目单最大包长
#define MSG_LIST_MAX  (65536-20-8)
// channel 0 节目单最大数据长度
#define MAX_ENTRY (MSG_LIST_MAX-sizeof(chnid_t))
struct msg_channel_st{
    chnid_t chnid;
    uint8_t data[1]; //可变数组
}__attribute__((packed)); // 高速编译器不要对齐
// 描述信息
struct msg_listentry_st{
    chnid_t chnid;
    uint16_t len;
    uint8_t desc[1];
}__attribute__((packed));

// 节目单结构体
struct msg_list_st
{
    chnid_t chnid; // must be listchnnid
    struct msg_listentry_st entry[1];
}__attribute__((packed));
#endif
