CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -g -pg -std=c++17
TARGETS = convex_hull_vector convex_hull_list

all: $(TARGETS)

convex_hull_vector: convex_hull_vector.cpp
	$(CXX) $(CXXFLAGS) -o convex_hull_vector convex_hull_vector.cpp

convex_hull_list: convex_hull_list.cpp
	$(CXX) $(CXXFLAGS) -o convex_hull_list convex_hull_list.cpp

clean:
	rm -f $(TARGETS)
