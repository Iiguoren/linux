## makefile学习
### 简单示例
```makefile
test1:
    echo "first"
test2:
    echo "second"
```
makefile中重要的两个概念：目标，规则，目标是所要构造的对象：test1,test2，规则是表达目标的方式:echo " "。
注： 在makefile中规则(echo "first")前必须是制表符而不能是空格
* 如果直接make: 默认构造第一个目标test1
```makefile
echo "first"
"first"
``` 

* make test1: 
```makefile
echo "first"
"first"
```

* make test2:
 ```makefile
echo "second"
"second"
``` 
在规则前加@，终端不会输出规则的命令
```makefile
test1:
    @echo "first"
test2:
    @echo "second"
```
### 编译单个文件
在编译中经常使用obj中间文件或者静态库和动态库，目标文件依赖这些文件，因此引入make的第三个重要因素**先决条件**
在目标文件后:加入先决条件代表所依赖的文件
要编译单个文件
```makefile
all:fun.c
	gcc -o fun fun.c
fun.exe:fun.c
	gcc -o fun.exe fun.c
clean:
	del fun.exe
```
编译"all"目标依赖fun.c，显示声明目标依赖文件。
<Make 有内置的​​隐式规则​​，可以自动推导某些依赖关系。如即使没有显式声明fan.c也可以编译成功>
这里all目标是虚拟目标并不生成，因此在构建之后再次make会因为没有找到all目标文件而重新编译;
在linux中使用rm ，windows中使用del
如果改为：
```makefile
fun.exe:fun.c
	gcc -o fun.exe fun.c
```
第二次编译：make: 'fun.exe' is up to date.
注意：由于windows的可执行文件有exe后缀，执行 make fun 时，Make会检查是否存在名为fun的文件（无扩展名）
由于实际生成的是 fun.exe，Make 认为 fun 文件不存在 → ​​总是触发编译​

### 分布式编译
大部分项目都不会只有一个源码文件，这时候必须采用分布式编译的方式：
main.c ---> main.o --
                    |-----> simple.exe
foo.c  ---> foo.o  --

先将各个源文件生成目标.o文件，最后链接为可执行文件
```makefile
all: main.o foo.o
	gcc -o simple.exe main.o foo.o
main.o:main.c	
	gcc -o main.o -c main.c
foo.o:foo.c
	gcc -o foo.o -c foo.c
clean:
	del main.o foo.o simple.exe
```
我们不使用假目标all，使用真实目标simple.exe，连续编译两次：make: 'simple.exe' is up to date.
```makefile
.PHONY: simple.exe
simple.exe: main.o foo.o
	gcc -o simple.exe main.o foo.o
main.o:main.c	
	gcc -o main.o -c main.c
foo.o:foo.c
	gcc -o foo.o -c foo.c
clean:
	del main.o foo.o simple.exe
```
如果我想自己构造一个假目标，采⽤.PHONY 关键字声明⼀个⽬标后，make 并不会将其当作⼀个⽂件来处理，⽽只是当作⼀个概念上的⽬标。对于假⽬标，我们可以想像的是由于并不与⽂件关联，所以每⼀次 make 这个假⽬标时，其所在的规则中的命令都会被执⾏
### 变量
```makefile
.PHONY: simple.exe
rm = del
exe = simple.exe
objs = main.o foo.o
simple.exe: $(objs)
	gcc -o $(exe) $(objs)
main.o:main.c	
	gcc -o main.o -c main.c
foo.o:foo.c
	gcc -o foo.o -c foo.c
clean:
	$(rm) $(exe) $(objs)
```

使用变量我们可以在文件头部就清楚知道 可执行文件名， obj文件，使用变量的方式：`$(param)`

### 自动变量
每个编译规则的 目标文件， 先决条件会在规则中反复出现，因此makefile中提供了一些固定的变量：
* $@⽤于表示⼀个规则中的⽬标。当我们的⼀个规则中有多个⽬标时，$@所指的是其中任何造成命令被运⾏的⽬标。
* $^则表示的是规则中的所有先择条件。
* $<表示的是规则中的第⼀个先决条件。
```makefile
.PHONY: simple.exe
rm = del
exe = simple.exe
objs = main.o foo.o
simple.exe: $(objs)
	gcc -o $@ $^
main.o:main.c	
	gcc -o $@ -c $<
foo.o:foo.c
	gcc -o $@ -c $<
clean:
	$(rm) $(exe) $(objs)
```
同时make给了两种变量赋值：递归扩展变量，简单扩展变量
递归扩展变量： 变量值在​​被引用时​​才会展开（惰性求值）。
简单扩展变量： 变量值在​​赋值时立即展开​​（立即求值）。
对比：
```makefile
VAR1 = Hello
VAR2 = $(VAR1) World  # VAR2 的值是 "Hello World"

VAR1 = Bye            # 修改 VAR1

all:
    @echo $(VAR2)     # 输出 "Bye World"（因为 VAR2 引用了最新的 VAR1）
```

