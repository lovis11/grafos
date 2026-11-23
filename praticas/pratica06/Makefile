CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJ = main.o dijkstra.o

.PHONY: all clean
all: main

main: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

%.o: %.c dijkstra.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) main main.exe
