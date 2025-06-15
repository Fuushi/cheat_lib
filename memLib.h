#ifndef MEM_LIB_H
#define MEM_LIB_H

#include <vector>
#include <cstring>
//memory helped library

//byte handler functions
void printBytes(const std::vector<char> &bytesArr);

std::vector<char> extractBytes(const void* addr, const int &span);


//type wrappers
class Bytes {
    //all bytes converters return vector<char>

    public: //

    //integer overflow
    std::vector<char> byte(const int &data) {
        return extractBytes((void*)&data, sizeof(data));
    };
    
};

#endif