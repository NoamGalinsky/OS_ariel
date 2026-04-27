CXX = g++
CXXFLAGS = -fPIC -Wall -Wextra -O2
LDFLAGS = -shared

LIB_TARGET = libmandelbrot.so
APP_TARGET = main_program

LIB_SRC = Mandelbrot.cpp
LIB_OBJ = $(LIB_SRC:.cpp=.o)

APP_SRC = main_program.cpp
APP_OBJ = $(APP_SRC:.cpp=.o)

all: $(LIB_TARGET) $(APP_TARGET)

# ===== Shared Library =====
$(LIB_TARGET): $(LIB_OBJ)
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp Mandelbrot.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ===== Main Program =====
$(APP_TARGET): $(APP_OBJ) $(LIB_TARGET)
	$(CXX) -o $@ $(APP_OBJ) -L. -lmandelbrot -Wl,-rpath=.
# ===== Clean =====
clean:
	rm -f *.o $(LIB_TARGET) $(APP_TARGET)
