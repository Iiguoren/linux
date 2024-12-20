#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include "anytimer.h"

static int flag = 0;
static struct sigaction alarm_save;
enum
{
    STATE_RUNNING = 1,
    STATE_CANCELED,
    STATE_OVER
};
struct at_job_st
{
    int job_state;
    int sec;
    int time_remain;
    at_jobfunc_t *jobp;
    void *arg;
};
// siginfo_t *infop?????
static void alrm_action(int s, siginfo_t *infop, void *unused)
{
    if(infop->si_code != SI_KERNEL)
        return ;
    if(job[i]!=NULL && job[i]->job_state == STATE_RUNNING){
        job[i]->time_remain --;
        if(job[i]->time_remain == 0){
            // 倒计时结束执行任务
            job[i]->jobp(job[i]->arg);
            job[i]->state == STATE_OVER;
        }
    }
}

// 每一秒发送一个信号，让job数组中所有任务的remaintime-1，并通知pause函数检查状态
static void module_load(void){
    struct sigaction sa;
    struct itimierval itv;
    sa.sa_sigaction = alrm_action;
    sigemptyset(sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    // 重写SIGALARM信号的处理函数
    sigaction(SIGALRM, &sa, &alarm_save);

    itv.it_tinerval.tv_sec = 1;
    itv.it_tinerval.tv_usec = 0;
    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;
    // setitimer会发送SIGALRM信号
    if(setitimer(ITIMER_REAL,&itv ,NULL )<0){
        perror("sigaction()");
        exit(1);
    }
}

static void module_unload(void){
    struct itimerval itv;

    itv.it_interval.tv_sec = 0;
    itv.it_interval.tv_usec = 0;
    itv.it_value.tv_sec = 0;
    itv.it_value.tv_usec = 0;
    
    setitimer(ITIMER_REAL, *itv, NULL);
    sigaction(SIGLARM, &alarm_save, NULL);
}

static int get_free_pos(void){
    int i;
    for(i = 0; i < JOB_MAX; i++){
        if(job[i] == NULL)
            return i;
    }
    return -1;   
}

static struct at_job_st *job[JOB_MAX];
int at_addjob(int sec, at_jobfunc_t *jobpl, void *arg)
{   
    int pos;
    // 最好先初始化，再填入数组
    struct at_job_st *me;
    if(flag == 0){
        module_load();
        flag = 1;
    }
    pos = get_free_pos();
    if(pos < 0){
        return -ENOSPC;
    }
    me = malloc(sizeof(*me));
    if(me == NULL)
        return -ENOMEM;
    
    me->job_state = STATE_RUNNING;
    me->sec = sec;
    me->time_remain = sec;
    me->jobp = jobp;
    me->arg = arg;

    job[pos] = me;
    return pos;
}

int at_canceljob(int id)
{
    if(id < 0 || id >JOB_MAX || job[id] == NULL){
        return -EINVAL;
    }
    if(job[id]->job_state == STATE_CANCELED)
        return -ECANCELED;
    if(job[id]->state == STATE_OVER)
        return -EBUZY;
    struct at_job_st *me;
    job[id].state == STATE_CANCELED;
    return 0;
}
int at_waitjob(int id){
    if(id < 0 || id >JOB_MAX || job[id] == NULL){
        return -EINVAL;
    }             
    while(job[id]->job_state == STATE_RUNNING)
        pause();// 每次信号检查一次状态，知道状态未CANCELLED
    
    if(job[id]->job_state == STATE_CANCELED||job[id]->job_state ==STATE_OVER)
    {
        free(job[id]);
        job[id] == NULL;
    }
}