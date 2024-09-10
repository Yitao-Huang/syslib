#include "../src/includes/logger.h"
#include <vector>
#include <chrono>

#define NUM_LOGGING_THREADS 128
#define LSN 10000

void benchmark_sync()
{
    std::vector<std::future<void>> futs;

    for (int i = 0; i < NUM_LOGGING_THREADS; i++)
    {
        futs.emplace_back(syslib::pool.enqueue([i](){
            for (int log_number = LSN * i; log_number < LSN * (i+1); log_number++) {
                ASYNCLOG(LogLevel::INFO, "Log number #%d", log_number)
            }
        }));
    }

    // for (auto& fut : futs)
    // {
    //     fut.wait();
    // }
}

void benchmark_async()
{
    std::vector<std::future<void>> futs;

    for (int i = 0; i < NUM_LOGGING_THREADS; i++)
    {
        futs.emplace_back(syslib::pool.enqueue([i](){
            for (int log_number = LSN * i; log_number < LSN * (i+1); log_number++) {
                LOG(LogLevel::INFO, "Log number #%d", log_number)
            }
        }));
    }

    // for (auto& fut : futs)
    // {
    //     fut.wait();
    // }
}

int main(int argc, char* argv[])
{
    if (strcmp(argv[0], "sync") == 0)
    {
        benchmark_sync();
    }
    else 
    {
        benchmark_async();
    }

    return 0;
}