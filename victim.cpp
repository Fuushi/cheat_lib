#include <iostream>

int main() {
    //declare target data
    int data = 123456;

    //display ref
    std::cout << &data << std::endl;

    std::cin.get(); //wait condition

    //while true to keep alive
    while (true) {
        std::cout << data << std::endl;
    }

    return 0;

}
