CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++17
TARGETS = client server

all: $(TARGETS)

server: server.cpp ../e8/proactor.cpp ../e8/proactor.hpp
	$(CXX) $(CXXFLAGS) -I../e8 -o server server.cpp ../e8/proactor.cpp -lpthread

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

clean:
	rm -f $(TARGETS)
