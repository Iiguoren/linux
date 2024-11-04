# I/O操作(input&output)
io是一切实现的基础，IO分为标准IO:stdio,系统调用IO/文件IO:sysio(APUE第五章第三章)。
## stdio函数
标准IO是对系统调用IO的封装，标准IO依赖于系统调用IO;都可以使用优先使用标准IO,例如：
标准IO fopen函数在linux环境下以来open函数，windows环境下以来openfile函数。因此标准IO可移植性大于系统调用IO
```c
// stdio:FILE类型贯穿始终，FILE是描述文件的结构体
fopen();
fclose();
fgetc();
fputc();
fgets();
fputs();
fread();
fwrite();

printf();
scanf();
fseek();
ftell();
rewind();

fflush();
```
fopen:
```c
FILE *fopen(const char *path, const char *mode);
// 返回一个FILE类型的结构体，接受两个char常量指针参数都用const修饰，防止拷贝和保证只读；如果fopen失败返回一个NULL,并置为errno表明err
// 第一个参数表示文件路径
/* 第二个参数：
    r:以只读打开，定位在开头(第一个有效字符)
    r+:以读写打开文件，定位在开头
    w:只写权限，有则清空文件没有就创建文件，定位文件开始处
    w+:读写权限，有则清空文件没有就创建文件，定位文件开始处
    a:以追加形式写在文件末尾处，不可读，流的指针定位在文件末尾(最后一个有效字节下一个位置)
    a+:以追加形式写在文件末尾处，可读，没有就创建文件；读操作就从文件开始处，写操作就从文件结尾
*/
```
**fopen应用**：
```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
int main(){
        FILE *fi;
        fi = fopen("tmp", "r");
        if(fi == NULL){
                fprintf(stderr, "fopen() failed!errno is %d\n",errno);
                exit(1);
        }
        puts("OK");
        exit(0);
}
```
`gcc -o program stdio.c `编译,`./program`查看输出结果：`fopen() failed!errno is 2`;这里输出了errno的值对比宏可得2代表没有参数中文件；
### errno
在fopen()失败后会置为errno，errno 被定义为一个宏，它会调用一个函数或表达式，获取当前线程的 errno 值。例如，在某些系统中，errno 可能被定义为一个宏，如#define errno (*__errno_location())，该宏会根据当前线程上下文返回不同的 errno 值。
实例函数：
```c
#include<errno.h>
errno;
```
gcc -E预编译，查看结果：
```
# 37 "/usr/include/errno.h" 3 4
extern int *__errno_location (void) __attribute__ ((__nothrow__ , __leaf__)) __attribute__ ((__const__));
# 52 "/usr/include/errno.h" 3 4
# 2 "test.c" 2
(*__errno_location ())
# 2 "test.c"
    ;
```
可看到errno变量消失已经被宏替换`(*__errno_location ())`；

### perror
```c
perror - print a system error message
void perror(const char *s);
// 唯一一个参数接受指向字符的指针，实际上是接受一个字符串；字符串被视为一个字符数组的指针
```
perror是以message形式输出errno;
```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
int main(){
        FILE *fi;
        fi = fopen("tmp", "r");
        if(fi == NULL){
                //fprintf(stderr, "fopen() failed!errno is %d\n",errno);
                perror("fopen():");
                exit(1);
        }
        puts("OK");
        exit(0);
}
```
结果：`fopen():: No such file or directory`

### stderror
```c
NAME
       strerror,  strerror_r, strerror_l - return string describing error num‐
       ber  // 接受一个字符串，在接受的字符串后添加errno描述，返回这个字符串

SYNOPSIS
       #include <string.h>  //必须引入这个头文件
       char *strerror(int errnum);
```
**实例：**
```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
int main(){
        FILE *fi;
        fi = fopen("tmp", "r");
        if(fi == NULL){ 
                fprintf(stderr, "fopen() failed!errno is %s\n",strerror(errno));
                exit(1);
        }
        puts("OK");
        exit(0);
}
-----------------------------------
结果：fopen() failed!errno is No such file or directory
```

**试题1**：
```c
char *ptr = "abc";
ptr[0] = 'x';
//是否可以输出ptr为："xbc"?

//字符指针ptr指向了一个字符常量，字符常量的内容是不可修改的；ptr[0]会导致程序修改只读内存，在大多数编译器下是非法行为
```



