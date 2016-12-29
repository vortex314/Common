/*
 * Log.h
 *
 *  Created on: Jul 3, 2016
 *      Author: lieven
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdint.h>
#include <stdarg.h>
#include <Sys.h>
#ifdef __cplusplus

#ifdef __linux__
#define LINE_LENGTH 1024
#else
#define LINE_LENGTH 256
#endif
typedef void (*LogFunction)(char* start,uint32_t length);
#ifdef ARDUINO
#define nullptr 0
#include <Arduino.h>
#include <Log.h>
#define __FLE__ strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
//#define LOGF(fmt,...) {Serial.printf("%ld | %s\t%s:%d \t| ", millis(),__FILE__,__FUNCTION__,__LINE__);Serial.printf(fmt,##__VA_ARGS__);Serial.println();}//delay(10);
#define LOGF(fmt,...)  if ( Log.enabled(LogMana::LOG_FATAL)) {Log.printf("%lu | %s:%d | ", millis(),__PRETTY_FUNCTION__,__LINE__);Log.printf(fmt,##__VA_ARGS__);Log.flush();}//delay(10);
#define ASSERT_LOG(xxx) if ( !(xxx)) { Log.printf(" Assertion failed %s",#xxx); Log.flush();while(1){delay(1000);};}
#define ASSERT(xxx) if ( !(xxx)) { Log.printf(" Assertion failed %s",#xxx); Log.flush();while(1){delay(1000);};}
#else

#define LOGF(fmt,...)  {Log.time();Log.host(0);Log.application(0);Log.printf("| %s:%d  ",__PRETTY_FUNCTION__,__LINE__);Log.printf(fmt,##__VA_ARGS__);Log.flush();}//delay(10);
#define INFO(fmt,...)  if ( Log.enabled(LogManager::LOG_INFO)) LOGF(fmt,##__VA_ARGS__)
#define ERROR(fmt,...)  if ( Log.enabled(LogManager::LOG_ERROR)) LOGF(fmt,##__VA_ARGS__)
#define WARN(fmt,...)  if ( Log.enabled(LogManager::LOG_WARN)) LOGF(fmt,##__VA_ARGS__)
#define FATAL(fmt,...)  if ( Log.enabled(LogManager::LOG_FATAL)) LOGF(fmt,##__VA_ARGS__)
#define DEBUG(fmt,...)  if ( Log.enabled(LogManager::LOG_DEBUG)) LOGF(fmt,##__VA_ARGS__)
#define TRACE(fmt,...)  if ( Log.enabled(LogManager::LOG_TRACE)) LOGF(fmt,##__VA_ARGS__)

#define ASSERT_LOG(xxx) if ( !(xxx)) { Log.printf(" Assertion failed %s",#xxx); Log.flush();while(1){Sys::delay(1000);};}
#define ASSERT(xxx) if ( !(xxx)) { Log.printf(" Assertion failed %s",#xxx); Log.flush();while(1){Sys::delay(1000);};}
#endif


class LogManager
{
public:
    typedef enum  { LOG_TRACE=0,LOG_DEBUG,LOG_INFO,LOG_WARN,LOG_ERROR,LOG_FATAL } LogLevel;
private:
    bool _enabled;
    LogFunction _logFunction;
    char _hostname[20];
    char _application[20];
    char _record[LINE_LENGTH];
    uint16_t _offset;
    LogLevel _level;
public:

    LogManager();
    ~LogManager();
    bool enabled(LogLevel level);
    void disable();
    void enable();
    void defaultOutput();
    void setOutput(LogFunction function);
    void printf(const char* fmt,...);
    void vprintf(const char* fmt,va_list args);
    void time();
    void host(const char* hostname);
    void application(const char* applicationName);
    void flush();
    void level(LogLevel l);
    LogLevel level();
};

extern LogManager Log;
#endif

#endif /* LOG_H_ */
