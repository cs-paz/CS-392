###############################################################################
# Author: Christian Szablewski-Paz & Zac Schuh
# Date: 3/3/21
# Pledge: I pledge my honor that I have abided by the Stevens honor system.
# Description: Makefile for sort.o
###############################################################################
sort: sort.o quicksort.o
	gcc sort.o quicksort.o -o sort
	
sort.o: sort.c 
	gcc -Wall -Werror -pedantic-errors -c sort.c
	
quicksort.o: quicksort.c quicksort.h
	gcc -Wall -Werror -pedantic-errors -c quicksort.c

clean:
	rm -f *.o sort sort.exe

