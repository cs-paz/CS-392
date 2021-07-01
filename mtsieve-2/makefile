 # Name: Christian Szablewski-Paz
 # Pledge: I pledge my honor that I have abided by the Stevems Honor System.
CC     = gcc
C_FILE = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(C_FILE))
CFLAGS = -g -Wall -Werror -pedantic-errors -pthread -O3

all:
	$(CC) $(CFLAGS) $(C_FILE) -o $(TARGET) -lm
clean:
	rm -f $(TARGET) $(TARGET).exe
