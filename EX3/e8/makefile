CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++17

all: proactor.o

proactor.o: proactor.cpp proactor.hpp
	$(CXX) $(CXXFLAGS) -c proactor.cpp -o proactor.o

clean:
	rm -f proactor.o
