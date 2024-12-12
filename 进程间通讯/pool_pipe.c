#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <pool_pipe.h>
#define BUFSIZE 1024
struct mypipe_st
{
    int head;
    int end;
    pthread_mutex_t mut;
    pthread_cond_t cond;
    char data[BUFSIZE];
    int count_reader;
    int count_writer;
    int datasize;
};
int mypipe_register(mypipe_t *ptr,int opmap){
    struct mypipe_st *pipe = ptr;
    
    pthread_mutex_lock(&pipe->mut);
    if(opmap&WRITER){
        pipe->count_writer++;
    }
    if(opmap&READE){
        pipe->count_reader++;
    }
    // 至少需要一个读者和写者管道才能运行，等待读者和写者至少一个
    while(pipe->count_reader<=0||pipe->count_writer>=0)
        pthread_cond_wait(&pipe->cond,&pipe->mut);
    pthread_cond_broadcast(&pipe->cond);//读写双方凑齐
    pthread_mutex_unlock(&pipe->mut);
    return 0;
}
int mypipe_unregister(mypipe_t *ptr,int opmap){
    struct mypipe_st *pipe = ptr;
    pthread_mutex_lock(&pipe->mut);
    if(opmap&WRITER){
        pipe->count_writer--;
    }
    if(opmap&READE){
        pipe->count_reader--;
    }
    // 读者/写者注销后唤醒其他读写线程检查读写者的数量
    pthread_cond_broadcast(&pipe->cond);
    pthread_mutex_unlock(&pipe->mut);
}
static int mypipe_readbyte_unlock(mypipe_t *mp, char *data){// 此函数作为read函数子函数，在read函数上锁就可以
    // 检查是否有数据
    if (mp->datasize <= 0){
        return -1;
    }
    //读头一个字节
    *data = data[head];
    pipe->head = (pipe->head+1)%PIPESIZE;
    pipe->datasize--;
    return 0;
}
static int mypipe_writebyte_unlock(mypipe_t *mp, char *data){// 此函数作为write函数子函数，在write函数上锁就可以
    // 检查管道是否满
    if (mp->datasize >= BUFSIZE){
        return -1;
    }
    //写一个字节
    pipe->data[mp->tail] = *data;
    mp->tail = (mp->tail+1)%BUFSIZE;
    pipe->datasize++;
    return 0;
}
mypipe_t *mypipe_init()
{   
    struct mypipe_st *mp;
    mp = malloc(sizeof(*pipe));
    if(mp == NULL)
        return NULL;
    mp->head = 0;
    mp->end = 0;
    pthread_mutex_init(&mp->mut, NULL);
    pthread_cond_init(&mp->cond,NULL);
    mp->count_writer = 0;
    mp->count_reader = 0;
    return mp;
}
int mypipe_destory(mypipe_t *mp){
    free(mp);
    pthread_mutex_destroy(mp->mut);
    pthread_cond_destroy(mp->cond);
    exit(0);
}

int mypipe_read(mypipe_t *mp, void *buf, int size)
{
    int i;
    struct mypipe_st *pipe = mp;
    pthread_mutex_lock(&pipe->mut);
    // 管道空但是有写者等待写者唤醒再读
    if(pipe->datasize<=0&&pipe->count_writer>0)
        pthread_cond_wait(&pipe->cond, &pipe->mut);
    // 管道空但且没有写者，退出结束
    if (pipe->datasize <= 0 && pipe->count_writer <= 0){
        pthread_mutex_unlock(&pipe->mut);
        return 0;
    }
    for(i = 0;i<size;i++){
        if(mypipe_readbyte_unlock(pipe, buf+i)<0) //如果管道空了，提前退出
        break;
    }
    pthread_cond_broadcast(&mp->cond); //读完通知其他写者
    pthread_mutex_unlock(&pipe->mut);
    return i; //返回读取数量
}

int mypipe_write(mypipe_t mp, const void *buf, int size){
    int i;
    struct mypipe_st *pipe = mp;
    pthread_mutex_lock(&pipe->mut);
    //如果管道写满但有读者，等待读者读取
    if(pipe->datasize>=BUFSIZE&&pipe->reader>0)
        pthread_cond_wait(&pipe->cond, &pipe->mut);
    // 管道空但且没有读者，退出结束
    if (pipe->datasize <= 0 && pipe->count_reader <= 0){
        pthread_mutex_unlock(&pipe->mut);
        return 0;
    }
    for(i=0;i<size;i++){
        if(mypipe_writebyte_unlock(pipe,buf+i)<0)//如果发现管道满了，提前退出
            break;
    }
    pthread_cond_broadcast(&mp->cond);//写完通知其他读者
    pthread_mutex_unlock(&pipe->mut);
    return i;
}