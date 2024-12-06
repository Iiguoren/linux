非阻塞IO --- 阻塞IO
在阻塞 I/O模式下，当一个进程或线程发起 I/O 操作（如读取数据、写入数据）时，它会阻塞并等待 I/O 操作完成。如果操作没有完成，线程会一直处于挂起状态，直到 I/O 完成并返回数据或确认操作成功。

非阻塞 I/O模式下，发起 I/O 操作时，线程或进程不会被阻塞，而是立即返回。即使数据不可用，线程也不会停止执行，而是继续执行后续的代码。线程可以通过反复检查 I/O 操作的状态（例如使用 poll() 或 select()），或者通过回调机制等待 I/O 操作完成。
有限状态机:复杂流程问题
    简单问题：如果一个程序的自然流程是结构化的，是简单流程
    复杂流程：自然流程是非结构化的，是复杂流程（顺序分支判断无法直接解决）


1. 非阻塞IO
2. IO多路转接
3. 其他读写函数
4. 存储映射IO
5. 文件锁

```c
// relay.c


#define TTY1 "/dev/tty11"
#define TTY2 "/dev/tty12"
#define BUFSIZE 1024
enum{
    STATE_R=1, //状态读
    STATE_W, //状态写
    STATE_Ex, //状态写
    STATE_T //状态写
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
                    errstr = "read error\n";
                    fsm->state = STATE_Ex;
                    }
            }
            else{
                fsm->pos = 0;
                fsm->state = STATE_W;}
            break;
        case STATE_W:
            ret = write(fsm->dfd, fsm->buf+pos, fsm->len);
            if(ret < 0){
                if(errno== EAGAIN)
                    fsm->state = STATE_W;
                else{
                    errstr = "write error/n";
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
relay(){
    int fd1_save;
    int fd2_save;
    struct fsm_st fsm12, fsm21;//读左写右和读右写左
    fd1_save = fcntl(fd1, F_GETFL); // 保存状态
    fcntl(fd1, F_SETFL, fd1_save|O_NONBLOCK);

    fd2_save = fcntl(fd2, F_GETFL); // 保存状态
    fcntl(fd2, F_SETFL, fd2_save|O_NONBLOCK);

    // 为状态机赋值
    fsm12.state = STATE_R;
    fsm12.sfd = fd1;
    fsm12.dfd = fd2;

    fsm21.state = STATE_R;
    fsm21.sfd = fd2;
    fsm21.dfd = fd1;
    while(fsm_12.state != STATE_T || fsm21.state!=STATE_T)//如果不是T态，循环
    {
        fsm_driver(fsm12);
        fsm_driver(fsm21);
    }
    fcntl(fd1, F_SETFL, fd1_save); //恢复
    fcntl(fd2, F_SETFL, fd1_save); //恢复
}
int main(){
    int fd1, fd2;
    fd1 = open(TTY1,O_RDWR);
    if(fd1<0)
    {
        perror("open()");
        exit(1);
    }
    fd2 = open(TTY2, O_RDWR_O_NONBLOCK);
    if(fd2<0)
    {
        perror("open()");
        exit(1);
    }
    
    close(fd1);
    close(fd2);
    relay();
}
```