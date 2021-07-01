###############################################################################
# Author: Christian Szablewski-Paz & Zac Schuh
# Date: 3/3/21
# Pledge: I pledge my honor that I have abided by the Stevens honor system.
# Description: Makefile for pfind.o
###############################################################################
pfind: pfind.o pfindsource.o
	gcc pfind.o pfindsource.o -o pfind
	
pfind.o: pfind.c 
	gcc -Wall -Werror -pedantic-errors -c pfind.c
	
pfindsource.o: pfindsource.c pfindsource.h
	gcc -Wall -Werror -pedantic-errors -c pfindsource.c

clean:
	rm -f *.o pfind pfind.exe

