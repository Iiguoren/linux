#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/sem.h>
#define THRNUM 20
#define FILEPATH "/etc/out"
#define BUFSIZE 1024
// 没有编译过
static P(){
    struct sembuf op;
    op.sem_num = 0; // 先设置数组，再使用op结构体操作
    op.sem_op = -1;//取一个资源
    op.sem_flg = 0;//特殊要求

    while(semop(semid,&op,1) < 0){ // 第二个参数是结构体数组
        if (errno == EINTR||errno == EAGAIN){
            continue;
        }else{
            perror("semop()");
            exit(1);
        }
    }
}
//还资源量
static void V(){
    struct sembuf op;

    op.sem_num = 0;
    op.sem_op = 1;//还一个资源
    op.sem_flg = 0;//特殊要求

    while(semop(semid,&op,1) < 0){
        if (errno == EINTR||errno == EAGAIN){ // 防止中断和堵塞
            continue;
        }else{
            perror("semop()");
            exit(1);
        }
    }

}
void handler(){
    int fd;
    char buf[1024];
    fd = open(FILEPATH, "r");
    P();
    // 使用标准IO因为系统IO中write不能使用格式符
    fgets(buf,BUFSIZE,fp);
    // 文件指针重新移动到头部
    fseek(fp,0,SEEK_SET);
    sleep(1);
    fprintf(fp,"%d\n",atoi(buf)+1);
    fflush(fp);

    V();

    fclose(fp);

}
static int semid;  // 信号量数组的id
int main(){
    pid_t pid;
    struct sembuf sops;
    semid = semget(IPC_PRIVATE,1,0666); // 创建只供父子进程通信的匿名信号量数组，数量为1，权限为0666
    if (semid < 0){
        perror("semget()");
        exit(1);
    }  
    //初始化
    if (semctl(semid,0,SETVAL,1)){//相当于互斥量
        perror("semctl()");
        exit(1);
    } 
    for(int i = 0; i<THRNUM; i++){
        pid = fork();
        if(pid<0){
            perror("fork()");
        exit(1);
        }
        if(pid == 0){
            handler();
            exit(0);
        }
    }
    
    if(pid > 0){
        for(int i = 0; i<THRNUM; i++){
         wait(NULL);
        }
    }
       

}