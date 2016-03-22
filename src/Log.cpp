#include "Log.h"
#include "Sys.h"

Log& Log::logger = * new Log();

Log::Log() : Str(256) {
    }

#define isprint(c) (((c)>0x1F) && ((c)>0x7f))

Log& Log::flush() {
	INFO("%s",logger.c_str());
    clear();
    return *this;
    }

Log& Log::log(){
//	logger.clear();
	return logger;
}

Log& Log::dump(Bytes& bytes) {
    uint32_t  i;

    for(i=0; i<bytes.length(); i++) {
        uint8_t b;
        b=bytes.read();
        appendHex(b);
        *this << " ";
        }
    bytes.offset(0);
    for(i=0; i<bytes.length(); i++) {
        uint8_t b;
        b=bytes.read();
        if ( isprint((char)b)) *this << (char)b;
        else *this << '.';
        }
    return *this;
    }