```makefile
VAR1 := Hello
VAR2 := $(VAR1) World  # VAR2 的值是 "Hello World"
VAR1 := Bye            # 修改 VAR1
all:
    @echo $(VAR2)      # 输出 "Hello World"（VAR2 的值在赋值时已固定）
```
此外还有+=用来在原来变量增加内容

### 模式
对于大型项目，对于每一个文件都写一条规则太过于繁琐了，因此Make中提供了模式功能：模式功能是基于​​文件系统中的实际文件​进行匹配，将一部分工作自动化
```makefile
.PHONY: simple.exe
rm = del
exe = simple.exe
objs = main.o foo.o
simple.exe: $(objs)
	gcc -o $@ $^
%.o: %.c
	gcc -o $@ -c $^
clean:
	$(rm) $(exe) $(objs)
```
### 常用函数
#### wildcard
类似linux的通配符，例如：
```makefile
.PHONY:all
SRC=$(wildcard *.c)
all:
	@echo "SRC = $(SRC)"
```
SRC = foo.c main.c
#### addprefix
addprefix 函数是⽤来在给字符串中的每个⼦串前加上⼀个前缀
```makefile
.PHONY:all
without_dir= foo.c bar.c main.c
with_dir:=$(addprefix objs/, $(without_dir))
all:
	@echo $(with_dir)
```
objs/foo.c objs/main.c

### 设置编译文件目录
```makefile
.PHONY: simple.exe
rm = del
exe = simple.exe
dirs=$(dir_exe) $(dir_objs)
dir_exe = exes
exe := $(addprefix $(dir_exe)/, $(exe))
srcs = $(wildcard *.c)
dir_objs = objs
objs = $(srcs:.c=.o)  # srcs变量中所有以 .c 结尾的文件名替换为以 .o 结尾的文件名
objs:=$(addprefix $(dir_objs)/, $(objs))
all: $(dirs) $(exe)
$(dirs):
	mkdir $@
$(exe):$(objs)
	gcc -o $@ $^
$(dir_objs)/%.o:%.c    # 使用通配符找到objs文件夹中的obj文件，作为目标实际上和objs变量相同
	gcc -o $@ -c $^
clean:
	$(rm) -fr $(dirs)
```
### 设置头文件
增加头文件：
```h
// foo.h
#ifndef __FOO_H
#define __FOO_H
void foo();
#endif
```
目前照用上面的makefile文件，没有增加对foo.h的依赖关系，但是依旧可以编译成功。
如果我们修改头文件：
```h
// foo.h
#ifndef __FOO_H
#define __FOO_H
void foo(int value);
#endif
```
这时候头文件和.c文件声明不同，预期结果应该是再次编译报错，实验：
`make: Nothing to be done for 'all'.`
这是因为makefile并没有对.h的依赖，检查不到.h文件的修改，如果清除编译后再次编译：
```
main.c:5:5: error: too few arguments to function 'foo'
     foo();
     ^~~
```
报错；

因此需要增加对头文件的依赖：
```makefile
.PHONY: simple.exe
rm = del
exe = simple.exe
dirs=$(dir_exe) $(dir_objs)
dir_exe = exes
exe := $(addprefix $(dir_exe)/, $(exe))
srcs = $(wildcard *.c)
dir_objs = objs
objs = $(srcs:.c=.o)  # srcs变量中所有以 .c 结尾的文件名替换为以 .o 结尾的文件名
objs:=$(addprefix $(dir_objs)/, $(objs))
all: $(dirs) $(exe)
$(dirs):
	mkdir $@
$(exe):$(objs)
	gcc -o $@ $^
$(dir_objs)/%.o:%.c foo.h  # 增加foo.h文件
	gcc -o $@ -c $<        # 将$^改为#<
clean:
	$(rm) /s $(dirs)
```
这样当修改.h文件后，make系统能识别依赖的修改从而重新编译
### 创建依赖关系
手动维护头文件依赖关系及其麻烦且容易出错，makefile可以使用dep文件自动生成并引入依赖关系：
在 Makefile 中，.dep 文件（或 .d 文件）是 ​​依赖关系文件​​，用于自动跟踪源代码文件之间的依赖关系（尤其是头文件依赖）。它的核心作用是解决以下问题：
* 当源代码文件（.c）包含头文件（.h）时，确保修改头文件后，所有依赖该头文件的代码文件会被自动重新编译。

