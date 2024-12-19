


static void server_job(int sd){
    char buf[1024];
    int len;
    //proto.h中约定为longlong 
    len =sprintf(bufm FMT_STAMP, (long long)time(NULL));
    if(send(sd, buf, len, 0)<0){
        perror("send()");
        exit(1);
    }
}

int main()
{   
    int sd;
    sd = socket(AF_INET, SOCK_STREAM, 0/*IPPROTO_TCP*/ );
    struct sockaddr_in laddr; //当前IP
    struct sockaddr_in raddr; 
    socket_t raddr_len;
    pid_t pid;
    if(sd<0){
        perror();
        exit(1);
    }
    // 解决没有close导致Bind buzy的问题
    int val =1;
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val))<0)
    {
        perror("setsockopt()");
        exit(1);
    }
    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(SERVERPORT)); // 字符串转整型数,转网络字节序
    inet_pton(AF_INET,"0.0.0.0",&laddr.sin_addr)
    if(bind(sd, (void *)&laddr, sizeof(laddr))<0)
    {
        perror("bind()");
        exit(1);
    }
    listen(sd, 200); // 开始监听，最多有200个全连接socket
    raddr_len = sizeof(raddr);
    while(1){
        int newsd;
        // accept()????
        newsd = accept(sd,(void *)&raddr.sin_addr, &raddr_len);
        if(newsd<0) // 在socket进行连接建立
        {
            perror();
            exit(1);
        } 
        pid = fork();
        if(pid<0){
            perror();
            exit(1);
        }
        if(pid == 0){
            /*!!!!*/
            // 关闭自己进程中不需要的文件描述符
            close(sd);
            inet_ntop(AF_INET, &raddr.sin_addr, ipstr, 1024);
            server_job(newsd);
            close(newsd);
            //send();
            exit(0);
        }
        /*!!!*/
        close(newsd); 
        // 这里关闭newsd是为了防止子进程close(newsd)后父进程抓着newsd导致缓冲区不刷新
    }
    
    close(sd);
    exit(0);
}