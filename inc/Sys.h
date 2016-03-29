#ifndef SYS_H
#define SYS_H

#include "stdint.h"
#include "errno.h"
#include <string.h>
#include <Erc.h>

#ifdef __cplusplus
extern "C" {
#endif


uint64_t SysMillis();


//#define SYSTEM_ERROR(erc) ERROR("errno : %d",erc)

// ---------------------- LINUX ------------------------------

#if defined( __CYGWIN__) || defined(__linux__)
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
	static void delayUs(uint32_t delay);
protected:
private:
};
#endif

#endif // SYS_H
