CC=g++
CFLAGS=-std=c++0x -O0 -Wall -pg
#CC=clang++
#CFLAGS=-std=c++11 -O3 -Wall
HINCLUDE=rand.hpp skiplist.hpp

all:test_skiplist_interface test_performance


test_skiplist_interface:driver_test.cpp $(HINCLUDE)
	$(CC) $(CFLAGS) $(INCLUDE) driver_test.cpp -o test_skiplist_interface

test_performance:speed_test.cpp $(HINCLUDE)
	$(CC) $(CFLAGS) $(INCLUDE) speed_test.cpp -o test_performance

clean:
	rm -rf test_skiplist_interface test_performance
