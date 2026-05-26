CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++17
TARGETS = client server

all: $(TARGETS)

server: server.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp -lpthread

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

clean:
	rm -f $(TARGETS)
