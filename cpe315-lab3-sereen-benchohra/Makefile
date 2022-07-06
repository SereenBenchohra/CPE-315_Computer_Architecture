CXX = g++
CC = gcc
CXXFLAGS = -ggdb -Wall -O2
CXXSRCS = matmul-driver.cpp 
ASRCS = matmul.s intmul.s intadd.s 
BIN = mm

all:
	$(CXX) $(CXXFLAGS) -c $(CXXSRCS)
	$(CC) $(CXXFLAGS) -c $(ASRCS)
	$(CC) $(CXXFLAGS) -o $(BIN) *.o

clean:
	rm -f *.o $(BIN)
