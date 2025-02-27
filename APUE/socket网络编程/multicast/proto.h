#ifndef PROTO_H__
#define PROTO_H__

#include <stdint.h>
#define RCVPORT "2333"
#define NAMESIZE 11
#define MULTGROUT "233.0.0.1"
struct msg_st{
    uint32_t math;
    uint32_t chinese;
    char name[NAMESIZE];
}__attribute__((packed));//不对齐

#endif
