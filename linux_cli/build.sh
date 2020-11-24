#!/bin/bash

CC=g++
SRC=simple.cpp
TARGET=simple

CFLAGS="$(pkg-config --cflags glfw3) -I./glad/include"
LIBS="$(pkg-config --libs glfw3)"

${CC} ${CFLAGS} -o ${TARGET} ${SRC} ./glad/src/glad.c ${LIBS}
