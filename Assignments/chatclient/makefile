###############################################################################
# Author: Michael DelGaudio & Sunmin Lee
# Date: 06/30/2020
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: CS-392 ChatClient Makefile
###############################################################################

CC     = gcc
S_FILE = chatserver.c
S_TARGET = chatserver
C_FILE = chatclient.c
TARGET = chatclient
CFLAGS = -g -Wall -Werror -pedantic-errors 

all: chatclient.c
	$(CC) $(CFLAGS) $(C_FILE) -o $(TARGET)
server:
	$(CC) $(CFLAGS) $(S_FILE) -o $(S_TARGET)
clean:
	rm -f $(TARGET) $(TARGET).exe $(S_TARGET) $(S_TARGET).exe