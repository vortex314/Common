#ifndef CBOR_H
#define CBOR_H
#include "Bytes.h"
//#include "Packer.h"
#include "Str.h"
#include "errno.h"
#include <cstdarg>
#include "Sys.h"

class Cbor: public Bytes {
public:
	typedef union CborVariant {
		uint64_t _uint64;
		int64_t _int64;
		double _double;
		float _float;
		bool _bool;
		int _length;
	} CborVariant;
	typedef enum {
		P_PINT = 0,
		P_NINT,
		P_BYTES,
		P_STRING,
		P_ARRAY,
		P_MAP,
		P_TAG,
		P_SPECIAL = 7, // major types
		P_BOOL,
		P_FLOAT,
		P_DOUBLE,
		P_BREAK,
		P_NILL,
		P_ERROR
	} // minor additional types
	PackType;
	Cbor();
	Cbor(uint32_t size);
	~Cbor();

	IROM Cbor& operator<<(uint32_t i) {
		return add(i);
	}
	IROM Cbor& add(int i);
	IROM Cbor& operator<<(int i);
	IROM Cbor& operator<<(const char *s);
	IROM Cbor& operator<<(uint64_t l);
	IROM Cbor& add(uint32_t i);
	IROM Cbor& add(float f);
	IROM Cbor& add(double d);
	IROM Cbor& add(Bytes& b);
	IROM Cbor& add(Str& str);
	IROM Cbor& add(const char* s);
	IROM Cbor& add(uint64_t i64);
	IROM Cbor& add(int64_t i64);
	IROM Cbor& add(bool b);
	IROM Cbor& addMap(int size);
	IROM Cbor& addArray(int size);
	IROM Cbor& addTag(int nr);
	IROM Cbor& addBreak();
	IROM Cbor& addNull();
	IROM Cbor& addKey(uint32_t idx);
	IROM bool addf(const char *fmt, ...);
	IROM bool vaddf(const char *fmt, va_list args);
	IROM bool scanf(const char* fmt, ...);
	IROM bool vscanf(const char* fmt, va_list args);
	IROM void sprintf(Str& s);

	IROM bool get(bool& bl);
	IROM bool get(uint32_t& i);
	IROM bool get(uint64_t& l);
	IROM bool get(float& f);
	IROM bool get(double& d);
	IROM bool get(int32_t& i);
	IROM bool get(char*s, int length);
	IROM bool get(Bytes& bytes);
	IROM bool get(Str& str);
	IROM bool gotoKey(uint32_t idx);
	IROM bool getMapped(Bytes& bytes);

	Erc readToken(PackType& type, CborVariant& variant);
	Erc toString(Str& str);
	Bytes& bytes() {
		return *this;
	}

protected:
private:
	void addToken(PackType type, uint64_t data);
	bool skipToken();
	void addHeader(uint8_t major, uint8_t minor);
	uint64_t getUint64(int length);
	PackType tokenToString(Str& str);
};

#endif // CBOR_H
