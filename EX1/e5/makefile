CC = g++
FLAGS = -Wall -Wextra -pg -O2 -std=c++17
all: MaximumSubArraySum_n MaximumSubArraySum_n2 MaximumSubArraySum_n3
.PHONY: all clean
MaximumSubArraySum_n: MaximumSubArraySum_n.cpp
	$(CC) $(FLAGS) $^ -o $@
MaximumSubArraySum_n2: MaximumSubArraySum_n2.cpp
	$(CC) $(FLAGS) $^ -o $@
MaximumSubArraySum_n3: MaximumSubArraySum_n3.cpp
	$(CC) $(FLAGS) $^ -o $@
clean:
	rm -f *.o MaximumSubArraySum_n MaximumSubArraySum_n2 MaximumSubArraySum_n3