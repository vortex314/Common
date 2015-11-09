#include "Sys.h"

Sys::Sys() {
	//ctor
}

Sys::~Sys() {
	//dtor
}
#ifdef __linux__
#include <time.h>

uint64_t Sys::millis() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	return ((unsigned long long)t.tv_sec*1000) + ( (unsigned long long)t.tv_nsec/1000000);
}



#include "errno.h"
#include "stdio.h"
#include "string.h"
void Sys::warn(int erc, const char* msg) {
	log(LOG_INFO,"", "", "%lu %s : %s \n", millis(), strerror(erc), msg);
}



#include "stdarg.h"

void Sys::log(SysLogLevel level,const char* file, const char* function, const char * format,...) {
	uint32_t time = millis();
	char buffer[256];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 256, format, args);
	va_end(args);
	char dst[50];
#ifdef __ESP8266__
	dst[0] = '\0';
	strAlign(dst, 15, file, strlen(file));
	strAlign(&dst[15], 15, function, strlen(function));
#else
	const char *ps = strrchr(file,'/');
	const char *pf=strchr(function,' ');
//	pf  = strchr(pf,'(');
	sprintf(dst,"%10.10s - %10.10s",ps+1,pf+1);
#endif
	printf("%06d.%03d | %s | %s\r\n", time / 1000, time % 1000, dst, buffer);
}

void Sys::interruptDisable() {
}
void Sys::interruptEnable() {
}

#endif

