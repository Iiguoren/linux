# 系统调用IO
## 文件描述符fd
文件描述符是用来定位内存中一个FILE结构体数组的一个int型下标
```c
open();
close();
read();
write();
lseek();
```
## open(),close()
```c
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
       int open(const char *pathname, int flags);
       int open(const char *pathname, int flags, mode_t mode);
       int creat(const char *pathname, mode_t mode);
       /* open没有缓冲区机制，而且open采用文件模式标志控制文件流:O_RDONLY(只读)、O_WRONLY(只写)、O_RDWR(读写)、O_CREAT(生成)、O_APPEND(追加)、O_TRUNC(截断),O_NONBLOCK：非阻塞模式。如果没有数据可读，read 调用会立即返回，不会阻塞进程。;返回文件描述符，失败则返回-1
        mode_t mode表示O_CREAT创建的文件权限，输入的参数&~umask;*/
```
```c
       #include <unistd.h>
       int close(int fd);
       // 关闭流
```

## read(),write()
```c
       #include <unistd.h>
       ssize_t read(int fd, void *buf, size_t count);
       // 根据文件描述符读取文件数据，参数：文件描述符，缓冲区的指针，读取的最大字节数；返回实际读取的字节数，若为0则到达文件末尾；否则-1   
       #include <unistd.h>
       ssize_t write(int fd, const void *buf, size_t count);
       // 向文件或文件描述符写入数据的系统调用。参数：文件描述符，buf的指针，要写入的字节数。返回实际写入的字节数（ssize_t 类型）。通常情况下，这个值等于 count，但在某些情况下可能小于 count（例如，磁盘空间不足）。失败返回-1
```

```c
#include <unistd.h>
#include <fcntl.h>
off_t lseek(int fd, off_t offset, int whence);
// fseek的系统调用
```
## 重构mycopy
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#define BUFSIZE 1024
int main(int argc, char **argv){
    int fds, fdd;
    char buf[BUFSIZE];
    int len,ret,pos;
    if(argc<3){
        fprintf(stderr, "Usage:%s <src_name> <dest_name>\n", argv[1]);
        exit(1);
    }
    fds = open(argv[1], O_RDONLY);
    if(fds <0){
        perror("open()");
        exit(1);
    }
    fdd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0666);
    while(1){
        len = read(fds, buf, BUFSIZE);
        if(len < 0){
            perror("read()");
            break;
        }
        if(len == 0)
            break;
        pos = 0;
        while(len>0){
            ret = write(fdd, buf + pos, len);
            if(ret<0){
                perror("write()");
                exit(1);
            }
            pos += ret;
            len -= ret;
        }
    }
    close(fdd);
    close(fds);
    exit(0);
}
```

## lseek()
```c
#include <sys/types.h>
       #include <unistd.h>

       off_t lseek(int fd, off_t offset, int whence);
       //使用fd实现fseek功能
```
## 文件共享
多个任务同时操作一个文件，或协同完成任务
写一个程序删除第十行
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#define BUFSIZE 1024
#define TARGET_LINE 10
int main(int argc, char **argv){
    FILE *fileread;
    FILE *filewrite;
    if(argc<2){
        fprintf(stderr,"Usage: %s <file_name>", argv[0]);
        exit(1);
    }

    fileread = fopen(argv[1], "r");
    if(fileread ==NULL){
        perror("fopen()");
        exit(1);
    }
    filewrite = fopen(argv[1], "r+");
    if(filewrite ==NULL){
        perror("fopen()");
        exit(1);
    }

    char buf[BUFSIZE];
    int cur_line = 1;
    long pos = 0;
    while(1){ 
        if(cur_line == TARGET_LINE){//第十行
            printf("after:%s", buf);
            pos = ftell(fileread);
            
            fseek(filewrite,pos,SEEK_SET); // fwrite定位10
            if(fgets(buf, BUFSIZE, fileread)==NULL) //10
            	break;
            
            fgets(buf, BUFSIZE, fileread); //11
            printf("%s", buf);
            fputs(buf,filewrite);
    
            printf("cur:%d\n", cur_line);
            
        }
        if(cur_line > TARGET_LINE){
                   
            if(fgets(buf, BUFSIZE, fileread)!=NULL){
            	printf("d:%s", buf);
                }
            else{
            break;
            }
            fputs(buf,filewrite);
            printf("cur:%d\n", cur_line);
        }
        if(cur_line < TARGET_LINE){
            fgets(buf, BUFSIZE, fileread);
            printf("%s", buf);
        }
        cur_line++;
    }
    if(cur_line<TARGET_LINE){
        printf("no enough lines..");
    }
    long lastline_pos = ftell(filewrite);
    truncate(argv[1], (off_t)lastline_pos); //截断最后一行
    fclose(fileread);
    fclose(filewrite);
    exit(0);
}
```

## 原子操作
原子操作：不可分割的操作
作用：解决竞争和冲突

## 重定向dup(),dup2()
dup() 函数用于复制一个已打开的文件描述符，从而创建一个新的文件描述符，该描述符指向相同的文件表项
```c
    #include <unistd.h>
    int dup(int oldfd);
    //生成一个新的文件描述符，最小且未被占用，指向与 oldfd 相同的文件
    int dup2(int oldfd, int newfd);
    // 将 newfd 复制为 oldfd 的副本。newfd 和 oldfd 可以相同，如果不同并且 newfd 已打开，dup2 会关闭 newfd。
```
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#define FILENAME "/tmp/out"
int main(){
    close(1);
    int fd;
    fd = open(FILENAME, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if(fd<0){
    	perror("open()");
    	exit(1);
    	}
    puts("hello world\n");
    exit(0);
}   
```
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#define FILENAME "/tmp/out"
int main(){
    int fd;
    fd = open(FILENAME, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if(fd<0){
        perror("open()");
        exit(0);
    }
    dup2(fd,1); // 将文件描述符fd复制到1的位置，删除fd
    if(fd != 1){
        close(fd);
    }

    puts("hello world!");
    exit(1);
}
```
将fd位置的FILE复制到1索引位置，puts对1进行输出，就会输出到/tmp/out里面。关闭原本的FILE