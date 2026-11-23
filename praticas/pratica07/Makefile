CC = gcc
CFLAGS = -Wall -Wextra -std=c11
OBJ = main.o bellman_ford.o
ALVO = main
ifeq ($(OS),Windows_NT)
ALVO = main.exe
endif

.PHONY: all clean
all: $(ALVO)

$(ALVO): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

%.o: %.c bellman_ford.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) main main.exe
