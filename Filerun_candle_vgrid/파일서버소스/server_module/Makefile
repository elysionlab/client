#Makefile
all:		manager		clean

manager:		manager.o
	gcc -g -D_GNU_SOURCE -lpthread  -L/usr/lib -lz manager.c -o manager

clean:	
	rm  -f   *.o  core
