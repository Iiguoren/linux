#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "mysem.h"
#define LEFT 30000000
#define RIGHT 30000200
#define THREAD_NUM 201
#define N 4
struct str_int{
    int num;
};
/*static struct mysem *sem = mysem_init(N);*/
static struct mysem *sem=NULL;
/*错误， C 语言要求静态变量的初始化值在编译时必须是常量，而 mysem_init(N) 是一个函数调用，它的值是在程序运行时计算的，不符合静态变量初始化时要求的常量表达式。*/
static void *thread_func(void *p){
        int i = ((struct str_int *)p)->num;// p是void *先进行格式转换
        int j;
        for(j = 2; j<i/2; j++){
            if(i % j == 0)
                break;
        }
        if(j == i/2){
            printf("%d is a primer.\n", i);}
        mysem_add(sem, 1);
        pthread_exit(p);
}
int main(){
    pthread_t tid[THREAD_NUM];
    int i;
    sem = mysem_init(N);
    void *ptr=NULL;
    struct str_int *p;
    printf("begin\n");
    for(i = LEFT; i<RIGHT; i++){
        mysem_sub(sem, 1);
        p = malloc(sizeof(*p));
        p->num =i;
        int err = pthread_create(tid+(i-LEFT), NULL, thread_func, p);
        if(err < 0 ){
            printf("pthread_create():%s",strerror(err));
            exit(1);
        }
    }
    printf("main wait\n");
    for(i = LEFT; i<RIGHT; i++){
        pthread_join(tid[i - LEFT], ptr);
        //printf("waiting");
        free(ptr);
    }
    mysem_destory(sem);
    printf("main over.\n");
    exit(0);
}