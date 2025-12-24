CC=gcc
CFLAGS=-Wall -g -m32
LDFLAGS=-m32

all: task1 task4

task1: task1.c
	$(CC) $(CFLAGS) $(LDFLAGS) task1.c -o task1

task4: task4.c
	$(CC) $(CFLAGS) -fno-pie -fno-stack-protector -o task4 task4.c

clean:
	rm -f task1 task4