CXX = g++
CXXFLAGS = -Wall -Wextra -O2
TARGETS = atom_warehouse atom_supplier

all: $(TARGETS)

atom_warehouse: atom_warehouse.cpp
	$(CXX) $(CXXFLAGS) -o atom_warehouse atom_warehouse.cpp

atom_supplier: atom_supplier.cpp
	$(CXX) $(CXXFLAGS) -o atom_supplier atom_supplier.cpp

clean:
	rm -f $(TARGETS)
