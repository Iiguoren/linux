#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/shm.h>
// 没有编译过
#define SHM_SIZE 4*1024
int main(){
    int shamd;
    void *ptr;
    // 创建共享内存
    shamd = shamget(IPC_PRIVATE,SHM_SIZE,  0666); // IPC_PRIVATE不用IPC_CREATE就可以自己创建
    if(shamd < 0){
        perror("shamget()");
        exit(1);
    }

    pid_t pid;
    pid  = fork();
    if(pid<0){
        perror("fork()");
        exit(1);
    }
    if(pid==0){
       ptr = shmat(shamd, NULL, 0);
        if(ptr== -1){
            perror("shmat()");
            exit(1);
        }
        strcpy(ptr,"hello");
        shmdt(ptr);
        exit(0);
    }
    else{
        wait(NULL);
        ptr = shmat(shamd, NULL, 0);
        if(ptr== -1){
            perror("shmat()");
            exit(1);
        }
        puts(ptr);
        shmdt(ptr);
        shmctl(shamd, IPC_RMID, NULL);
        exit(0);
    }
}