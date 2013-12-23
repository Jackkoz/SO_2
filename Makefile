CC=gcc

all: clean client server

client: 
	$(CC) client.c shared_lib.c -o klient

server: 
	$(CC) -pthread server.c shared_lib.c -o serwer

clean:
	rm -rf *o

.PHONY: all clean
