FLAGS =-Wall -g
CC = gcc #if we want to change compiler

all: sniffer myping

myping:myping.c
	$(CC) $(FLAGS) myping.c -o myping

sniffer:sniffer.c
	$(CC) $(FLAGS) sniffer.c -o sniffer -lpcap

.PHONY: clean
clean:
	rm -f *.o *.a *.so myping sniffer
runp:
	sudo ./myping

runs:
	sudo ./sniffer