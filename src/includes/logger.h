#pragma once
#include <atomic>
#include <cstdio>
#include <cstdarg>
#include <chrono>
#include <ctime>
#include <future>
#include <string>
#include "on_demand.h"
#include "queue.h"
#include "thread_pool.h"

#define MAX_LOG_MESSAGE_LENGTH 128
#define MESSAGE_QUEUE_SIZE 32
#define BUFFER_SIZE 4096

// Severity levels
enum class LogLevel : uint8_t
{ 
    INFO,
    WARNING,
    ERROR 
};

inline const char* to_string(LogLevel level) 
{
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

inline const char* get_current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    char* time_str = std::ctime(&current_time);
    time_str[strlen(time_str)-1] = '\0';
    return time_str;
}

// Macro to log with severity and formatted message
#define LOG(level, fmt, ...) \
    printf("%s [%s] " fmt "\n", get_current_timestamp(), to_string(level), ##__VA_ARGS__);

class async_logger
{
public:
    async_logger(): queue_(MESSAGE_QUEUE_SIZE), stop_(false)
    {
        join_ = syslib::pool.enqueue([this](){
            char message_buffer[BUFFER_SIZE];
            size_t offset = 0;
            std::string log_message;

            while (!stop_.load(std::memory_order_acquire) || queue_.dequeue(log_message))
            {
                do {
                    if (offset + log_message.size() >= BUFFER_SIZE)
                    {
                        break;
                    }
                    strcpy(message_buffer+offset, log_message.c_str());
                    offset += log_message.size();
                } while (queue_.dequeue(log_message));
                message_buffer[offset] = '\0';
                if (offset)
                {
                    printf("%s", message_buffer);
                    if (offset + log_message.size() >= BUFFER_SIZE)
                    {
                        printf("%s", log_message.c_str());
                    }
                }
                offset = 0;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }

    ~async_logger()
    {
        stop_.store(true, std::memory_order_release);
        join_.wait();
    }

    void log(LogLevel level, const char* fmt, ...)
    {
        const char* severity_level = to_string(level);

        std::string log_message;

        char format_string[MAX_LOG_MESSAGE_LENGTH];
        va_list args;
        va_start(args, fmt);
        int len = snprintf(format_string, MAX_LOG_MESSAGE_LENGTH, "%s [%s] ", get_current_timestamp(), severity_level);
        len += vsnprintf(format_string + len, MAX_LOG_MESSAGE_LENGTH - len, fmt, args);
        va_end(args);

        log_message.append(format_string);
        log_message.append("\n");

        while (!queue_.enqueue(std::move(log_message))) {}
    }

private:
    mpsc_queue<std::string> queue_;
    std::future<void> join_;
    std::atomic<bool> stop_;
};

static on_demand<async_logger> logger;

// Macro to log asychronously with severity and formatted message
#define ASYNCLOG(level, fmt, ...) \
    { \
        auto logger_ptr = logger.write(); \
        logger_ptr->log(level, fmt, ##__VA_ARGS__); \
    }