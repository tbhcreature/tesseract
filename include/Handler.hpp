#pragma once
#include <cstdio>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <type_traits>
#include <format>
#include <string>
#include <atomic>

namespace phosphor::log {

enum class LOG_LEVEL {
    INFO = 0,
    WARN,
    ERROR,
    DEBUG,
    NONE
};

inline std::atomic<LOG_LEVEL> logLevel{LOG_LEVEL::INFO};

inline std::string toString(LOG_LEVEL level) {
    switch (level) {
        case LOG_LEVEL::INFO: return "INFO";
        case LOG_LEVEL::WARN: return "WARN";
        case LOG_LEVEL::ERROR: return "ERROR";
        case LOG_LEVEL::DEBUG: return "DEBUG";
        default: return "NONE"; 
    }
}

[[nodiscard]] inline std::string getTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = {};

#if defined(_WIN32)
    localtime_s(&tm, &time);
#elif defined(__linux__)
    // no need for __APPLE__ because xnu is linux compatible
    localtime_r(&time, &tm);
#else
    #error i dont know how to get the time on this platform!
#endif

    return std::format(
        "{:02}:{:02}:{:02}",
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec
    );
}

class core {
public:
    static void logInt(int val);
    static void logHex(uintptr_t val);
    static void logCstr(const char* str);

    inline void formatImpl(const char* fmt) {
        while (*fmt)
            std::fputc(*fmt++, stderr);
    }

    template<typename T, typename... Args>
    void formatImpl(const char* fmt, T val, Args... args);
    template<typename... Args>
    void logFmt(LOG_LEVEL level, const char* fmt, Args... args);
};

namespace detail {

inline void dispatchLog(char spec, int val) {
    switch (spec) {
        case 'd': core::logInt(val); break;
        default: std::fputc('?', stderr); break;
    }
}

inline void dispatchLog(char spec, const char* val) {
    switch (spec) {
        case 's': core::logCstr(val); break;
        case 'p':
            core::logHex(reinterpret_cast<uintptr_t>(val));
            break;
        default: std::fputc('?', stderr); break;
    }
}

template<typename T>
void dispatchLog(char spec, T* val) {
    switch (spec) {
        case 'p':
        case 'x':
            core::logHex(reinterpret_cast<uintptr_t>(val));
            break;
        default: std::fputc('?', stderr); break;
    }
}

constexpr std::size_t ctGetSpecs(const char* spec) {
    std::size_t n = 0;
    for (std::size_t i = 0; spec[i]; ++i) {
        if (
            spec[i] == '{' &&
            spec[i+1] == ':' &&
            spec[i+2] &&
            spec[i+3] == '}'
        ) {
            ++n;
            i += 3;
        }
    }
    return n;
}

inline std::size_t rtGetSpecs(const char* spec) {
    std::size_t n = 0;
    for (; *spec; ++spec) {
        if (
            spec[0] == '{' &&
            spec[1] == ':' &&
            spec[2] &&
            spec[3] == '}'
        ) {
            ++n;
            spec += 3;
        }
    }
    return n;
}

} // namespace detail   

} // namespace phosphor::log

#include "Template.hpp"