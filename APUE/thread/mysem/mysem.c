#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

struct mysem
{
    int val;
    pthread_mutex_t mut;
    pthread_cond_t cond;
};

void *mysem_init(int val){
    struct mysem *sem;
    sem = malloc(sizeof(*sem));
    if (sem == NULL){
        return NULL;
    }
    sem->val = val;
    pthread_mutex_init(&sem->mut, NULL);  // 使用 pthread_mutex_init 初始化互斥锁
    pthread_cond_init(&sem->cond, NULL); 
    return sem;
}

int mysem_add(void *ptr, int num){
    struct mysem *sem = ptr;
    if(ptr == NULL)
    {
        return -1;
    }
    pthread_mutex_lock(&sem->mut);
    sem->val += num;
    pthread_cond_broadcast(&sem->cond); //会自动Unlock吗:不会
    pthread_mutex_unlock(&sem->mut);
    return num;
}

int mysem_sub(void *ptr,int num){
    struct mysem *sem = ptr;
    if(ptr == NULL)
    {
        return -1;
    }
    pthread_mutex_lock(&sem->mut);
    while(sem->val<num)
        pthread_cond_wait(sem->cond, sem->mut); //会自动Unlock吗:会
    sem->val -= num;
    pthread_mutex_unlock(&sem->mut);
    return num;
}

int mysem_destory(void *ptr){
    
    struct mysem *sem = ptr;
    pthread_mutex_lock(&sem->mut);
    pthread_mutex_destroy(&sem->mut);   // 销毁互斥锁
    pthread_cond_destroy(&sem->cond);   // 销毁条件变量
    free(sem);   // 释放内存
    pthread_mutex_unlock(&sem->mut);    // 解锁
    return 0; //一般不会出错
}