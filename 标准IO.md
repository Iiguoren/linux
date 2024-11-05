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

## fclose()
```C
NAME
       fclose - close a stream
SYNOPSIS
       #include <stdio.h>
       int fclose(FILE *stream);
```
fopen和fclose成对出现
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
        fclose(fi);
        exit(0);
}
```

### 权限问题
如果文件不存在，会自动创建文件，而自动创建的文件的权限是怎么决定的呢？
默认创建的文件权限遵循：
`0666 & ~umask`,umask一般为0002;
都换位二进制数为110 110 110 & ~ (000 000 010)
即 110 110 100（0664）换位权限rw- rw- r--

## fgetc(),getc(),getchar()
```c
NAME
       fgetc, fgets, getc, getchar, ungetc - input of characters and strings
SYNOPSIS
       #include <stdio.h>
       int fgetc(FILE *stream);  //定义为函数
       char *fgets(char *s, int size, FILE *stream);
       int getc(FILE *stream);  // 定义为宏
       int getchar(void);
       int ungetc(int c, FILE *stream);
// fgetc读取stream中的下一个字符并且返回一个字符转换的int型，或者返回EOF在文件末尾或者error情况下
// getc等同于fgetc，但是以宏的形式实现
//用于从标准输入（stdin）中读取一个字符，不需要显式指定文件流。它等价于 getc(stdin)，默认从键盘读取输入
// ungetc 是一个 C 标准库函数，用于将读取的字符“退回”到输入流中，这样它就可以在下一次读取时再次被读取。它可以用于标准输入 stdin 或文件流
// *fgets可以从输入流接受多个字符并存储在字符串s中
```

**实现**：一个复制文件的程序：
```c
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char** argv){
	FILE *fps, *fpd;
	int ch;
	if(argc<3){
		fprintf(stderr, "Usage:%s <src_file> <dest_file>\n",argv[0]);
		exit(1);
	}
	fps = fopen(argv[1], "r");
	if(fps == NULL){
		perror("fopen()");
		exit(1);
	}
	
	fpd = fopen(argv[2], "w");
	if(fpd == NULL){
		perror("fopen()");
		exit(1);
	}
	while(1){
		ch = fgetc(fps);
		if(ch == EOF)
			break;
		fputc(ch, fpd);
	}	
	fclose(fps);
	fclose(fpd);
}
```
fgetc 通常实现为一个函数，而 getc 可能实现为宏。这意味着在某些编译器或环境中，getc 的执行速度可能比 fgetc 更快，因为宏调用通常会比函数调用少一步压栈和返回的开销。
**试题1**：
```c
char *ptr = "abc";
ptr[0] = 'x';
//是否可以输出ptr为："xbc"?

//字符指针ptr指向了一个字符常量，字符常量的内容是不可修改的；ptr[0]会导致程序修改只读内存，在大多数编译器下是非法行为
```
## **思考题**：
**fopen函数返回的FILE结构体指针是指向栈/静态区/堆？**
栈：栈上的内存由编译器自动分配和释放，通常用于存储局部变量和函数调用时的参数、返回地址等。

堆：堆上的内存由程序员手动分配和释放，使用 malloc、calloc（在 C 中）或 new（在 C++ 中）动态分配，使用 free（C）或 delete（C++）释放。

静态区：由编译器在程序开始运行时分配，程序结束时释放。用于存储全局变量、静态变量（static 修饰的变量）、常量（字符串常量等）。
fopen函数返回的指针对象内存不会在函数内消失，因此不在堆；函数fopen声明FILE变量时候，如果储存在静态区因为静态区的静态变量在全局作用域只声明一次，就只能打开一个文件，所以不在静态区；又因为堆上面的内存要手动回收而且fopen有fclose对应因此在堆上。

**宏和函数在编译上有什么区别？**
宏占用编译时间不占用调用时间；函数不占用编译时间只占用调用时间

