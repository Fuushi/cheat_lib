#include <Windows.h>
#include <TlHelp32.h> //for helper function
#include <iostream>

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

int main() {

    //get pid using helper
    auto pid = findPidByName("victim.exe");

    if (pid) {
        std::cout << "Found the victim" << std::endl;
        
        //open process via handle
        HANDLE handleToProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

        //
        if (handleToProcess) {
            // readwrites in here

            //enter known addr in virtual space
            std::cout << "Enter memory addr:" << std::endl;
            void *address = 0;
            std::cin >> address;

            // create cache for data
            int data = 0;
            ReadProcessMemory(handleToProcess, address, &data, sizeof(data), nullptr);

            //dump read memory
            std::cout << data << std::endl;

            //write
            int newData = 69;
            if (WriteProcessMemory(handleToProcess, address, &newData, sizeof(newData), nullptr)) {
                std::cout << "Written New Data" << std::endl;
            }
            
            else {
                std::cout << "Write Operation Rejected" << std::endl;
            }
        }

        else {
            std::cout << "Process Rejected Opening" << std::endl;
        }

    }

    else {
        std::cout << "I could not find that shi chief" << std::endl;
    }

    std::cin.ignore();
    std::cin.get(); //keep app alive
    return 0;
}