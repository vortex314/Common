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
#define LINE_LENGTH 256
typedef void (*LogFunction)(char* start,uint32_t length);

class LogManager {
public:

private:
	 bool _enabled;
	 LogFunction _logFunction;
	 char* _record;
	 uint16_t _offset;
public:
	 LogManager();
	 ~LogManager();
	 bool enabled();
	 void disable();
	 void enable();
	 void defaultOutput();
	 void setOutput(LogFunction function);
	 void printf(const char* fmt,...);
	 void vprintf(const char* fmt,va_list args);
	 void flush();
};

extern LogManager Log;

#endif /* LOG_H_ */
