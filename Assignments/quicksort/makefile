###############################################################################
# Author: Michael DelGaudio & Sunmin Lee
# Date: 5/27/2020
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: Makefile for quicksort in c
###############################################################################

sort: sort.o quicksort.o
	gcc sort.o quicksort.o -o sort
sort.o: sort.c
	gcc -Wall -Werror -pedantic-errors -c sort.c
quicksort.o: quicksort.c quicksort.h
	gcc -Wall -Werror -pedantic-errors -c quicksort.c
clean:
	rm -f *.o sort sort.exe