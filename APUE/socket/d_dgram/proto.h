#ifndef PROTO_H__
#define PROTO_H__
// 未编译
#include <stdint.h>
#define RCVPORT "2333"
#define NAMEMAX 512-8-8

struct msg_st{
    uint32_t math;
    uint32_t chinese;
    char name[1];
}__attribute__((packed));//不对齐

#endif
