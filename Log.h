/*
 * Log.h
 *
 *  Created on: Jul 3, 2016
 *      Author: lieven
 */

#ifndef LOG_H_
#define LOG_H_

#include <Str.h>
#include <Sys.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <string>

typedef void (*LogFunction)(char* start, uint32_t length);

class Log {
  public:
    typedef enum {
        LOG_TRACE = 0,
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR,
        LOG_FATAL,
        LOG_NONE
    } LogLevel;
    static char _logLevel[7];
    std::string _line;

  private:
    bool _enabled;
    LogFunction _logFunction;
    char _hostname[20];
    char _application[20];
    LogLevel _level;

  public:
    Log(uint32_t size);
    ~Log();
    bool enabled(LogLevel level);
    void setLogLevel(char l);
    void disable();
    void enable();
    void defaultOutput();
    void setOutput(LogFunction function);
    void printf(const char* fmt, ...);
    void log(const char* file, uint32_t line, const char* function,
             const char* fmt, ...);

    void vprintf(const char* fmt, va_list args);
    const char* time();
    void host(const char* hostname);
    void location(const char* module, uint32_t line);
    void application(const char* applicationName);
    void flush();
    void level(LogLevel l);
    void logLevel();
    static void serialLog(char* start, uint32_t length);
    LogLevel level();
};

extern Log logger;
#include <cstdio>

#define LOGF(fmt, ...)                                                         \
    logger.log(__FLE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)

/*
#define LOGF(fmt, ...)                                                         \
    logger.log(__FLE__, __LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__)
*/
#define __FLE__ strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__

#ifdef ARDUINO
#define nullptr 0
#include <Arduino.h>

#define ASSERT_LOG(xxx)                                                        \
    if (!(xxx)) {                                                              \
        logger.printf(" Assertion failed %s", #xxx);                           \
        logger.flush();                                                        \
        while (1) {                                                            \
            delay(1000);                                                       \
        };                                                                     \
    }
#define ASSERT(xxx)                                                            \
    if (!(xxx)) {                                                              \
        logger.printf(" Assertion failed %s", #xxx);                           \
        logger.flush();                                                        \
        while (1) {                                                            \
            delay(1000);                                                       \
        };                                                                     \
    }

#else

#define ASSERT_LOG(xxx)                                                        \
    if (!(xxx)) {                                                              \
        logger.printf(" Assertion failed %s", #xxx);                           \
        logger.flush();                                                        \
        while (1) {                                                            \
            Sys::delay(1000);                                                  \
        };                                                                     \
    }
#define ASSERT(xxx)                                                            \
    if (!(xxx)) {                                                              \
        logger.printf(" Assertion failed %s", #xxx);                           \
        logger.flush();                                                        \
        while (1) {                                                            \
            Sys::delay(1000);                                                  \
        };                                                                     \
    }

#endif

#define INFO(fmt, ...)                                                         \
    if (logger.enabled(Log::LOG_INFO))                                         \
    LOGF(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...)                                                        \
    if (logger.enabled(Log::LOG_ERROR))                                        \
    LOGF(fmt, ##__VA_ARGS__)
#define WARN(fmt, ...)                                                         \
    if (logger.enabled(Log::LOG_WARN))                                         \
    LOGF(fmt, ##__VA_ARGS__)
#define FATAL(fmt, ...)                                                        \
    if (logger.enabled(Log::LOG_FATAL))                                        \
    LOGF(fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...)                                                        \
    if (logger.enabled(Log::LOG_DEBUG))                                        \
    LOGF(fmt, ##__VA_ARGS__)
#define TRACE(fmt, ...)                                                        \
    if (logger.enabled(Log::LOG_TRACE))                                        \
    LOGF(fmt, ##__VA_ARGS__)

#endif /* LOG_H_ */

#define LL                                                                     \
    ::fprintf(stdout, "%s:%d\n", __FILE__, __LINE__);                          \
    ::fflush(stdout)
