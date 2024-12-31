#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/log.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "../..include/proto.h"

#include "thr_list.h"
#include "server_conf.h"
static pthread_t tid_list; // 线程列表
static int num_list_entry; // 节目单中频道数量
static struct mlib_listentry_st *list_entry; // 频道列表
/* 创建频道线程*/

static void *thr_list(void *p){
    int totalsize; // msg_list所需要的空间大小
    struct msg_list_st *entrylistptr; // 节目单列表结构体
    struct msg_listentry_st *entryptr;  // 节目单信息结构体
    int ret;
    int size;

    totalsize = sizeof(chnid_t);
    for(int i; i<num_list_entry; i++){
        // 节目单结构体大小： chnid_t + list_entry结构体大小(chnid + desc)
        totalsize += sizeof(struct msg_listentry_st) + strlen(list_entry[i].desc);
        }
        entrylistptr = malloc(totalsize); // 创建节目单结构体
        if (entrylistptr == NULL) {
        syslog(LOG_ERR, "malloc():%s", strerror(errno));
        exit(1);
        }
        entrylistptr->chnid = LISTCHNID; // 这是列表频道
        syslog(LOG_DEBUG, "nr_list_entn:%d\n", num_list_entry);
        while (1) {
            syslog(LOG_INFO, "thr_list sndaddr :%d\n", sndaddr.sin_addr.s_addr);
            ret = sendto(serversd, entrylistptr, totalsize, 0, (void *)&sndaddr, sizeof(sndaddr));
            syslog(LOG_DEBUG, "sent content len:%d\n", entrylistptr->entry->len);
            if (ret < 0) {
            syslog(LOG_WARNING, "sendto(serversd, enlistp...:%s", strerror(errno));
            } else {
            syslog(LOG_DEBUG, "sendto(serversd, enlistp....):success");
            }
            sleep(1);    

        }
    }

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

int thr_list_destroy(void){
    pthread_cancel(tid_list);
    pthread_join(tid_list, NULL);
    return 0;
}
