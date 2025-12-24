all: task1 task4

task1: task1.c
	gcc -Wall -g -m32 task1.c -o task1

task4: task4.c
	gcc -Wall -g -m32 -no-pie -fno-stack-protector task4.c -o task4

clean:
	rm -f task1 task4