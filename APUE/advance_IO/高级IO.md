**非阻塞IO --- 阻塞IO**
在阻塞 I/O模式下，当一个进程或线程发起 I/O 操作（如读取数据、写入数据）时，它会阻塞并等待 I/O 操作完成。如果操作没有完成，线程会一直处于挂起状态，直到 I/O 完成并返回数据或确认操作成功。

非阻塞 I/O模式下，发起 I/O 操作时，线程或进程不会被阻塞，而是立即返回一个错误EAGAIN。即使数据不可用，线程也不会停止执行，而是继续执行后续的代码。线程可以通过反复检查 I/O 操作的状态（例如使用 poll() 或 select()），或者通过回调机制等待 I/O 操作完成。
对非阻塞套接字进行写操作时，如果系统的发送缓冲区已满，则会返回 EAGAIN。
如果你连续做read操作而没有数据可读,则会返回 EAGAIN。
有限状态机:复杂流程问题
    简单问题：如果一个程序的自然流程是结构化的，是简单流程
    复杂流程：自然流程是非结构化的，是复杂流程（顺序分支判断无法直接解决）


1. 非阻塞IO
2. IO多路转接
3. 其他读写函数
4. 存储映射IO
5. 文件锁

![Alt text](状态机.png)
共有四个状态：读状态:STATE_R;写：STATE_W；错误：STATE_Ex；完成：STATE_T；
读状态： 读取sfd,如果成功进入写状态，不成功进入EAGAIN重复执行，其他进入Ex状态
写状态：将缓冲区写入dfd,如果成功检查缓冲区是否全部写入，不成功进入EAGAIN重复执行，其他进入Ex状态
Ex状态：报错，转入T状态
T状态：退出
```c
// relay.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"
#define BUFSIZE 1024
enum{
    STATE_R=1, //状态读，初始状态
    STATE_W, //状态写
    STATE_Ex, //状态错误
    STATE_T //状态完成
};
struct fsm_st{
    int state;
    int sfd;
    int dfd;
    char buf[BUFSIZE];
    int len;
    int pos;
    char *errstr;
};
static void fsm_driver(struct fsm_st *fsm){
    int ret;
    switch(fsm->state){
        case STATE_R:
            fsm->len = read(fsm->sfd,fsm->buf, BUFSIZE);
            if(fsm->len == 0)
                fsm->state = STATE_T;
            else if(fsm->len < 0){
                if(errno == EAGAIN)
                    fsm->state = STATE_R;
                else{
                    fsm->errstr = "read error\n";
                    fsm->state = STATE_Ex;
                    }
            }
            else{
                fsm->pos = 0;
                fsm->state = STATE_W;}
            break;
        case STATE_W:
            ret = write(fsm->dfd, fsm->buf + fsm->pos, fsm->len);
            if(ret < 0){
                if(errno== EAGAIN)
                    fsm->state = STATE_W;
                else{
                    fsm->errstr = "write error\n";
                    fsm->state = STATE_Ex;
                    }
            }
            else{
                fsm->pos += ret;
                fsm->len -= ret;
                if(fsm->len == 0)
                    fsm->state = STATE_R;
                else
                {
                    fsm->state = STATE_W;
                    }
            }
            break;
        case STATE_Ex:
            perror(fsm->errstr);
            fsm->state = STATE_T;
            /*do something*/
            break;
        case STATE_T:
            /*do something*/
            break;
        default:
            abort();
            /*do something*/
            break;
    }

}
static void relay(int fd1,int fd2){
    int fd1_save;
    int fd2_save;
    struct fsm_st fsm12, fsm21;//读左写右和读右写左
    fd1_save = fcntl(fd1, F_GETFL); // 保存状态
    fcntl(fd1, F_SETFL, fd1_save|O_NONBLOCK); // 将状态覆盖fd1

    fd2_save = fcntl(fd2, F_GETFL); // 保存状态
    fcntl(fd2, F_SETFL, fd2_save|O_NONBLOCK);

    // 为状态机赋值
    fsm12.state = STATE_R;
    fsm12.sfd = fd1;
    fsm12.dfd = fd2;

    fsm21.state = STATE_R;
    fsm21.sfd = fd2;
    fsm21.dfd = fd1;
    while(fsm12.state != STATE_T || fsm21.state!=STATE_T)//如果不是T态，循环
    {
        fsm_driver(&fsm12);
        fsm_driver(&fsm21);
    }
    fcntl(fd1, F_SETFL, fd1_save); //恢复
    fcntl(fd2, F_SETFL, fd2_save); //恢复
}
int main(){
    int fd1, fd2;
    fd1 = open(TTY1,O_RDWR);
    if(fd1<0)
    {
        perror("open()");
        exit(1);
    }
    write(fd1,"TTY1\n",5);
    fd2 = open(TTY2, O_RDWR | O_NONBLOCK); //以非阻塞方式打开
    if(fd2<0)
    {
        perror("open()");
        exit(1);
    }
    write(fd2,"TTY2\n",5);
    relay(fd1,fd2);
    close(fd1);
    close(fd2);
}
```
### 终端
在 Ubuntu 上，TTY 是指终端设备。
按下 Ctrl + Alt + F11：
这会将你切换到虚拟终端11 (TTY11)。
如果 TTY11 没有配置，可能会显示一个黑屏或者没有活动的会话。
**终端和 Shell 的关系**
终端：提供用户与系统交互的接口。
Shell：终端内运行的命令解释器（如 Bash、Zsh）。
交互方式：终端为用户提供一个 Shell 会话，用户通过输入命令与操作系统交互