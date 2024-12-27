#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static pthread_t tid_list; // 线程列表
static int num_list_entry;
static struct mlib_listentry_st *list_entry; // 频道列表
/* 创建频道线程*/
int thr_list_create(struct mlib_listentry_st *listptr, int num_ent){
    int err;
    list_entry = listptr;
    num_list_entry = num_ent;
    syslog(LOG_DEBUG, "list content:chnid:%d, desc:%s\n",listptr->chnid, listptr->desc);
    err = pthread_create(&tid_list, NULL, thr_list, NULL);
    if (err) {
    syslog(LOG_ERR, "pthread_create():%s", strerror(errno));
    return -1;
    }
    return 0;
}
