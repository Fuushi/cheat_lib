#include <Windows.h>
#include <TlHelp32.h> //for helper function
#include <iostream>
#include <vector>

#include "memLib.h"
#include "cLib.h"

Bytes bytesHandler;

//create lib interface
int main(int argc, char *argv[]) {

    //constants
    const int data = 265;
    const std::vector<char> bytes = bytesHandler.byte(data);

    printBytes(bytes);

    // get pid from args
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <process_name.exe>\n";
        return 1;
    }
    const char* processName = argv[1];

    //establish handle
    cLib handle(processName);

    handle.validateHandles();

    std::vector<void*> addrs = handle.detect(bytes);

    //write below (indev)

}
