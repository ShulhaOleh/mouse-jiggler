#pragma once

#include <string>
#include <fstream>
#include <mutex>

enum class log_level {
    LOG_ERROR
};

class logger {
public:
    static logger& instance();

    void log(log_level level, const std::string& message, const char* file, int line, const char* function);
    
    void set_log_file(const std::string& filepath);
    
    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;

private:
    logger();
    ~logger();
    
    std::string get_timestamp() const;
    std::string level_to_string(log_level level) const;
    std::string format_message(log_level level, const std::string& message, const char* file, int line, const char* function) const;
    
    std::ofstream log_file_;
    std::mutex mutex_;
    std::string log_filepath_;
};

// Logging macros
#define LOG_ERROR(message) \
    logger::instance().log(log_level::LOG_ERROR, message, __FILE__, __LINE__, __FUNCTION__)
