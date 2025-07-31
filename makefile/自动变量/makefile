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