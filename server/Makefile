CC = g++
LD = g++
CFLAGS = -g -Wall -Wextra -ansi -pedantic -std=c++17
USER_OPTIONS ?=

# Source files
SERVER_SRC = server.cpp

# Object files
SERVER_OBJ = server.o

# Executable
EXE = server

.PHONY: all clean run

all: $(EXE)

$(EXE): $(SERVER_OBJ)
	$(LD) $(CFLAGS) -o $@ $^

$(SERVER_OBJ): $(SERVER_SRC)
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(EXE)
	@echo "Run with: ./$(EXE) $(USER_OPTIONS)"
	./$(EXE) $(USER_OPTIONS)

clean:
	@echo "Cleaning files generated"
	rm -f $(SERVER_OBJ) $(EXE)
