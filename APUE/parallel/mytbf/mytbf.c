#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "mytbf.h"
static sighandler_t  alarm_handler_save;
static int inited = 0;
struct  mytbf_st{
    int cps;
    int pos;
    int burst;
    int token;
}

static void alrm_handler(int pos){
    int i;
    alarm(1);
    for(i = 0;i<TBFSIZE;i++){
        if(job[pos]==NULL){
            job[i]->token += job[i]->cps;
            if (job[i]->token > job[i]->burst) {
            job[i]->token = job[i]->burst;
            }
        }
    }
}

int get_free_pos(){
    int i;
    for(i = 0; i<TBFSIZE; i++){
        struct mytbf_st *tbf = job[i];
        if(tbf != NULL){
            return i;
        }
    }
    return -1;
}
//释放令牌桶
static void module_unload(){
    int i;
    signal(SIGALRM,alarm_handler_save); //恢复信号处理函数
    alarm(0);
    for(i = 0;i<TBFSIZE;i++){
        free(job[i]);
    }
}

void module_load(){
    alarm_handler_save = signal(SIGALRM, alrm_handler);
    alarm(1);
    atexit(module_unload);
}


mytbf_t *mytbf_init(int cps,int burst){
    struct mytbf_st *tbf;
    if(!inited){
        module_load();
        inited = 1;
    }
    int pos;
    pos = get_free_pos();
    if (pos == -1){
        return NULL;
    }
    tbf = malloc(sizeof(*tbf));
    tbf->token = 0;
    tbf->burst = burst;
    tbf->cps = cps;
    tbf->pos = pos;
    job[pos] = tbf;
    return tbf;
}

int mytbf_fetchtoken(mytbf_t *ptr, int num)
{
    struct mytbf_st *tbf = ptr;
    if(num <= 0){
        return -EINVAL;
    }

    if (job[i]->token > job[i]->burst) {
            job[i]->token = job[i]->burst;
            }
    tbf->token -= n;

    return n;
}

int mytbf_retrurntoken(mytbf_t*ptr, int num){
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
    job[tbf->pos] = NULL;
    free(tbf);
    return 0;
}