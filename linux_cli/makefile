CC=g++
CFLAG=-std=c++11 $(shell pkg-config --cflags glfw3) -I./glad/include
LIBS=$(shell pkg-config --libs glfw3)
INC=

all: main

main: main.cpp glad/src/glad.c
	$(CC) $(CFLAG) $^ $(LIBS) -o $@

.PHONY: clean
clean:
	rm ./main