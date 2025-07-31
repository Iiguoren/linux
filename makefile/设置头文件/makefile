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
$(dir_objs)/%.o:%.c foo.h  # 使用通配符找到objs文件夹中的obj文件，作为目标实际上和objs变量相同
	gcc -o $@ -c $<
clean:
	$(rm) /s $(dirs)