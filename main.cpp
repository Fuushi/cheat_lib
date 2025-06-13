#include <Windows.h>
#include <TlHelp32.h> //for helper function
#include <iostream>
#include <vector>

#include "cLib.h"

//create lib interface
int main(int argc, char *argv[]) {

    //get args
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <process_name.exe>\n";
        return 1;
    }
    const char* processName = argv[1];
    const int searchByte = std::stoi(argv[2]);

    //ceate handle (clib obj)
    cLib handle(processName);

    //define search condition
    //from argv

    //search
    std::vector<void*> hits = handle.detect(searchByte);

    //display hits
    std::cout << "Addresses with hit values: " << hits.size() << std::endl;
    //for (auto addr : hits) {
    //    std::cout << addr << std::endl;
    //}

    //define second search condition
    int searchByte2 = -1;
    std::cout << "Waiting for new value: " << std::flush;
    std::cin >> searchByte2;

    //search again (refine)
    hits = handle.detect(searchByte2, hits);

    //display hits
    std::cout << "Addresses with hit values:\n";
    for (auto addr : hits) {
        std::cout << addr << std::endl;
    }

    //batch write
    int newValue = 999;
    handle.batchWrite(newValue, hits);

    return 1;
}
