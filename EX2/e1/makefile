CXX = g++

CXXFLAGS = -Wall -Wextra -O0 --coverage

LDFLAGS = --coverage

TARGETS = atom_warehouse atom_supplier

all: $(TARGETS)

atom_warehouse: atom_warehouse.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

atom_supplier: atom_supplier.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

coverage:
	gcov atom_warehouse.cpp
	gcov atom_supplier.cpp

clean:
	rm -f $(TARGETS) *.gcno *.gcda *.gcov