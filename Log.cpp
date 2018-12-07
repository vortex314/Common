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

char Log::_logLevel[7] = {'T', 'D', 'I', 'W', 'E', 'F', 'N'};

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
//#define ESP8266_OPEN_RTOS
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
#if defined(__linux__) || defined(ESP8266_OPEN_RTOS) || defined(ESP32_IDF)
    *(start + length) = '\0';
    fprintf(stdout, "%s\n", start);
    fflush(stdout);
#endif
}


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

void Log::logLevel()
{
    append(_logLevel[_level]).append(" | ");
}

void Log::host(const char *hostname)
{
    append(Sys::hostname()).append(" | ");
}
void Log::application(const char *application)
{
    append(" | ");
}

void Log::location(const char *module, uint32_t line)
{
    append(module).append(':').append(line).append(" | ");
}

#ifdef ESP8266
extern "C" {
#include <ets_sys.h>
};
#endif

#ifdef ARDUINO

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
    static char buffer[100];
    gettimeofday(&tv, &tz);
    tm = ::localtime(&tv.tv_sec);
    sprintf(buffer,"%02d:%02d:%02d.%03ld ", tm->tm_hour,
           tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);

    append(buffer).append(" | ");
}

extern const char *__progname;

#endif
//_________________________________________ EMBEDDED  ________________________________________
//

#if ! defined(__linux__) && ! defined(__ARDUINO__)



void Log::time()
{
    append(Sys::millis()).append(" | ");
}

#endif
