# Makefile  user
CC   = g++
LD   = g++

CFLAGS = -g -Wall -Wextra -ansi -pedantic -std=c++17

AUXLIB = parser.h
SOCKETLIB = socket_functions.h
COMMANDSLIB = commands.h

USER_OPTIONS ?=

.PHONY: all clean run

all: client

client: socket.o parser.o commands.o client.o 
	$(LD) $(CFLAGS) -o user socket.o parser.o commands.o client.o 

aux.o: parser.cpp $(AUXLIB) constants.h
	$(CC) $(CFLAGS) -o parser.o -c parser.cpp

client.o: client.cpp $(COMMANDSLIB) $(SOCKETLIB) $(AUXLIB) constants.h
	$(CC) $(CFLAGS) -o client.o -c client.cpp

commands.o: commands.cpp $(COMMANDSLIB) $(SOCKETLIB) $(AUXLIB) constants.h
	$(CC) $(CFLAGS) -o commands.o -c commands.cpp

socket.o: socket_functions.cpp $(SOCKETLIB) $(AUXLIB) constants.h
	$(CC) $(CFLAGS) -o socket.o -c socket_functions.cpp

run: user
	@echo run with make run USER_OPTIONS="...." to execute the user	
	./user $(USER_OPTIONS)

clean:
	@echo Cleaning files generated
	rm -f *.o user