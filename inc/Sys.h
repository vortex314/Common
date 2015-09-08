#ifndef SYS_H
#define SYS_H

#include "stdint.h"

#define DEBUG(fmt,...) Sys::log(__FILE__,__FUNCTION__,fmt,##__VA_ARGS__)
#define INFO(fmt,...)  Sys::log(__FILE__, __PRETTY_FUNCTION__ ,fmt,##__VA_ARGS__)
#define WARN(fmt,...)  Sys::log(__FILE__, __PRETTY_FUNCTION__ ,fmt,##__VA_ARGS__)
#define PERROR(erc)  Sys::log(__FILE__, __PRETTY_FUNCTION__ ,"errno : %",erc)
// ---------------------- LINUX ------------------------------
#ifdef __linux__
#endif
// ---------------------- ESP8266 ----------------------------
#ifdef __ESP8266__
#define IROM __attribute__((section(".irom0.text")))
#define IRAM __attribute__((section(".text")))
#define noinline __attribute__ ((noinline))
#define attr_pure __attribute__ ((pure))
#define attr_const __attribute__ ((const))
#endif

class Sys
{
    public:
        Sys();
        virtual ~Sys();
        static uint64_t millis();
        static void warn(int erc,const char* s);
        static void interruptEnable();
        static void interruptDisable();
        static void log(const char* file, const char* function, const char * format, ...) ;
    protected:
    private:
};

#endif // SYS_H
