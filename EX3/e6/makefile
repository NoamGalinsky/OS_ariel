CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++17
TARGETS = client server

all: $(TARGETS)

server: server.cpp ../e5/reactor.cpp ../e5/reactor.hpp
	$(CXX) $(CXXFLAGS) -I../e5 -o server server.cpp ../e5/reactor.cpp -lpthread

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

clean:
	rm -f $(TARGETS)
