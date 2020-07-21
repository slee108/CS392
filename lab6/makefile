###############################################################################
# Author: Michael DelGaudio & Sunmin Lee
# Date: 06/11/2020
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: CS-392 Lab 6 - Sum Makefile
###############################################################################

CC     = gcc
CFLAGS = -g -Wall -Werror -pedantic-errors

all: main libsum.so

main: main.c
	$(CC) $(CFLAGS) -o main main.c -ldl 

libsum.so: sum.c sum.h
	$(CC) $(CFLAGS) -shared -fPIC -o libsum.so sum.c 

clean:
	rm -f libsum.so main main.exe