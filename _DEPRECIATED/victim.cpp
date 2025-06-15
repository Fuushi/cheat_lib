#include <iostream>

int main() {
    //declare target data (4 bytes)
    int data = 265;

    //display ref
    std::cout << &data << std::endl;

    //while true to keep alive
    while (true) {
        std::cout << data << std::flush;
        std::cin.get(); //saves power
    }

    return 0;

}
