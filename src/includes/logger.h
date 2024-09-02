#include <cstdio>
#include <cstdlib>
#include <cstring>

// Severity levels
enum class LogLevel { INFO, WARNING, ERROR };

// Macro to log with severity, timestamp, and formatted message
#define LOG(level, fmt, ...) \
    { \
        const char* severity_str; \
        if (level == LogLevel::INFO) severity_str = "INFO"; \
        else if (level == LogLevel::WARNING) severity_str = "WARNING"; \
        else if (level == LogLevel::ERROR) severity_str = "ERROR"; \
        printf("[%s] ", severity_str); \
        printf(fmt, ##__VA_ARGS__); \
        printf("\n"); \
    }

