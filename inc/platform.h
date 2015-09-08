
#ifdef ESP8266
#define IRAM section()
#else

#define IRAM
#define IROM
#define UINT32_MAX (uint32_t)0xFFFFFFFF

#endif


