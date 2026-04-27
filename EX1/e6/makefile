CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17

# Executables
SENDER = signal_sender
RECEIVER = signal_receiver

# Source files
SENDER_SRC = signal_sender.cpp
RECEIVER_SRC = signal_receiver.cpp

# Default: build both
all: $(SENDER) $(RECEIVER)

# Build sender
$(SENDER): $(SENDER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Build receiver
$(RECEIVER): $(RECEIVER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Clean executables
clean:
	rm -f $(SENDER) $(RECEIVER)
