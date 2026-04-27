CC = g++
FLAGS = -Wall -g -Wextra -fprofile-arcs -ftest-coverage -std=c++17
all: dijkstra test
.PHONY: all clean
dijkstra: dijkstra.cpp
	$(CC) $(FLAGS) $^ -o $@
test: test.cpp
	$(CC) $(FLAGS) $^ -o $@
clean:
	rm -f *.o dijkstra test *.gcda *.gcno