#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "includes/asyncio.h"

namespace syslib 
{
    /*
        global thread pool, destruct when main exits
    */
    thread_pool pool;
}

namespace asyncio 
{
    std::future<std::string> read(const std::string& fileName) 
    {
        return syslib::pool.enqueue([](const std::string& fileName) 
        {
            int fd = open(fileName.c_str(), O_RDONLY);
            if (fd == -1) 
            {
                throw std::runtime_error("file not found");
            }

            struct stat fileStat;
            if (fstat(fd, &fileStat) == -1) 
            {
                close(fd);
                throw std::runtime_error("unable to get file size");
            }

            std::string content(fileStat.st_size, '\0');
            if (::read(fd, &content[0], fileStat.st_size) == -1) 
            {
                close(fd);
                throw std::runtime_error("read failed");
            }

            close(fd);
            return content;
        }, fileName);
    }

    std::future<bool> write(const std::string& fileName, const std::string& content) 
    {
        return syslib::pool.enqueue([](const std::string& fileName, const std::string& content) 
        {
            int fd = open(fileName.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                throw std::runtime_error("unable to open file for writing");
            }

            if (::write(fd, content.c_str(), content.size()) == -1)
            {
                close(fd);
                throw std::runtime_error("write failed");
            }

            close(fd);
            return true;
        }, fileName, content);
    }
}
