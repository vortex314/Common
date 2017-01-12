/*
 * LOg.cpp
 *
 *  Created on: Jul 3, 2016
 *      Author: lieven
 */
#include "Log.h"
#include <stdio.h>



#include <Log.h>
#include <Str.h>
#include <stdlib.h>
#include <stdio.h>


#ifdef ARDUINO
#include <WString.h>
#include <Arduino.h>
#endif
#ifdef OPENCM3
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
static void usart_send_string(const char *s)
{
	while (*s) {
		usart_send_blocking(USART1, *(s++));
	}
}
#endif

void serialLog(char* start, uint32_t length)
{
#ifdef ARDUINO
	Serial.write(start, length);
	Serial.write("\r\n");
#endif
#ifdef OPENCM3
	*(start + length) = '\0';
	usart_send_string(start);
#endif
#ifdef __linux__
	*(start + length) = '\0';
	fprintf(stdout, "%s\n", start);
	fflush(stdout);
#endif
}

Log::Log(uint32_t size) :	Str(size),	_enabled(true), _logFunction(serialLog), _level(LOG_ERROR)
{
	_application[0] = 0;
	_hostname[0] = 0;

}

Log::~Log()
{

}

bool Log::enabled(LogLevel level)
{
	return level >= _level;
}
void Log::disable()
{
	_enabled = false;
}
void Log::enable()
{
	_enabled = true;
}

void Log::defaultOutput()
{
	_logFunction = serialLog;
}

void Log::setOutput(LogFunction function)
{
	_logFunction = function;
}
#ifdef ARDUINO
extern "C" {
#include <ets_sys.h>
};

#endif

void Log::printf(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	format(fmt,args);
/*	if (_offset < LINE_LENGTH) {
		
#ifdef ARDUINO
//		ets_vsnprintf((char*) (_record + _offset), fmt, args);
//		_offset = strlen(_record);
		_offset += ets_vsnprintf((char*) (_record + _offset), LINE_LENGTH - _offset,
		                    fmt, args);
#else
		_offset +=vsnprintf((char*) (_record + _offset), LINE_LENGTH - _offset,
		                    fmt, args);
#endif

	}*/
	va_end(args);
}

void Log::flush()
{
	if (_logFunction)
		_logFunction((char*)data(), length());
	clear();
}

void Log::level(LogLevel l)
{
	_level = l;
}

Log::LogLevel Log::level()
{
	return _level;
}

#ifdef __linux__
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
void Log::time()
{
	struct timeval tv;
	struct timezone tz;
	struct tm *tm;
	gettimeofday(&tv, &tz);
	tm = ::localtime(&tv.tv_sec);
	printf("%d/%02d/%02d %02d:%02d:%02d.%03ld ",
	                   tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
	                   tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);

//   strftime (line, sizeof(line), "%Y-%m-%d %H:%M:%S.mmm", sTm);
}
#include <unistd.h>
#include <string.h>
void Log::host(const char* hostname)
{
	if (hostname == 0) {
		if (strlen(_hostname) == 0) {
			::gethostname(_hostname, sizeof(_hostname));
		}
		printf( "| %s ", _hostname);
	} else {
		printf( "| %s ", hostname);
	}
}
extern const char *__progname;


void Log::application(const char* application)
{
	if (application == 0) {
		if (strlen(_application) == 0) {
			strncpy(_application,__progname,sizeof(_application)-1);
		}
		printf( "| %s ", _application);
	} else {
		printf( "| %s ", application);
	}
}
#endif
#ifndef __linux__
void Log::time()
{
	Str str((uint8_t*) (_record + _offset), LINE_LENGTH - _offset);
	str.append(Sys::millis()).append(' ');
	_offset += str.length();
}
void Log::host(const char* hostname)
{
	Str str((uint8_t*) (_record + _offset), LINE_LENGTH - _offset);
	str.append(Sys::hostname()).append(' ');
	_offset += str.length();
}
void Log::application(const char* application)
{

}
#endif
