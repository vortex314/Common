#ifndef SYS_H
#define SYS_H

#include "stdint.h"
#include "errno.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum  SysLogLevel {
	LOG_TRACE = 0, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL
} SysLogLevel;
extern const char* SysLogLevelStr[];

void SysLog(SysLogLevel level, const char* file, const char* function,
		const char * format, ...);
uint64_t SysMillis();

#define TRACE(fmt,...) SysLog(LOG_TRACE,__FILE__,__FUNCTION__,fmt,##__VA_ARGS__)
#define DEBUG(fmt,...) SysLog(LOG_DEBUG,__FILE__,__FUNCTION__,fmt,##__VA_ARGS__)
#define INFO(fmt,...)  SysLog(LOG_INFO,__FILE__,__FUNCTION__ ,fmt,##__VA_ARGS__)
#define WARN(fmt,...)  SysLog(LOG_WARN,__FILE__, __FUNCTION__ ,fmt,##__VA_ARGS__)
#define ERROR(fmt,...)  SysLog(LOG_ERROR,__FILE__, __FUNCTION__ ,fmt,##__VA_ARGS__)
#define FATAL(fmt,...) SysLog(LOG_FATAL,__FILE__,__FUNCTION__,fmt,##__VA_ARGS__)
#define PERROR()  SysLog(LOG_ERROR,__FILE__, __FUNCTION__ ,"line : %d - System failure : %d : %s ",__LINE__,errno,strerror(errno));
//#define SYSTEM_ERROR(erc) ERROR("errno : %d",erc)

// ---------------------- LINUX ------------------------------
#ifdef __linux__
#define IRAM
#define IROM
#endif
// ---------------------- ESP8266 ----------------------------
#ifdef __ESP8266__
#include "mem.h"
#define IROM __attribute__((section(".irom0.text")))
#define IROM1 __attribute__((section(".irom01.text")))
#define IRAM __attribute__((section(".text")))
#define noinline __attribute__ ((noinline))
#define attr_pure __attribute__ ((pure))
#define attr_const __attribute__ ((const))
void* malloc(uint32_t size);
void free(void* ptr);

extern const char* CLOCK_ID;
extern const char* MQTT_ID;
extern const char* TCP_ID;
extern const char* WIFI_ID;

#endif

#ifdef __cplusplus
}
;
#endif

#ifdef __cplusplus
class Sys {
public:
	Sys();
	virtual ~Sys();
	static uint64_t millis();
	static void warn(int erc, const char* s);
	static void interruptEnable();
	static void interruptDisable();
	static void log(SysLogLevel level, const char* file, const char* function,
			const char * format, ...);
protected:
private:
};
#endif

#endif // SYS_H
