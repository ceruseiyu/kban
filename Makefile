CC = gcc
CFLAGS = -I -Wall -g -lcurl
kban: main.c kban.c
	$(CC) -pthread -o kban main.c kban.c $(CFLAGS)
