#include <Windows.h>
#include <TlHelp32.h> //for helper function
#include <iostream>
#include <vector>

std::vector<void*> hits;


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

void readAddress(HANDLE handleToProcess, void* memAddr, int &data) {
    ReadProcessMemory(handleToProcess, memAddr, &data, sizeof(data), nullptr);
    return;
}

void writeAddress(HANDLE handleToProcess, void* memAddr, int data) {

    if (WriteProcessMemory(handleToProcess, memAddr, &data, sizeof(data), nullptr)) {
        std::cout << "Written New Data: " << data << std::endl;
    }
    
    else {
        std::cout << "Write Operation Rejected" << std::endl;
    }

    return;
}

void scanPage(HANDLE handleToProcess, void *pageAddress, SIZE_T pageSize, int searchByte = -1) {
    //this scans the page for all addresses contained
    //page given ie 0x000000f000
    //this function searches memory addresses within the page //page size is typically 4096

    //loop through ALL addresses in page
    for (int i = 0; i < pageSize; i++) {

        //add offset to page Address
        void* address = (char*)pageAddress + i;

        //read data -> cache
        int data = -1;
        readAddress(handleToProcess, address, data);

        //detect hits
        if (data == searchByte) {
            //add hit to global vector
            hits.push_back(address);

            //print to terminal hit addr
            std::cout << "Hit: " << std::flush;
            std::cout << address << std::endl;

        }

    }

}

void scanPageRanges(HANDLE handleToProcess, void *baseAddress, int pagesCount, SIZE_T pageSize, int searchByte = -1) {
    //this gets all pages, in a range, then calls another function to search the page itself
    for (int i = 0; i < pagesCount; i++) {

        //generate Page address
        void* currentPage = (char*)baseAddress + i * pageSize;

        //print page address to terminal
        //std::cout << "Scanning page: " << currentPage << std::endl;

        //call span page function to scan the page
        scanPage(handleToProcess, currentPage, pageSize, searchByte=searchByte);

    }
}

void scanVirtualPages(HANDLE handleToProcess, int searchByte = -1) {
    //takes handle as arg, and scanes virtual mem space

    //get the page size info
    SYSTEM_INFO sSysInfo = {};
    GetSystemInfo(&sSysInfo);

    //create mem table
    MEMORY_BASIC_INFORMATION memInfo;

    //index
    void *currentScanAddress = 0;

        while (true) {
            //load bytes?
            SIZE_T readBytes = 
                VirtualQueryEx(handleToProcess, currentScanAddress, &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

            //detect end of pages
            if (!readBytes) {
                return;
            }

            currentScanAddress = (char *)memInfo.BaseAddress + memInfo.RegionSize;

            //observe if memory is commited (set by application)
            if (memInfo.State == MEM_COMMIT) {
                //note more flag ie PAGE_READONLY & PAGE_EXECUTE_READWRTIE

                if (memInfo.Protect == PAGE_READWRITE) {
                    //std::cout << "Found READWRITE page(s) at base address: " << memInfo.BaseAddress << " Size: "
                    //<< memInfo.RegionSize << " = pages count: " << memInfo.RegionSize / sSysInfo.dwPageSize << "\n";

                    //further scanning of each table in here
                    int pagesCount = memInfo.RegionSize / sSysInfo.dwPageSize;
                    scanPageRanges(handleToProcess, memInfo.BaseAddress, pagesCount, sSysInfo.dwPageSize, searchByte=searchByte);
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