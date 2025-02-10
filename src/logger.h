#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <type_traits>

// ANSI color codes for terminal output
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

// Log levels
enum class LogL { INFO, WARNING, DANGER, DEBUG };

// Logger class definition
class Logger {
public:
    // Logging function
    template <typename... Args>
    static void log(LogL level, const std::string& message, Args&&... args);

private:
    // Mutex for thread safety
    static std::mutex log_mutex;

    // Helper to print variable values
    template <typename T>
    static void print_variable(std::ostringstream& oss, T&& value);

    // Helper to get the current timestamp as a string
    static std::string get_timestamp();
};

// Initialize the static mutex
inline std::mutex Logger::log_mutex;

// Helper to print variable values
template <typename T>
void Logger::print_variable(std::ostringstream& oss, T&& value) {
    if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
        oss << "\"" << value << "\"";
    } else {
        oss << value;
    }
}

// Helper to get the current timestamp as a string
inline std::string Logger::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t_now), "%H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << millis.count();
    return oss.str();
}

// Logging function
template <typename... Args>
void Logger::log(LogL level, const std::string& message, Args&&... args) {
    std::lock_guard<std::mutex> guard(log_mutex); // Ensure thread safety

    std::ostringstream oss;
    oss << "[" << get_timestamp() << "] ";

    // Add log level
    switch (level) {
        case LogL::INFO:    oss << GREEN << "[INFO] " << RESET; break;
        case LogL::WARNING: oss << YELLOW << "[WARNING] " << RESET; break;
        case LogL::DANGER:   oss << RED << "[ERROR] " << RESET; break;
        case LogL::DEBUG:   oss << BLUE << "[DEBUG] " << RESET; break;
    }

    oss << message;

    // Format additional arguments
    ((oss << " ", print_variable(oss, std::forward<Args>(args))), ...);

    // Print the log message
    std::cout << oss.str() << std::endl;
}