例如：
main.c 引用了 foo.h
如果你修改了 foo.h，Makefile 需要知道：
main.o 应该重新编译
所有其他引用了 foo.h 的目标文件也需要重新编译
使用gcc编译器的-MM参数可以自动生成依赖关系：
`gcc -MM main.c  # 输出: main.o: main.c utils.h`
makefile中:
```makefile
dirs=$(dir_exe) $(dir_objs) $(dir_dep)
dir_exe = exes
exe = simple.exe
exe := $(addprefix $(dir_exe)/, $(exe))
srcs = $(wildcard *.c)
dir_objs = objs
objs = $(srcs:.c=.o)
objs:=$(addprefix $(dir_objs)/, $(objs))
dir_dep=dep
dep=$(srcs:.c=.dep)
dep:=$(addprefix $(dir_dep)/, $(dep))
all:$(dirs) $(exe) $(dep)

$(dirs):
	@mkdir $@
$(exe):$(objs)
	gcc -o $@  $^
$(dir_objs)/%.o:%.c
	gcc -o $@ -c $^
$(dir_dep)/%.dep:%.c
	@echo "Making dep..."
	@set -e ; \
	rm -rf $@.tmp
	@gcc -MM $^ > $@.tmp ; \
	sed 's,\(.*\)\.o[ :]*,objs/\1.o: ,g' < $@.tmp > $@ ; \
	rm -rf $@.tmp
clean:
	rm -rf $(dirs)
```
使用了addprefix函数增加文件夹前缀
使用了wildcard函数进行通配符匹配文件名
使用了$(param:.c=.o)的make自动修改后缀方法
sed和正则表达式将gcc -MM输出写入.dep文件

现在我们有了dep依赖文件，那么如何使用这些文件让make自动查询依赖呢：
`include $(dep)`
使用include包含依赖
```makefile
dirs=$(dir_exe) $(dir_objs) $(dir_dep)
dir_exe = exes
exe = simple.exe
exe := $(addprefix $(dir_exe)/, $(exe))
srcs = $(wildcard *.c)
dir_objs = objs
objs = $(srcs:.c=.o)
objs:=$(addprefix $(dir_objs)/, $(objs))
dir_dep=dep
dep=$(srcs:.c=.dep)
dep:=$(addprefix $(dir_dep)/, $(dep))
all:$(dirs) $(exe) $(dep)

include $(dep)

$(dirs):
	@mkdir $@
$(exe):$(objs)
	gcc -o $@  $^
$(dir_objs)/%.o:%.c
	gcc -o $@ -c $^
$(dir_dep)/%.dep:%.c
	@echo "Making dep..."
	@set -e ; \
	rm -rf $@.tmp
	@gcc -MM $^ > $@.tmp ; \
	sed 's,\(.*\)\.o[ :]*,objs/\1.o: ,g' < $@.tmp > $@ ; \
	rm -rf $@.tmp
clean:
	rm -rf $(dirs)
```
第一次运行报错：
```
/bin/sh: 1: cannot create dep/main.dep.tmp: Directory nonexistent
/bin/sh: 2: cannot open dep/main.dep.tmp: No such file
Making dep...
/bin/sh: 1: cannot create dep/foo.dep.tmp: Directory nonexistent
/bin/sh: 2: cannot open dep/foo.dep.tmp: No such file
```
这是因为make运行Include时会先查找要引入的目录，这时因为目录还没有创建所以报错，同样所以很⾃然 make在处理gcc -MM指令时，是找不到依赖⽂件的。
此外，make在检测到文件夹和dep文件不存在会查找是否有规则去更新他们，如果有，就运行，那么为什么这一次没有运行自动生成dep文件？makefile检查include内容，(include和make all的顺序与前后位置无关，include优先于目标)，不存在就会尝试根据已有规则创建，而目录创建依赖all文件，此时目录还不存在，所以⽆法成功的构建依赖⽂件。有了这些信息之后，我们需要对 Makefile 的依赖关系进⾏调整，即将 dir_dep的创建放在构建依赖的dep的先决条件中：
```makefile
dirs=$(dir_exe) $(dir_objs) $(dir_dep)
dir_exe = exes
exe = simple.exe
exe := $(addprefix $(dir_exe)/, $(exe))
srcs = $(wildcard *.c)
dir_objs = objs
objs = $(srcs:.c=.o)
objs:=$(addprefix $(dir_objs)/, $(objs))
dir_dep=dep
dep=$(srcs:.c=.dep)
dep:=$(addprefix $(dir_dep)/, $(dep))
all:$(exe) 

include $(dep)

$(dirs):
	mkdir $@
$(exe):$(objs) |$(dir_exe) 
	gcc -o $@  $(filter %.o, $^)
$(dir_objs)/%.o:%.c |$(dir_objs)
	gcc -o $@ -c $(filter %.c, $^)
$(dir_dep)/%.dep:%.c |$(dir_dep) 
	@echo "Making dep..."
	@set -e ; \
	rm -rf $@.tmp
	@gcc -MM $(filter %.c, $^) > $@.tmp ; \
	sed 's,\(.*\)\.o[ :]*,objs/\1.o: ,g' < $@.tmp > $@ ; \
	rm -rf $@.tmp


clean:
	rm -rf $(dirs)
```
而all中只需要exe
注意:先决条件中|将后面的作为order_only，不进行时间戳更新检查，如果不这么做，会导致include 生成dep->dep先执行先决条件dir_dep->dir_dep时间戳更新导致include更新dep->的无限循环
而为了不让先决条件中的dep导致错误参数传入gcc中使用filter函数将$(filter %.c, $^)所需要的.c文件或.o文件过滤出来

