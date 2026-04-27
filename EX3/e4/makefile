CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -g
TARGETS = client server

all: $(TARGETS)

server: server.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp

client: client.cpp
	$(CXX) $(CXXFLAGS) -o client client.cpp

clean:
	rm -f $(TARGETS)