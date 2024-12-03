#include <errno.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>

#include "mytbf.h"
static pthread_t ptid;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int inited = 0;
struct  mytbf_st{
    int cps;
    int pos;
    int burst;
    int token;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};

static void *handler(void *p){
    int i;
    struct timespec ts;
    ts.tv_sec = 1;
    ts.tv_nsec = 0;
    while(1){
        pthread_mutex_lock(&mutex);
        for(i = 0;i<TBFSIZE;i++){
            if(job[i]!=NULL){
                pthread_mutex_lock(&job[i]->mut);
                job[i]->token += job[i]->cps;
                if (job[i]->token > job[i]->burst) {
                job[i]->token = job[i]->burst;
                }
                pthread_mutex_unlock(&job[i]->mut);
                pthread_cond_broadcast(&job[i]->cond);
            }
        }
        
        pthread_mutex_unlock(&mutex);
        nanosleep(&ts,NULL);
    }
};

static int get_free_pos_unlocked(){
  for (int i = 0;i < TBFSIZE;i++){
    if (job[i] == NULL)
      return  i;
  }
  return -1;
}
//释放令牌桶
static void module_unload(){
    int i;
    
    for(i = 0;i<TBFSIZE;i++){
        free(job[i]);
    }
}

void module_load(){
    int err = pthread_create(&ptid, NULL, handler, NULL); // 创建一个线程用来执行发放token操作
    if(err<0){
    fprintf(stderr, "pthread_create():%s",strerror(err));
    exit(1);
    }
    atexit(module_unload);//钩子函数
}


mytbf_t *mytbf_init(int cps,int burst){
    struct mytbf_st *tbf;
    if(!inited){
        module_load();
        inited = 1;
    }
    int pos;
    
    tbf = malloc(sizeof(*tbf));
    tbf->token = 0;
    tbf->burst = burst;
    tbf->cps = cps;
    pthread_mutex_init(&tbf->mut,NULL);
    pthread_cond_init(&tbf->cond,NULL);
    pthread_mutex_lock(&mutex);
    pos = get_free_pos_unlocked();
    if (pos == -1){
        free(tbf);
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    tbf->pos = pos;
    job[pos] = tbf;
    pthread_mutex_unlock(&mutex);
    return tbf;
}

int mytbf_fetchtoken(mytbf_t *ptr, int num)
{
    struct mytbf_st *tbf = ptr;
    if(num <= 0){
        return -EINVAL;
    }
    pthread_mutex_lock(&mutex);
    while(tbf->token<=0)
        pthread_cond_wait(&tbf->cond,&tbf->mut);//等通知 抢锁
    int n =tbf->token<num?tbf->token:num;
    tbf->token -= n;
    pthread_mutex_unlock(&mutex);
    return n;
}

int mytbf_returntoken (mytbf_t*ptr, int num){
    struct mytbf_st *tbf = ptr;
    if(num <= 0){
        return -EINVAL;
    }

    tbf->token += num;
    tbf->token<tbf->burst?tbf->token:tbf->burst;
    return tbf->token;
}

int mytbf_destory(mytbf_t *ptr){
    struct mytbf_st *tbf = ptr;
    pthread_mutex_lock(&mutex);
    job[tbf->pos] = NULL;
    free(tbf);
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&tbf->mut);
    pthread_cond_destroy(&tbf->cond);
    return 0;
}