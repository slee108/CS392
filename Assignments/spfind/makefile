###############################################################################
# Author: Michael DelGaudio & Sunmin Lee
# Date: 06/06/2020
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: CS-392 Assignment 4 - Sorted PFind Makefile
###############################################################################

CC     = gcc
C_FILE = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(C_FILE))
CFLAGS = -g -Wall -Werror -pedantic-errors

spfind: spfind.c
	$(CC) $(CFLAGS) spfind.c -o $(TARGET)
clean:
	rm -f $(TARGET) $(TARGET).exe