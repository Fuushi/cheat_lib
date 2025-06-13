#ifndef CLIB_H
#define CLIB_H

#include <vector>

DWORD findPidByName(const char *name);

void readAddress(HANDLE handleToProcess, void* memAddr, int &data);

void writeAddress(HANDLE handleToProcess, void* memAddr, int data);

void scanPage(
    HANDLE handleToProcess, 
    void *pageAddress, 
    SIZE_T pageSize, 
    int searchByte, 
    std::vector<void*> &returnBuffer
);

void scanPageRanges(
    HANDLE handleToProcess, 
    void *baseAddress, 
    int pagesCount, 
    SIZE_T pageSize, 
    int searchByte, 
    std::vector<void*> &returnBuffer
);

void scanVirtualPages(
    HANDLE handleToProcess, 
    int searchByte, 
    std::vector<void*> &returnBuffer
);

HANDLE getProcess(DWORD pid);

class cLib {

    public:
    HANDLE hProcess = nullptr;

    std::vector<void*> detect(int searchByte) {
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

    std::vector<void*> detect(int searchByte, std::vector<void*> searchBuffer) {
        //iterate through each addr in searchBuffer, readAddr, and compare its value to searchByte
        //given address function overflow
        std::vector<void*> returnBuffer;

        int data = 0;
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

    void batchWrite(int newByte, std::vector<void*> writeBuffer) {
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