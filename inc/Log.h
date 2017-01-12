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
#include <Str.h>

typedef void (*LogFunction)(char* start,uint32_t length);


class Log : public Str
{
public:
	typedef enum  { LOG_TRACE=0,LOG_DEBUG,LOG_INFO,LOG_WARN,LOG_ERROR,LOG_FATAL } LogLevel;
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

extern Log log;

#ifdef ARDUINO
#define nullptr 0
#include <Arduino.h>

#define __FLE__ strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
#define LOGF(fmt,...)  if ( log.enabled(Log::LOG_FATAL)) {log.printf("%lu | %s:%d | ", millis(),__PRETTY_FUNCTION__,__LINE__);log.printf(fmt,##__VA_ARGS__);log.flush();}//delay(10);
#define ASSERT_LOG(xxx) if ( !(xxx)) { log.printf(" Assertion failed %s",#xxx); log.flush();while(1){delay(1000);};}
#define ASSERT(xxx) if ( !(xxx)) { log.printf(" Assertion failed %s",#xxx); log.flush();while(1){delay(1000);};}

#else

#define LOGF(fmt,...)  {log.time();log.host(0);log.application(0);log.printf("| %s:%d  ",__PRETTY_FUNCTION__,__LINE__);log.printf(fmt,##__VA_ARGS__);log.flush();}//delay(10);
#define ASSERT_LOG(xxx) if ( !(xxx)) { log.printf(" Assertion failed %s",#xxx); log.flush();while(1){Sys::delay(1000);};}
#define ASSERT(xxx) if ( !(xxx)) { log.printf(" Assertion failed %s",#xxx); log.flush();while(1){Sys::delay(1000);};}

#endif

#define INFO(fmt,...)  if ( log.enabled(Log::LOG_INFO)) LOGF(fmt,##__VA_ARGS__)
#define ERROR(fmt,...)  if ( log.enabled(Log::LOG_ERROR)) LOGF(fmt,##__VA_ARGS__)
#define WARN(fmt,...)  if ( log.enabled(Log::LOG_WARN)) LOGF(fmt,##__VA_ARGS__)
#define FATAL(fmt,...)  if ( log.enabled(Log::LOG_FATAL)) LOGF(fmt,##__VA_ARGS__)
#define DEBUG(fmt,...)  if ( log.enabled(Log::LOG_DEBUG)) LOGF(fmt,##__VA_ARGS__)
#define TRACE(fmt,...)  if ( log.enabled(Log::LOG_TRACE)) LOGF(fmt,##__VA_ARGS__)

#endif /* LOG_H_ */
