#include "logger.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

#include "platform.h"

#if _WIN32
    #include <direct.h>
    #include <windows.h>
#elif __linux__
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

// public methods

// Meyers' Singleton
logger& logger::instance() {
    static logger instance;
    return instance;
}

void logger::log(log_level level, const std::string& message, const char* file, int line, const char* function) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!log_file_.is_open()) return;
    
    std::string formatted = format_message(level, message, file, line, function);
    log_file_ << formatted << std::endl;
    log_file_.flush();
}

void logger::set_log_file(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (log_file_.is_open()) log_file_.close();
    
    log_filepath_ = filepath;
    log_file_.open(log_filepath_, std::ios::out | std::ios::app);
    
    if (!log_file_.is_open())
        std::cerr << "Failed to open log file: " << log_filepath_ << std::endl;
}

// private methods

logger::logger() : log_filepath_("logs" PATH_SEPARATOR "mouse-jiggler.log") {
#if _WIN32
    _mkdir("logs");
#elif __linux__
    mkdir("logs", DIR_PERMISSIONS);
#endif
    
    log_file_.open(log_filepath_, std::ios::out | std::ios::app);
    
    if (!log_file_.is_open())
        std::cerr << "Failed to open log file: " << log_filepath_ << std::endl;
}

logger::~logger() {
    if (log_file_.is_open()) log_file_.close();
}

std::string logger::get_timestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::tm tm_buf;
#if _WIN32
    localtime_s(&tm_buf, &time);
#elif __linux__
    localtime_r(&time, &tm_buf);
#endif
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

std::string logger::level_to_string(log_level level) const {
    switch (level) {
        case log_level::LOG_ERROR:   return "ERROR";
        default:                     return "UNKNOWN";
    }
}

std::string logger::format_message(log_level level, const std::string& message, const char* file, int line, const char* function) const {
    // Extract filename from full path
    std::string filename(file);
    size_t pos = filename.find_last_of("/\\");
    if (pos != std::string::npos)
        filename = filename.substr(pos + 1);
    
    std::ostringstream oss;
    oss << "[" << get_timestamp() << "] "
        << "[" << level_to_string(level) << "] "
        << "[" << filename << ":" << line << ":" << function << "] "
        << message;
    
    return oss.str();
}
