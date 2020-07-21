###############################################################################
# Author: Michael DelGaudio & Sunmin Lee
# Date: 06/29/2020
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: CS-392 Lab 11 Makefile
###############################################################################

CC     = gcc
C_FILE = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(C_FILE))
CFLAGS = -g -Wall -Werror -pedantic-errors 

all:
	$(CC) $(CFLAGS) $(C_FILE) -o $(TARGET) -pthread
clean:
	rm -f $(TARGET) $(TARGET).exe