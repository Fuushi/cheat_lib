#ifndef CLIB_H
#define CLIB_H

#include "memLib.h"
#include <vector>

DWORD findPidByName(const char *name);

void* nextPhysicalAddress(const void* addr);

void readAddress(const HANDLE handleToProcess, const void* memAddr, char &data);

void writeAddress(HANDLE handleToProcess, void* memAddr, char data);

void scanPage(
    const HANDLE handleToProcess, 
    void *pageAddress, 
    SIZE_T pageSize, 
    char searchByte, 
    std::vector<void*> &returnBuffer,
    bool verbose = false
);

void scanPageRanges(
    HANDLE handleToProcess, 
    void *baseAddress, 
    int pagesCount, 
    SIZE_T pageSize, 
    char searchByte, 
    std::vector<void*> &returnBuffer
);

void scanVirtualPages(
    HANDLE handleToProcess, 
    char searchByte, 
    std::vector<void*> &returnBuffer
);

HANDLE getProcess(DWORD pid);

class cLib {

    public:
    HANDLE hProcess = nullptr;

    std::vector<void*> detect(char searchByte) {
        //wraps essentially what is currently in main
        //valuidate pointers
        validateHandles();

        std::vector<void*> returnBuffer;
        scanVirtualPages(
            hProcess,
            searchByte,
            returnBuffer
        );

        return returnBuffer;
    }

    std::vector<void*> detect(char searchByte, std::vector<void*> searchBuffer) {
        //iterate through each addr in searchBuffer, readAddr, and compare its value to searchByte
        //given address function overflow
        std::vector<void*> returnBuffer;

        char data = 0;
        for (void* addr: searchBuffer) {
            readAddress(
                hProcess,
                addr,
                data
            );

            if (data == searchByte) {
                returnBuffer.push_back(addr);
            }
        }

        return returnBuffer;
    }

    std::vector<void*> detect(const std::vector<char> searchBytes) {
        //search for n bytes in memory

        //initialize buffer
        std::vector<void*> searchBuffer = {};
        std::vector<void*> hits = {};

        //start by finding all instances of first byte
        scanVirtualPages(
            hProcess,
            searchBytes[0],
            searchBuffer
        );

        std::cout << "Searching for secondary bytes" << std::endl;

        //caches
        char data = 0;
        int byteHits = 0;

        //scan for successive bytes
        for (int p = 0; p < searchBuffer.size(); p++) {
            //each pointer

            //set counters
            byteHits = 0;

            //copy referencce pointer into scope
            void* pointer = searchBuffer[p]; //this will be iterated

            for (int b = 1; b < searchBytes.size(); b++) {
                //each byte for each pointer

                //iterate pointer
                pointer = nextPhysicalAddress(pointer);

                //load value at addr
                readAddress(
                    hProcess,
                    pointer,
                    data
                );

                //check if data is expected byte
                if (data == searchBytes[b]) {
                    //good contition
                    byteHits += 1;
                }

            }

            if (byteHits == (searchBytes.size()-1)) {
                std::cout << "Hits detected @ >> " << searchBuffer[p] << std::endl;
                hits.push_back(searchBuffer[p]);
            }
        }

        return searchBuffer;
    }
    //search refinement buffer not needed for more complex datatypes

    void batchWrite(char newByte, std::vector<void*> writeBuffer) {
        for (void* addr : writeBuffer) {
            writeAddress(
                hProcess,
                addr,
                newByte
            );
        }
    }

    void validateHandles() {
        if (hProcess == nullptr) {std::cout << "Null Handle" << std::endl;}
    }

    cLib(const char *name) {
        DWORD pid = findPidByName(name);
        
        if (pid) {
            hProcess = getProcess(pid);
        } else {
            std::cout << "Failed to find Process" << std::endl;
        }
        // Optionally store hProcess or pid as member variables if needed
    }

};

#endif