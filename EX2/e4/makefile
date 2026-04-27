CXX = g++
CXXFLAGS = -Wall -Wextra -O2
TARGETS = molecule_requester drinks_bar atom_supplier

all: $(TARGETS)

molecule_requester: molecule_requester.cpp
	$(CXX) $(CXXFLAGS) -o molecule_requester molecule_requester.cpp

drinks_bar: drinks_bar.cpp
	$(CXX) $(CXXFLAGS) -o drinks_bar drinks_bar.cpp

atom_supplier: atom_supplier.cpp
	$(CXX) $(CXXFLAGS) -o atom_supplier atom_supplier.cpp

clean:
	rm -f $(TARGETS)
