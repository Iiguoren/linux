# I/O操作(input&output)
io是一切实现的基础，IO分为标准IO:stdio,系统调用IO/文件IO:sysio(APUE第五章第三章)。
## stdio函数
标准IO是对系统调用IO的封装，标准IO依赖于系统调用IO;都可以使用优先使用标准IO,例如：
标准IO fopen函数在linux环境下以来open函数，windows环境下以来openfile函数。因此标准IO可移植性大于系统调用IO
### 格式化字符串
格式化字符串是一种包含文本和格式说明符的字符串，用于指定输出的内容和排版方式。格式说明符通常以 % 开头；
`printf("Name: %s, Age: %d, Height: %.2f, Initial: %c\n", name, age, height, initial);`
在这个示例中，格式化字符串为 "Name: %s, Age: %d, Height: %.2f, Initial: %c\n"，其中：
%s 被替换为字符串变量 name 的值 "Alice"。
%d 被替换为整数变量 age 的值 25。
%.2f 表示输出一个小数点后保留两位的浮点数，用 height 替换，输出 1.75。
%c 被替换为字符变量 initial 的值 A。
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
**实例：fopen应用**：
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
       int getchar(void); // getc的特定形式，指定FILE为stdin
       int ungetc(int c, FILE *stream);
// fgetc读取stream中的下一个字符并且返回一个字符转换的int型，或者返回EOF在文件末尾或者error情况下
/* getc等同于fgetc，但是以宏的形式实现
用于从标准输入（stdin）中读取一个字符，不需要显式指定文件流。它等价于 getc(stdin)，默认从键盘读取输入*/
// ungetc 是一个 C 标准库函数，用于将读取的字符“退回”到输入流中，这样它就可以在下一次读取时再次被读取。它可以用于标准输入 stdin 或文件流
// *fgets可以从输入流接受多个字符并存储在指针s指向的buffer中，遇到'\n'或者EOF停止并返回s，如果到了文件末尾返回NULL，buffer最后一个位置会设置为'\0'
```
fgetc 通常实现为一个函数，而 getc 可能实现为宏。
这意味着在某些编译器或环境中，getc 的执行速度可能比 fgetc 更快，因为宏调用通常会比函数调用少一步压栈和返回的开销。
## fputc(), putc(), putchar()
```c
NAME
       fputc, fputs, putc, putchar, puts - output of characters and strings
SYNOPSIS
       #include <stdio.h>
       int fputc(int c, FILE *stream);
       int fputs(const char *s, FILE *stream);
       int putc(int c, FILE *stream);
       int putchar(int c);
       int puts(const char *s);
// fputc接受一个字符并输出到流中
// 接受一个字符串，输出到流中
// putc和fputc作用相同，putc用宏实现
// puts是fputs的特例，即putc(const char *s, stdout);
```
**实现**：一个复制文件的程序：
```c
#include <stdio.h>
#include <stdlib.h>
#define BUFSIZE 1024

int main(int argc, char** argv){
	FILE *fps, *fpd;
	
        char buf[BUFSIZE];
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
        // 字符实现
        /*
        int ch;
	while(1){
		ch = fgetc(fps);
		if(ch == EOF)
			break;
		fputc(ch, fpd);
	} 
        // fgets字符串实现
        while(fgets(buf, BUFSIZE, fps)!=NULL)
                fputs(buf, fpd);	
        */
        // fread实现
        int n;
        while((n = fread(buf, 1, BUFSIZE, fps)) > 0)
		fwrite(buf,1, n, fpd);
        fclose(fps);
	fclose(fpd);
}
```
## fread() fwrite
```c
       #include <stdio.h>
       size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
       size_t fwrite(const void *ptr, size_t size, size_t nmemb,
                     FILE *stream);
        // fread()/fwrite从流中读取/向流中写入nmemb数量item,每个size字节大小。存放在/读取ptr指针指向的buffer内存。返回成功读取/写入的对象的个数
```

## fprintf(), printf(),sprintf()
```c
 #include <stdio.h>
       int printf(const char *format, ...);
       int fprintf(FILE *stream, const char *format, ...);
       // printf是printf的特例，等同于fprintf(stdout, const char *format, ...);
       int sprintf(char *str, const char *format, ...);
        // sprintf可以将多种类型按照格式符的形式放入字符串中，返回str字符个数
```
**实例**：
```c
#include <stdio.h>
#include <stdlib.h>
int main(){
	char buf[1024];
	int year = 2024, month = 11, day = 6;
	sprintf(buf, "%d=%d=%d", year,month, day);
	puts(buf);
	exit(0);
}
```
## scanf(),fscanf(),sscanf()
```c
       #include <stdio.h>

       int scanf(const char *format, ...);
       int fscanf(FILE *stream, const char *format, ...);
       int sscanf(const char *str, const char *format, ...);
        // scanf从标准输入stdin中按照格式字符串中接受字符
        //fscanf可以从指定文件流传入给格式字符串
        //sscanf从一个字符串按照格式符输入
```


### 缓冲区：
**行缓冲**：换行刷新，满了刷新，将数据一次性写入目标文件或设备，stdin,stdout默认行缓存
**全缓冲**：满了刷新，强制刷新，将数据写入目标文件或设备，文件io默认全缓冲
**无缓冲**：数据不经过缓冲区，每次调用 I/O 函数时都会直接输出或写入文件
```c
#include<stdio.h>
#include<stdlib.h>
int main(){
printf("before");
while(1);
printf("arter");
return 1;
}
不会输出before,如果使用printf("before");会输出before或者使用fflush(stdout)刷新缓冲区也可以输出
```
**实现**：读取文件大小
```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv[]){
        FILE *fps;
        fps = fopen(argv[1]);
        if(fps == NULL){
                perror(fopen());
                exit(0);
        }
        fseek(fps, 0, SEEK_END);
        printf("%d", ftell(fps));
        fclose(fps);
        exit(1);
}

## getline
```c
 #include <stdio.h>
       ssize_t getline(char **lineptr, size_t *n, FILE *stream);
       //getline获取一个指向buffer指针的指针，一个指向size_t表示缓冲区的大小，和一个FILE流；将第一行存入buffer中，如果buffer内存不够会自动分配大小；返回行的字节数或没有返回-1
```
**实例**：
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
	FILE *fps;
	char *buf = NULL;
	size_t num = 0;
	if(argc<2){
		fprintf(stderr, "Usage:%s <src_file> \n",argv[0]);
		exit(1);
		}
	fps = fopen(argv[1],"r");
	if(fps == NULL){
		perror("fopen()");
		exit(1);
		}
	while(1){
		if(getline(&buf, &num, fps) < 0){
			printf("finish\n");
			break;}
		printf("%ld\n", strlen(buf));
		printf("%ld\n", num);
	}
	fclose(fps);
	exit(0);
}
```

## 临时文件tmpnam(),tempfile()
```c
  #include <stdio.h>
  char *tmpnam(char *s);
// 创建一个临时文件，先产生文件名字2、创建文件
  FILE *tmpfile(void);
// 创建临时文件，返回一个FILE结构体用来操作，原子化的创建，并且即使没有close掉在程序结束时候也会自动清理内存，没有泄露风险。
```

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
fopen函数返回的指针对象内存不会在函数内消失，因此不在栈；函数fopen声明FILE变量时候，如果储存在静态区因为静态区的静态变量在全局作用域只声明一次，就只能打开一个文件，所以不在静态区；又因为堆上面的内存要手动回收而且fopen有fclose对应因此在堆上。

**宏和函数在编译上有什么区别？**
宏占用编译时间不占用调用时间；函数不占用编译时间只占用调用时间

