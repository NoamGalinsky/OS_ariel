CXX = g++
CXXFLAGS = -Wall -Wextra -O2
TARGETS = molecule_requester molecule_supplier atom_supplier

all: $(TARGETS)

molecule_requester: molecule_requester.cpp
	$(CXX) $(CXXFLAGS) -o molecule_requester molecule_requester.cpp

molecule_supplier: molecule_supplier.cpp
	$(CXX) $(CXXFLAGS) -o molecule_supplier molecule_supplier.cpp

atom_supplier: atom_supplier.cpp
	$(CXX) $(CXXFLAGS) -o atom_supplier atom_supplier.cpp

clean:
	rm -f $(TARGETS)
