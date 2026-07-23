CXX = g++
CXXFLAGS = -Wall -Wextra -g -O0 --coverage
LDFLAGS = --coverage

TARGETS = drinks_bar atom_supplier molecule_requester

all: $(TARGETS)

drinks_bar: drinks_bar.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

atom_supplier: atom_supplier.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

molecule_requester: molecule_requester.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

coverage:
	gcov drinks_bar.cpp
	gcov atom_supplier.cpp
	gcov molecule_requester.cpp

clean:
	rm -f $(TARGETS)
	rm -f *.gcda *.gcno *.gcov
	rm -f inventory.txt
	rm -f /tmp/*.sock