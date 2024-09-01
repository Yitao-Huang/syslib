#include <iostream>

// Severity levels
enum class LogLevel { INFO, WARNING, ERROR };

// Macro to log with severity, timestamp, and formatted message
#define LOG(level, fmt, ...) \
    { \
        std::string severity_str; \
        if (level == LogLevel::INFO) severity_str = "INFO"; \
        else if (level == LogLevel::WARNING) severity_str = "WARNING"; \
        else if (level == LogLevel::ERROR) severity_str = "ERROR"; \
        std::cout << "[" << severity_str << "] " \
                  << std::format(fmt, __VA_ARGS__) << std::endl; \
    }
