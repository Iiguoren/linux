#include "../../include/proto.h"

#ifndef MEDIALIB_H
#define MEDIALIB_H

struct mlib_listentry_st //自用结构体,保存chID和描述
{
    /* data */
    chnid_t chnid;
    char *desc;
};
// 参数：数组，数组长度
int mlib_getchnlist(struct mlib_listentry_st **, int *);
int mlib_freechnlist(struct mlib_listentry_st *);
ssize_t mlib_readchn(chnid_t, void * , size_t );
#endif