#pragma once
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <future>
#include "thread_pool.h"

namespace asyncio 
{
    static std::future<std::string> read(const std::string& file_name) 
    {
        return syslib::pool.enqueue([](const std::string& file_name) 
        {
            int fd = open(file_name.c_str(), O_RDONLY);
            if (fd == -1) 
            {
                throw std::runtime_error("file not found");
            }

            struct stat file_stat;
            if (fstat(fd, &file_stat) == -1) 
            {
                close(fd);
                throw std::runtime_error("unable to get file size");
            }

            std::string content(file_stat.st_size, '\0');
            if (::read(fd, &content[0], file_stat.st_size) == -1) 
            {
                close(fd);
                throw std::runtime_error("read failed");
            }

            close(fd);
            return content;
        }, file_name);
    }

    static std::future<bool> write(const std::string& file_name, const std::string& content) 
    {
        return syslib::pool.enqueue([](const std::string& file_name, const std::string& content) 
        {
            int fd = open(file_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
        }, file_name, content);
    }
}
