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


#endif

