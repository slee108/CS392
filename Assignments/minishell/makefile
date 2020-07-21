###############################################################################
# Author: Michael DelGaudio & Sunmin Lee
# Date: 06/17/2020
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: MiniShell Makefile
###############################################################################

CC     = gcc
C_FILE = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(C_FILE))
CFLAGS = -g -Wall -Werror -pedantic-errors

all:
	$(CC) $(CFLAGS) $(C_FILE) -o $(TARGET)
clean:
	rm -f $(TARGET) $(TARGET).exe