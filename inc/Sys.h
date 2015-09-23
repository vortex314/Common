#ifndef SYS_H
#define SYS_H

#include "stdint.h"
#include "errno.h"
 #include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void SysLog(const char *type,const char* file, const char* function, const char * format, ...);

#ifdef __cplusplus
};
#endif

#define ERROR(fmt,...) SysLog("ERROR",__FILE__,__FUNCTION__,fmt,##__VA_ARGS__)
#define DEBUG(fmt,...) SysLog("DEBUG",__FILE__,__FUNCTION__,fmt,##__VA_ARGS__)
#define INFO(fmt,...)  SysLog("INFO ",__FILE__,__FUNCTION__ ,fmt,##__VA_ARGS__)
#define WARN(fmt,...)  SysLog("WARN ",__FILE__, __PRETTY_FUNCTION__ ,fmt,##__VA_ARGS__)
#define PERROR()  Sys::log("ERROR",__FILE__, __PRETTY_FUNCTION__ ,"line : %d - System failure : %d : %s ",__LINE__,errno,strerror(errno));
#define SYSTEM_ERROR(erc) ERROR("errno : %d",erc)
// ---------------------- LINUX ------------------------------
#ifdef __linux__
#define IRAM
#define IROM
#endif
// ---------------------- ESP8266 ----------------------------
#ifdef __ESP8266__
#define IROM __attribute__((section(".irom0.text")))
#define IROM1 __attribute__((section(".irom01.text")))
#define IRAM __attribute__((section(".text")))
#define noinline __attribute__ ((noinline))
#define attr_pure __attribute__ ((pure))
#define attr_const __attribute__ ((const))
#endif




#ifdef __cplusplus
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
#endif

#endif // SYS_H
