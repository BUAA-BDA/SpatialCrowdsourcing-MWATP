##
#	Makefile for `DASFAA'18` application
##
CC = gcc
CXX = g++
LIBS = -lrt
CFLAGS = -W -Wall -Wno-sign-compare -O2 -s -pipe -mmmx -msse -msse2 -msse3
TARGET = ./all

all: ignore replan

global.o: global.h global.cpp
	$(CXX) $(CFLAGS) -c global.cpp $(LIBS)

input.o: input.h input.cpp
	$(CXX) $(CFLAGS) -c input.cpp $(LIBS)

ignore: input.h input.cpp output.h global.h global.cpp ignore.cpp input.o global.o monitor.h
	$(CXX) $(CFLAGS) -o ignore input.o global.o ignore.cpp $(LIBS) -D USE_MEM

replan: input.h input.cpp output.h global.h global.cpp replan.cpp input.o global.o monitor.h
	$(CXX) $(CFLAGS) -o replan input.o global.o replan.cpp $(LIBS) -D USE_MEM

.PHONY: clean
clean:
		-@rm *.o *.gcno *~ 2> /dev/null || true
		-@rm ignore replan 2> /dev/null || true

		