#ifndef SYS_H
#define SYS_H

#include "stdint.h"
#include "errno.h"
//#include <string.h>
#include <Erc.h>

#ifdef __cplusplus



#ifdef ARDUINO
#define nullptr 0
#include <Arduino.h>
#include <Log.h>
#define __FLE__ strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__
//#define LOGF(fmt,...) {Serial.printf("%ld | %s\t%s:%d \t| ", millis(),__FILE__,__FUNCTION__,__LINE__);Serial.printf(fmt,##__VA_ARGS__);Serial.println();}//delay(10);
#define LOGF(fmt,...)  if ( Log.enabled()) {Log.printf("%lu | %s:%d | ", millis(),__PRETTY_FUNCTION__,__LINE__);Log.printf(fmt,##__VA_ARGS__);Log.flush();}//delay(10);
#define ASSERT_LOG(xxx) if ( !(xxx)) { Log.printf(" Assertion failed %s",#xxx); Log.flush();while(1){delay(1000);};}
#define ASSERT(xxx) if ( !(xxx)) { Log.printf(" Assertion failed %s",#xxx); Log.flush();while(1){delay(1000);};}
#else
#include <Log.h>
#define LOGF(fmt,...)  if ( Log.enabled()) {Log.time();Log.printf(" | %s:%d | ",__PRETTY_FUNCTION__,__LINE__);Log.printf(fmt,##__VA_ARGS__);Log.flush();}//delay(10);
#define ASSERT_LOG(xxx) if ( !(xxx)) { Log.printf(" Assertion failed %s",#xxx); Log.flush();while(1){Sys::delay(1000);};}
#define ASSERT(xxx) if ( !(xxx)) { Log.printf(" Assertion failed %s",#xxx); Log.flush();while(1){Sys::delay(1000);};}
#endif
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
	static uint64_t _upTime;
public:
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
