#
# CS 11 C++ Advanced track: Makefile for lab 7
#

CXX = g++
CXXFLAGS = -std=c++20 -Wall -g

OBJS = test-treeset.o testbase.o

all: test-treeset

test-treeset: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

test: test-treeset
	./test-treeset

clean:
	rm -rf test-treeset *.o *~

.PHONY: all test clean
