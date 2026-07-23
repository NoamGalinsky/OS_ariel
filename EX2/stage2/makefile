CXX = g++

CXXFLAGS = -Wall -Wextra -O0 --coverage

LDFLAGS = --coverage

TARGETS = molecule_requester molecule_supplier atom_supplier

all: $(TARGETS)

molecule_requester: molecule_requester.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

molecule_supplier: molecule_supplier.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

atom_supplier: atom_supplier.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

coverage:
	gcov molecule_requester.cpp
	gcov molecule_supplier.cpp
	gcov atom_supplier.cpp

clean:
	rm -f $(TARGETS) *.gcno *.gcda *.gcov