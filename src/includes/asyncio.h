#include <string>
#include <future>
#include "thread_pool.h"

namespace asyncio
{
    std::future<std::string> read(const std::string& fileName);
    std::future<bool> write(const std::string& fileName, const std::string& content);
}
