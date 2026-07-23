# Operating Systems Assignment 2 - Code Coverage Report
 
## Overview
 
This report presents the code coverage results for the final implementation of the project (Stage 6). Since Stage 6 integrates all functionality from the previous stages, the coverage analysis was performed only on the final version of the system.
 
Coverage measurement was performed using GCC's built-in coverage support (`--coverage`) and the `gcov` tool. The project was compiled with coverage instrumentation enabled, executed using multiple test scenarios, and then analyzed using `gcov`.
 
---
 
## Build Configuration
 
The project was compiled with code coverage support enabled:
 
```bash
g++ --coverage
```
 
Coverage reports were generated using:
 
```bash
gcov drinks_bar.cpp
gcov atom_supplier.cpp
gcov molecule_requester.cpp
```
 
---
 
## Test Environment
 
The following executables were tested:
 
- `drinks_bar` (Final server implementation)
- `atom_supplier` (TCP / UDS Stream client)
- `molecule_requester` (UDP / UDS Datagram client)
 
The tests covered communication through both Internet sockets and Unix Domain Sockets, functionality inherited from all previous stages, inventory management, molecule delivery, keyboard commands, and persistent storage.
 
---
 
## Executed Test Cases
 
### TCP Functionality
 
The server was started using TCP and UDP communication channels.
 
Valid commands were sent through `atom_supplier`:
 
```text
ADD CARBON 100
ADD OXYGEN 50
ADD HYDROGEN 200
```
 
Invalid commands were also tested:
 
```text
ADD TEST 5
ADD CARBON ABC
```
 
These tests verified:
 
- TCP connection establishment
- Command parsing
- Resource updates
- Error handling
- Client disconnection handling
 
---
 
### UDP Functionality
 
The following molecule requests were sent using `molecule_requester`:
 
```text
DELIVER WATER 10
DELIVER CARBON DIOXIDE 10
DELIVER ALCOHOL 10
DELIVER GLUCOSE 5
```
 
Additional invalid and edge-case requests were tested:
 
```text
DELIVER TEST 10
DELIVER WATER 999999
```
 
These tests verified:
 
- UDP communication
- Molecule generation logic
- Atom consumption
- Insufficient resource handling
- Invalid command handling
 
---
 
### Keyboard Commands
 
The following server-side commands were executed directly from the server console:
 
```text
print
GEN SOFT DRINK
GEN VODKA
GEN CHAMPAGNE
RANDOM COMMAND
```
 
These tests verified:
 
- Inventory display
- Drink calculation logic
- Console command processing
- Error handling for unsupported commands
 
---
 
### Unix Domain Socket Testing
 
Both Unix Domain Socket modes were tested.
 
#### Stream Mode
 
Using:
 
```bash
./atom_supplier -f <stream_socket_path>
```
 
Commands tested:
 
```text
ADD CARBON 10
ADD OXYGEN 20
ADD HYDROGEN 30
```
 
#### Datagram Mode
 
Using:
 
```bash
./molecule_requester -f <datagram_socket_path>
```
 
Commands tested:
 
```text
DELIVER WATER 1
DELIVER ALCOHOL 1
DELIVER GLUCOSE 1
```
 
These tests verified:
 
- UDS Stream communication
- UDS Datagram communication
- Correct handling of local socket connections
 
---
 
### Persistent Storage Testing
 
The server was executed with:
 
```bash
-f inventory.txt
```
 
The following behaviors were verified:
 
- Automatic file creation
- Saving inventory updates
- Loading inventory from file
- File synchronization after resource modifications
 
---
 
## Coverage Results
 
|      Source File       | Coverage |
|------------------------|----------|
| drinks_bar.cpp         |  85.41%  |
| atom_supplier.cpp      |  73.97%  |
| molecule_requester.cpp |  76.47%  |
 
Overall coverage reported by gcov:
 
---
 
## Analysis
 
The testing process covered the majority of the application's functionality, including:
 
- TCP communication
- UDP communication
- Unix Domain Stream sockets
- Unix Domain Datagram sockets
- Inventory updates
- Molecule production
- Drink calculations
- File persistence
- Client connection handling
- Invalid input processing
- Resource limit validation
 
The final server implementation (`drinks_bar.cpp`) achieved over 85% line coverage, demonstrating that most execution paths were exercised during testing.
 
---
 
## Conclusion
 
The code coverage results indicate that the major components and execution paths of the final Stage 6 implementation were successfully tested.
 
Final coverage results:
 
- **drinks_bar.cpp:** 85.41%
- **atom_supplier.cpp:** 73.97%
- **molecule_requester.cpp:** 76.47%
 
These results provide evidence that the integrated system, including all functionality from Stages 1 through 6, was thoroughly exercised and validated through practical testing.
