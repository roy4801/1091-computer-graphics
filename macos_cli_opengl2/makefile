CC=g++
CFLAG=-std=c++17 $(shell pkg-config --cflags glfw3) -I../deps/glad/include
LIBS=$(shell pkg-config --libs glfw3)
INC=

all: main

main: main.cpp ../deps/glad/src/glad.c
	$(CC) $(CFLAG) $^ $(LIBS) -o $@

.PHONY: clean
clean:
	rm ./main