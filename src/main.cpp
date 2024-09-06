#include <iostream>
#include "includes/asyncio.h"
#include "includes/logger.h"

int main() {
    try 
    {
        auto read_res = asyncio::read("../tests/example.txt");
        auto write_res = asyncio::write("../tests/output.txt", "Hello, IO world!");
        LOG(LogLevel::INFO, "Read content: %s", read_res.get().c_str());
        write_res.get();
        LOG(LogLevel::INFO, "Write done");
    } 
    catch (const std::exception& ex) 
    {
        LOG(LogLevel::ERROR, "I/O operation failed: %s", ex.what());
    }

    return 0;
}