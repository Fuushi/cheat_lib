#include <Windows.h>
#include <TlHelp32.h> //for helper function
#include <iostream>
#include <vector>
#include "cLib.h"


// Increment the address by 1 byte
void* nextPhysicalAddress(const void* addr) {
    return (char*)addr + 1;
}

DWORD findPidByName(const char *name) {

    //declare handles
    HANDLE h;
    PROCESSENTRY32 singleProcess;
    
    //get handle
    h = CreateToolhelp32Snapshot(
        TH32CS_SNAPPROCESS,
        0
    );

    singleProcess.dwSize = sizeof(PROCESSENTRY32);

    do
	{
		if (strcmp(singleProcess.szExeFile, name) == 0)
		{
			DWORD pid = singleProcess.th32ProcessID;
			CloseHandle(h);
			return pid;
		}

	} while (Process32Next(h, &singleProcess));

	CloseHandle(h);

	return 0;
}

void readAddress(const HANDLE handleToProcess, const void* memAddr, char &data) {
    ReadProcessMemory(handleToProcess, memAddr, &data, sizeof(data), nullptr);
    return;
}

void writeAddress(HANDLE handleToProcess, void* memAddr, char data) {

    if (WriteProcessMemory(handleToProcess, memAddr, &data, sizeof(data), nullptr)) {
        std::cout << "Written New Data: " << data << std::endl;
    }
    
    else {
        std::cout << "Write Operation Rejected" << std::endl;
    }

    return;
}

void scanPage(
    const HANDLE handleToProcess, 
    void *pageAddress, 
    SIZE_T pageSize, 
    char searchByte, 
    std::vector<void*> &returnBuffer, 
    bool verbose
) {
    //this scans the page for all addresses contained
    //page given ie 0x000000f000
    //this function searches memory addresses within the page //page size is typically 4096

    //loop through ALL addresses in page
    for (int i = 0; i < pageSize; i++) {

        //add offset to page Address
        void* address = (char*)pageAddress + i;

        //read data -> cache
        char data = 1;
        readAddress(handleToProcess, address, data);

        //detect hits
        if (data == searchByte) {
            //add hit to global vector
            returnBuffer.push_back(address);

            //print to terminal hit addr if verbose
            if (verbose) {
                std::cout << "Hit: " << std::flush;
                std::cout << address << std::endl;
            }

        }

    }

}

void scanPageRanges(HANDLE handleToProcess, void *baseAddress, int pagesCount, SIZE_T pageSize, char searchByte, std::vector<void*> &returnBuffer) {
    //this gets all pages, in a range, then calls another function to search the page itself
    for (int i = 0; i < pagesCount; i++) {

        //generate Page address
        void* currentPage = (char*)baseAddress + i * pageSize;

        //call span page function to scan the page
        scanPage(handleToProcess, currentPage, pageSize, searchByte, returnBuffer);
    }
}

void scanVirtualPages(HANDLE handleToProcess, char searchByte, std::vector<void*> &returnBuffer) {
    //takes handle as arg, and scans virtual memory for process

    //get the page size info
    SYSTEM_INFO sSysInfo = {};
    GetSystemInfo(&sSysInfo);

    //create mem table
    MEMORY_BASIC_INFORMATION memInfo;

    //index
    void *currentScanAddress = 0;

    while (true) {
        //load bytes wize with virtual query
        SIZE_T readBytes = VirtualQueryEx(handleToProcess, currentScanAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

        //detect end of pages
        if (!readBytes) {
            return;
        }

        currentScanAddress = (char *)memInfo.BaseAddress + memInfo.RegionSize;

        //observe if memory is commited (set by application)
        if (memInfo.State == MEM_COMMIT) {
            //note more flag ie PAGE_READONLY & PAGE_EXECUTE_READWRTIE

            if (memInfo.Protect == PAGE_READWRITE) {
                //inspect each page via sub function
                int pagesCount = memInfo.RegionSize / sSysInfo.dwPageSize;
                scanPageRanges(handleToProcess, memInfo.BaseAddress, pagesCount, sSysInfo.dwPageSize, searchByte, returnBuffer);
            }
        }
    }

    return;
}

HANDLE getProcess(DWORD pid) {
    if (pid)
    {
        std::cout << "Found the victim process!\n";

        HANDLE handleToProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

        if (handleToProcess)
        {
            return handleToProcess;
        }
        else
        {
            std::cout << "Failed to open process.\n";
        }

    }
    else
    {
        std::cout << "couldn't find victim process :/\n";
    }
    //the program will inevitable crash if it gets here
    return nullptr;
}


