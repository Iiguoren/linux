#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include "relay.h"
#define BUFSIZE 1024
static pthread_once_t job_once = PTHREAD_ONCE_INIT;
static pthread_mutex_t job_mutex = PTHREAD_MUTEX_INITIALIZER;

static void fsm_driver(struct fsm_st* fsm){
    int ret;
    //printf("%d\n", fsm->state);
    switch(fsm->state){
        case STATE_R:
            fsm->len = read(fsm->sfd,fsm->buf,BUFSIZE);
            printf("%d:%s\n",fsm->name,fsm->buf);
            printf("%d:read finished\n",fsm->name);
            if (fsm->len == 0){
                fsm->state = STATE_T;//正常结束
            }else if (fsm->len < 0){
                if (errno == EAGAIN)
                  fsm->state = STATE_R;//假错 保留状态
                else{
                    printf("enter readerr\n");
                    fsm->state = STATE_Ex;//真正出错
                    fsm->errstr = "读取失败";
                }

            }else{
                fsm->pos = 0;
                printf("%d:成功读入 转换状态\n",fsm->name);
                fsm->state = STATE_W;//成功读入 转换状态
            }

            break;
        case STATE_W:
            printf("%d:enter write\n",fsm->name);
            ret = write(fsm->dfd,fsm->buf+fsm->pos,fsm->len);
            printf("%d:dfd:%s;%d\n",fsm->name,fsm->buf+fsm->pos,fsm->len);
            if (ret < 0){
                if (errno == EAGAIN){
                    fsm->state = STATE_W;
                }else{
                    fsm->errstr = "写入失败";
                    fsm->state = STATE_Ex;
                }
            }else{
                //坚持写够
                fsm->pos += ret;
                fsm->len -= ret;
                if (fsm->len == 0){
                    fsm->state = STATE_R;
                    printf("%d,成功write 转换状态\n",fsm->name);
                }else{
                    fsm->state = STATE_W;
                    printf("%d:continue write \n",fsm->name);
                }
            }

            break;
        case STATE_Ex:
            perror(fsm->errstr);
            fsm->state = STATE_T;
            break;
        case STATE_T:
            //printf("job finished\n");
            /*do sth*/
            break;
        default:
            printf("other condition\n");
            abort();
            break;
    }

}

static int get_free_pos_unlocked(){
    for (int i = 0;i < JOBMAX;i++){
        if (jobs[i] == NULL)
          return  i;
    }
    printf("no pos\n");
    return -1;
}

static void *handler(void *p){
    while(1){
        pthread_mutex_lock(&job_mutex);
        for(int i=0; i<JOBMAX;i++){
            if(jobs[i]!=NULL){
                if(jobs[i]->state==STATE_RUNNING){
                    fsm_driver(&jobs[i]->fsm12);
                    fsm_driver(&jobs[i]->fsm21);
                }
                else if(jobs[i]->fsm12.state == STATE_T&&
                                jobs[i]->fsm21.state == STATE_T){
                        jobs[i]->state = STATE_OVER;//两个状态机都已经停止 任务结束
                        printf("over\n");
                }
                else{
                printf("other occor\n");
                }
            }
        }
        //printf("no new job, waiting..\n");
        pthread_mutex_unlock(&job_mutex);
    }
    pthread_exit(NULL);
}
static void module_load(){
    pthread_t ptid;
    int err = pthread_create(&ptid, NULL, handler, NULL);
    printf("module load sucessfully\n");
    if (err){
        fprintf(stderr,"pthread_create() %s\n",strerror(err));
        exit(1);
    }
}
int add_job(int fd1, int fd2){
    //当多个线程调用 pthread_once 时：第一次调用：执行指定的初始化函数，确保其成功完成。后续调用：直接返回，不再重复执行初始化函数。
    pthread_once(&job_once,module_load); //确保初始化操作仅被执行一次

    struct job_st *job;
    job = malloc(sizeof(*job)); // 动态分配内存
    int pos;
    if (job == NULL){
        return -ENOMEM;
    }

    job->fd1 = fd1;
    job->fd2 = fd2;
    job->state = STATE_RUNNING;

    job->old_fd1 = fcntl(job->fd1,F_GETFL);
    fcntl(fd1,F_SETFL,job->old_fd1|O_NONBLOCK);

    job->old_fd2 = fcntl(job->fd2,F_GETFL);
    fcntl(fd2,F_SETFL,job->old_fd2|O_NONBLOCK);

    //job->state = STATE_RUNNING; //initialize prestate..
    job->fsm12.sfd = job->fd1;
    job->fsm12.dfd = job->fd2;
    job->fsm12.state = STATE_R;
    job->fsm12.name = 1;
    
    job->fsm21.sfd = job->fd2;
    job->fsm21.dfd = job->fd1;
    job->fsm21.state = STATE_R;
    job->fsm21.name = 2;
    pthread_mutex_lock(&job_mutex); // 防止在更改jobs有线程访问
    pos = get_free_pos_unlocked();
    
    if (pos < 0){
        pthread_mutex_unlock(&job_mutex);
        //恢复原来的文件描述符状态
        fcntl(job->fd1,F_SETFL,job->old_fd1);
        fcntl(job->fd2,F_SETFL,job->old_fd2);
        free(job);
        return -ENOSPC;
    }
    jobs[pos] = job;
    pthread_mutex_unlock(&job_mutex);
    return pos;
}