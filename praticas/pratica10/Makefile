CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
OBJ = main.o hamiltoniano.o tps.o
ALVO = main
ifeq ($(OS),Windows_NT)
ALVO = main.exe
endif

.PHONY: all clean
all: $(ALVO)

$(ALVO): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

main.o tps.o: tps.h
$(OBJ): hamiltoniano.h

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) main main.exe
