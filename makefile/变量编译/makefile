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