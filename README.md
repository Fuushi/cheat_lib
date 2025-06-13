# CheatLib

A simple C++ library and tool for scanning and modifying memory in external Windows processes. Useful for game hacking, debugging, and educational purposes.

## Features
- Find a process by name and obtain a handle
- Scan process memory for specific integer values
- Refine search results based on new known values
- Batch write new values to found memory addresses

## Files
- `cLib.h` / `cLib.cpp`: The core library for process memory scanning and manipulation
- `main.cpp`: Example usage and command-line tool

## Usage

### Build
Compile with a C++ compiler on Windows. Example (using g++/MinGW):

```sh
# Compile
 g++ -o main.exe main.cpp cLib.cpp -lpsapi
```

### Run
```sh
main.exe <process_name.exe> <search_value>
```
- `<process_name.exe>`: Name of the target process (e.g., `notepad.exe`)
- `<search_value>`: Integer value to search for in the process memory

#### Example
```sh
main.exe notepad.exe 1234
```

1. The program scans the process for all addresses containing `1234`.
2. It prompts you to enter a new value (e.g., after changing a value in the target process).
3. It refines the search to addresses now containing the new value.
4. It prints the found addresses and writes `999` to all of them.

## Library API

### cLib Class
- `cLib(const char *name)`: Constructor. Finds process by name and opens a handle.
- `std::vector<void*> detect(int searchByte)`: Scan for addresses with `searchByte`.
- `std::vector<void*> detect(int searchByte, std::vector<void*> searchBuffer)`: Refine search in `searchBuffer`.
- `void batchWrite(int newByte, std::vector<void*> writeBuffer)`: Write `newByte` to all addresses in `writeBuffer`.

### Standalone Functions
- `DWORD findPidByName(const char *name)`
- `void readAddress(HANDLE, void*, int&)`
- `void writeAddress(HANDLE, void*, int)`
- `void scanPage(...)`, `scanPageRanges(...)`, `scanVirtualPages(...)`
- `HANDLE getProcess(DWORD pid)`

## Disclaimer
This code is for educational purposes only. Use responsibly and only on processes you have permission to access.

Documentation may be out of date.
