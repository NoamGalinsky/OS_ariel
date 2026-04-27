CC = g++
CFLAGS = -Wall
DEBUGFLAGS = -g

SRC_1 = Mandelbrot.cpp

BIN_1 = Mandelbrot


all: $(BIN_1)

$(BIN_1): $(SRC_1)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -o $@ $^

clean:
	rm -f $(BIN_1)
