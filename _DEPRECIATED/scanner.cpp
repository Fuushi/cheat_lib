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

    //ceate handle (clib obj)
    cLib handle(processName);

    //define search condition
    int searchByte = 69;

    //search
    std::vector<void*> hits = handle.detect(searchByte);

    //display hits
    std::cout << "Addresses with hit values:\n";
    for (auto addr : hits) {
        std::cout << addr << std::endl;
    }

    //define second search condition
    int searchByte2 = 73;

    //search again (refine)
    std::vector<void*> hits = handle.detect(searchByte2, hits);

    //display hits
    std::cout << "Addresses with hit values:\n";
    for (auto addr : hits) {
        std::cout << addr << std::endl;
    }

    //batch write

    return 1;
}

/*
int main(int argc, char *argv[]) {
    //define args

    //PID
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <process_name.exe>\n";
        return 1;
    }
    const char* processName = argv[1];

    //search data 
    int searchByte = 52;

    //desired data
    int newData = 69;
    int newData2 = 0; //just a cache for cin >>

    //get pid from name
    auto pid = findPidByName(processName);

    //scan addr -> global vector hits
    HANDLE handleToProcess = getProcess(pid);
    if (handleToProcess == nullptr) {return 1;}

    

    //scan pages
    scanVirtualPages(handleToProcess, searchByte);


    //display hits vector
    std::cout << "Addresses with hit values:\n";
    for (auto addr : hits) {
        std::cout << addr << std::endl;
    }

    //copy hits
    const std::vector<void*> hitsCopy = hits;

    std::cout << "Waiting for change..." << std::flush;
    std::cin >> newData2;

    //clear hits
    hits = {};

    //search known hits again for new value
    int data = 0;
    for (auto addr: hitsCopy) {
        readAddress(handleToProcess, addr, data);

        if (data == newData2) {
            hits.push_back(addr);

            std::cout << "Hit: " << std::flush;
            std::cout << addr << std::endl;
            
        }
    }

    //display hits vector
    std::cout << "Addresses with hit values:\n";
    for (auto addr : hits) {
        std::cout << addr << std::endl;
    }

    std::cout << "Write? ..." << std::flush;
    std::cin.get();

    //write new values to addresses
    for (auto addr : hits) {
        writeAddress(handleToProcess, addr, newData);
    }

    std::cout << "Press enter to exit: " << std::flush;
	std::cin.get();
	return 0;
}
*/