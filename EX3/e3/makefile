CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -g -std=c++17
TARGETS = convex_hull

all: $(TARGETS)

convex_hull: convex_hull.cpp
	$(CXX) $(CXXFLAGS) -o convex_hull convex_hull.cpp

clean:
	rm -f $(TARGETS)