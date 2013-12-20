CC=gcc

all: clean client server

client: 
	$(CC) client.c -o klient

server: 
	$(CC) server.c -o serwer

clean:
	rm -rf *o

.PHONY: all clean
