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