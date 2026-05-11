CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++17

all: reactor.o

reactor.o: reactor.cpp reactor.hpp
	$(CXX) $(CXXFLAGS) -c reactor.cpp -o reactor.o

clean:
	rm -f reactor.o
