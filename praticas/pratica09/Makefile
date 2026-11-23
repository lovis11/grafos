CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2
OBJ = main.o cobertura.o
ALVO = main
ifeq ($(OS),Windows_NT)
ALVO = main.exe
endif

.PHONY: all clean
all: $(ALVO)

$(ALVO): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

%.o: %.c cobertura.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) main main.exe
