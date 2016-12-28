/*
 * LOg.cpp
 *
 *  Created on: Jul 3, 2016
 *      Author: lieven
 */
#include "Log.h"

LogManager Log;

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
static void usart_send_string(const char *s) {
	while (*s) {
		usart_send_blocking(USART1, *(s++));
	}
}
#endif

void serialLog(char* start, uint32_t length) {
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

LogManager::LogManager() :		_enabled(true), _logFunction(serialLog), _offset(0), _level(LOG_ERROR) {
	_application[0] = 0;
	_hostname[0] = 0;

}

LogManager::~LogManager() {

}

bool LogManager::enabled(LogLevel level) {
	return level >= _level;
}
void LogManager::disable() {
	_enabled = false;
}
void LogManager::enable() {
	_enabled = true;
}

void LogManager::defaultOutput() {
	_logFunction = serialLog;
}

void LogManager::setOutput(LogFunction function) {
	_logFunction = function;
}

void LogManager::printf(const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	if (_offset < LINE_LENGTH)
		_offset += vsnprintf((char*) (_record + _offset), LINE_LENGTH - _offset,
				fmt, args);
	va_end(args);
}

void LogManager::flush() {
	if (_logFunction)
		_logFunction(_record, _offset);
	_offset = 0;
}

void LogManager::level(LogLevel l) {
	_level = l;
}

LogManager::LogLevel LogManager::level() {
	return _level;
}

#ifdef __linux__
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
void LogManager::time() {
	struct timeval tv;
	struct timezone tz;
	struct tm *tm;
	gettimeofday(&tv, &tz);
	tm = ::localtime(&tv.tv_sec);
	_offset += sprintf(&_record[_offset], "%d/%02d/%02d %02d:%02d:%02d.%03ld ",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
			tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);

//   strftime (line, sizeof(line), "%Y-%m-%d %H:%M:%S.mmm", sTm);
}
#include <unistd.h>
#include <string.h>
void LogManager::host(const char* hostname) {
	if (hostname == 0) {
		if (strlen(_hostname) == 0) {
			::gethostname(_hostname, sizeof(_hostname));
		}
		_offset += sprintf(&_record[_offset], "| %s ", _hostname);
	} else {
		_offset += sprintf(&_record[_offset], "| %s ", hostname);
	}
}
extern const char *__progname;


void LogManager::application(const char* application) {
	if (application == 0) {
		if (strlen(_application) == 0) {
			strncpy(_application,__progname,sizeof(_application)-1);
		}
		_offset += sprintf(&_record[_offset], "| %s ", _application);
	} else {
		_offset += sprintf(&_record[_offset], "| %s ", application);
	}
}
#endif
#ifndef __linux__
void LogManager::time() {
	Str str((uint8_t*) (_record + _offset), LINE_LENGTH - _offset);
	str.append(Sys::hostname()).append(' ');
	str.append(Sys::millis());
	_offset += str.length();
}
void LogManager::host(const char* hostname) {

}
void LogManager::application(const char* application) {

}
#endif
