# Operating Systems Assignment 2
 
## Code Coverage Report
 
### Overview
 
This project was compiled with GCC code coverage support enabled using the `--coverage` flag. Coverage analysis was performed using the `gcov` tool after executing multiple test scenarios on both the server and client applications.
 
### Build Configuration
 
The applications were compiled with coverage instrumentation enabled:
 
```bash
g++ --coverage
```
 
Coverage reports were generated using:
 
```bash
gcov atom_warehouse.cpp
gcov atom_supplier.cpp
```
 
### Test Scenarios
 
The following functionality was tested:
 
#### atom_warehouse (Server)
 
- Server startup and initialization.
- TCP client connections.
- Processing valid commands:
- `ADD CARBON <number>`
- `ADD OXYGEN <number>`
- `ADD HYDROGEN <number>`
- Inventory updates.
- Invalid command handling.
- Client disconnection handling.
- Multiple command exchanges during a single connection.
- Server shutdown after testing.
 
#### atom_supplier (Client)
 
- Connection to the server.
- Transmission of valid atom supply commands.
- User input processing.
- Communication with the server.
- Client termination and disconnection.
 
### Coverage Results
 
|    Source File     | Executable Lines Covered |
|--------------------|--------------------------|
| atom_warehouse.cpp |           82.47%         |
| atom_supplier.cpp  |           70.73%         |
 
### Analysis
 
The test execution covered most of the core functionality of both applications.
 
For the server application (`atom_warehouse.cpp`), the majority of command processing, connection management, and inventory update logic was exercised during testing.
 
For the client application (`atom_supplier.cpp`), the main communication flow and command submission logic were covered, resulting in over 70% line coverage.
 
### Conclusion
 
The coverage results indicate that the primary features of the system were successfully tested.
 
Final code coverage:
 
- **atom_warehouse.cpp:** 82.47%
- **atom_supplier.cpp:** 70.73%
 
These results demonstrate that the main execution paths, command handling routines, and client-server interactions were exercised during testing.