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