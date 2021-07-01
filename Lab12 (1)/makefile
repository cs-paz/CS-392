CC     = gcc
CFLAGS = -g -Wall -Werror -pedantic-errors

all: main libsum.so

# TODO: build main
main: main.c 
	$(CC) $(CFLAGS) main.c -o main -ldl
# TODO: build libsum.so
libsum.so: sum.c sum.h
	$(CC) $(CFLAGS) -shared -fPIC sum.c -o libsum.so
clean:
	rm -f libsum.so main main.exe
