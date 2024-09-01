#include <iostream>
#include "includes/asyncio.h"

int main() {
    try {
        auto read_res = asyncio::read("../tests/example.txt");
        auto write_res = asyncio::write("../tests/output.txt", "Hello, IO world!");

        std::cout << "Read content: " << read_res.get() << std::endl;
        write_res.get();
        std::cout << "Write Done" << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "I/O operation failed: " << ex.what() << std::endl;
    }

    return 0;
}