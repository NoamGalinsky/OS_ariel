CC = gcc
FLAGS = -Wall -Wextra
FLAGSD = -Wall -g -Wextra
all: ex1 ex2 ex3 ex1_d ex2_d ex3_d
.PHONY: all clean
ex1: ex1.cpp
	$(CC) $(FLAGS) $^ -o $@
ex2: ex2.cpp
	$(CC) $(FLAGS) $^ -o $@
ex3: ex3.cpp
	$(CC) $(FLAGS) $^ -o $@
ex1_d: ex1.cpp
	$(CC) $(FLAGSD) $^ -o $@
ex2_d: ex2.cpp
	$(CC) $(FLAGSD) $^ -o $@
ex3_d: ex3.cpp
	$(CC) $(FLAGSD) $^ -o $@
clean:
	rm -f *.o ex1 ex2 ex3 ex1_d ex2_d ex3_d