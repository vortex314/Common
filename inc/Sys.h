#ifndef SYS_H
#define SYS_H

#include "stdint.h"
#include "errno.h"
//#include <string.h>
#include <Erc.h>
#include <Log.h>

//#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined   (__GNUC__)        /* GNU Compiler */
    #define __ALIGN_END    __attribute__ ((aligned (4)))
    #define __ALIGN_BEGIN
  #else
    #define __ALIGN_END
    #if defined   (__CC_ARM)      /* ARM Compiler */
      #define __ALIGN_BEGIN    __align(4)
    #elif defined (__ICCARM__)    /* IAR Compiler */
      #define __ALIGN_BEGIN
    #elif defined  (__TASKING__)  /* TASKING Compiler */
      #define __ALIGN_BEGIN    __align(4)
    #endif /* __CC_ARM */
  #endif /* __GNUC__ */
//#else
//  #define __ALIGN_BEGIN
//  #define __ALIGN_END
//#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */

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
#define IRAM __attribute__((section(".iram.text")))
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
	static char _hostname[30];
public:
	static  uint64_t _upTime;
	Sys();
	virtual ~Sys();
	static void init();
	static uint64_t millis();
	static void warn(int erc, const char* s);
	static void interruptEnable();
	static void interruptDisable();
	static void delayUs(uint32_t delay);
	static void delay(uint32_t msec);
	static void tick();
	static void hostname(const char*hn);
	static const char* hostname();
protected:
private:
};
#endif

#endif // SYS_H
