/*
 * LOg.cpp
 *
 *  Created on: Jul 3, 2016
 *      Author: lieven
 */
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
#endif
#ifdef ESP32_IDF
#endif
#define ESP8266_OPEN_RTOS
#ifdef ESP8266_OPEN_RTOS

#endif

void Log::serialLog(char *start, uint32_t length)
{
#ifdef ARDUINO
    Serial.write((const uint8_t *)start, length);
    Serial.write("\r\n");
#endif
#ifdef OPENCM3
    *(start + length) = '\0';
    while (*s) {
        usart_send_blocking(USART1, *(s++));
    }
#endif
#ifdef __linux__
    *(start + length) = '\0';
    fprintf(stdout, "%s\n", start);
    fflush(stdout);
#endif
#ifdef ESP8266_OPEN_RTOS
    *(start + length) = '\0';
    ::printf("%s\n", start);
    fflush(stdout);
#endif
#ifdef ESP32_IDF
    *(start + length) = '\0';
    ::printf("%s\n", start);
#endif
}
char Log::_logLevel[7] = {'T', 'D', 'I', 'W', 'E', 'F', 'N'};

Log::Log(uint32_t size) : Str(size), _enabled(true), _logFunction(serialLog), _level(LOG_INFO)
{
    _application[0] = 0;
    _hostname[0] = 0;
}

Log::~Log()
{
}

void Log::setLogLevel(char c)
{
    for (uint32_t i = 0; i < sizeof(_logLevel); i++)
        if (_logLevel[i] == c) {
            _level = (Log::LogLevel)i;
            break;
        }
}

bool Log::enabled(LogLevel level)
{
    if (level >= _level) {
        append(_logLevel[level]).append('|');
        return true;
    }
    return false;
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
#ifdef ESP8266
extern "C" {
#include <ets_sys.h>
};
#endif
#endif

void Log::printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    format(fmt, args);
    va_end(args);
}

void Log::flush()
{
    if (_logFunction)
        _logFunction((char *)data(), length());
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
//---------------------------------------------------------------------------------------------
void Log::location(const char *module, uint32_t line)
{

    printf("| %10s:%4d | ", module, line);

    //   strftime (line, sizeof(line), "%Y-%m-%d %H:%M:%S.mmm", sTm);
}

//_________________________________________ LINUX  ___________________________________________
//
#ifdef __linux__
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
//---------------------------------------------------------------------------------------------
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

//---------------------------------------------------------------------------------------------

void Log::host(const char *hostname)
{
    if (hostname == 0) {
        printf("| %s ", Sys::hostname());
    } else {
        printf("| %s ", hostname);
    }
}
extern const char *__progname;

//---------------------------------------------------------------------------------------------

void Log::application(const char *application)
{
    if (application == 0) {
        if (strlen(_application) == 0) {
            strncpy(_application, __progname, sizeof(_application) - 1);
        }
        printf("| %s ", _application);
    } else {
        printf("| %s ", application);
    }
}
#endif
//_________________________________________ EMBEDDED  ________________________________________
//
#ifndef __linux__
void Log::time()
{
    append(Sys::millis()).append(' ');
}
void Log::host(const char *hostname)
{
    append(Sys::hostname()).append(' ');
}
void Log::application(const char *application)
{
    append("-");
}
#endif
