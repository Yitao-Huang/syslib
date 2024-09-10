#include "includes/asyncio.h"
#include "includes/logger.h"

int main() {
    try 
    {
        auto read_res = asyncio::read("../tests/example.txt");
        auto write_res = asyncio::write("../tests/output.txt", "Hello, IO world!");
        LOG(LogLevel::INFO, "Read content: %s", read_res.get().c_str())
        write_res.get();
        LOG(LogLevel::INFO, "Write done")
    } 
    catch (const std::exception& ex) 
    {
        LOG(LogLevel::ERROR, "I/O operation failed: %s", ex.what())
    }

    auto res1 = syslib::pool.enqueue([](){
        for (size_t i = 0; i < 10; i++) {
            ASYNCLOG(LogLevel::INFO, "Log number #%d", i)
        }
    });

    auto res2 = syslib::pool.enqueue([](){
        for (size_t i = 10; i < 20; i++) {
            ASYNCLOG(LogLevel::INFO, "Log number #%d", i)
        }
    });

    res1.wait();
    res2.wait();

    return 0;
}