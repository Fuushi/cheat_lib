#include "memLib.h"
#include <cstring>
#include <vector>
#include <iostream>

void printBytes(const std::vector<char> &bytesArr) {
    std::cout << "Bytes: ";
    for (int i = 0; i < bytesArr.size(); i++) {
        std::cout << static_cast<int>(bytesArr[i]) << " ";
    }
    std::cout << std::flush;
}

std::vector<char> extractBytes(const void* addr, const int &span) {
    std::vector<char> bytes(span);
    std::memcpy(bytes.data(), addr, span);  // Copies span bytes from addr
    return bytes;
};