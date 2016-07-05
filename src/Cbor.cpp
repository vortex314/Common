#include "Cbor.h"
#include <Sys.h>

Cbor::Cbor() :
		Bytes() {
}

Cbor::Cbor(uint32_t size) :
		Bytes(size) {
	//dtor
}

Cbor::~Cbor() {
	//dtor
}

// <type:5><minor:3>[<length:0-64>][<data:0-n]
// if minor<24 => length=0
int tokenSize[] = { 1, 2, 4, 8 };

bool Cbor::get(bool& bl) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) != E_OK)
		return false;
	if (type == P_BOOL) {
		bl = v._bool;
		return true;
	} else if (type == P_NILL) {
		return true;
	}
//	LOGF("get bool failed");
	return false;
}

bool Cbor::get(int32_t& i) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) != E_OK)
		return false;
	if (type == P_NINT) {
		i = -v._uint64;
		return true;
	} else if (type == P_PINT) {
		i = v._uint64;
		return true;
	} else if (type == P_NILL) {
		return true;
	}
//	LOGF("get int32_t failed");
	return false;
}

bool Cbor::get(uint32_t& i) {
	CborVariant v;
	PackType type;
	if ((readToken(type, v) == E_OK) && (type == P_PINT)) {
		i = v._uint64;
		return true;
	} else if (type == P_NILL) {
		return true;
	}
//	LOGF("get uint32_t failed");
	return false;
}
bool Cbor::get(uint64_t& l) {
	CborVariant v;
	PackType type;
	if ((readToken(type, v) == E_OK) && (type == P_PINT)) {
		l = v._uint64;
		return true;
	} else if (type == P_NILL) {
		return true;
	}
//	LOGF("get uint64_t failed");
	return false;
}

bool Cbor::get(float& fl) {
	CborVariant v;
	PackType type;
	union {
		float f;
		uint8_t b[4];
	};
	if ((readToken(type, v) == E_OK) && (type == P_SPECIAL && v._int64 == 26)) {
		for (int i = 3; i >= 0; i--)
			b[i] = read();
		fl = f;
		return true;
	} else if (type == P_NILL) {
		return true;
	}
//	LOGF("get float failed");
	return false;
}
bool Cbor::get(double& d) {
	CborVariant v;
	PackType type;
	union {
		double f;
		uint8_t b[8];
	};
	if ((readToken(type, v) == E_OK) && (type == P_SPECIAL && v._int64 == 27)) {
		for (int i = 7; i >= 0; i--)
			b[i] = read();
		d = f;
		return true;
	} else if (type == P_NILL) {
		return true;
	}
//	LOGF("get double failed");
	return false;
}

bool Cbor::get(Bytes& bytes) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) == E_OK && (type == P_BYTES || type==P_STRING)) {
		bytes.clear();
		for (uint32_t i = 0; i < v._uint64; i++)
			bytes.write(read()); // skip data
		return true;
	} else if (type == P_NILL) {
		return true;
	}
	return false;
}

bool Cbor::getMapped(Bytes& bytes) {
	CborVariant v;
	PackType type;
	bytes.clear();
	if (readToken(type, v) == E_OK && type == P_BYTES) {
		bytes.map(data() + offset(), v._uint64);
		return true;
	} else if (type == P_NILL) {
		return true;
	}
	return false;
}

bool Cbor::get(Str& str) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) == E_OK && type == P_STRING) {
		str.clear();
		for (uint32_t i = 0; i < v._uint64; i++)
			str.write(read()); // skip data
		return true;
	} else if (type == P_NILL) {
		return true;
	}
//	LOGF("get Str failed");

	return false;

}

bool Cbor::get(char* s, uint32_t length) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) == E_OK && type == P_STRING) {
		for (uint32_t i = 0; i < v._uint64; i++)
			if (i < length)
				*s++ = read();
			else
				read(); // skip data
		*s = '\0';
		return true;
	} else if (type == P_NILL) {
		return true;
	}
//	LOGF("get char* failed");
	return false;

}

Erc Cbor::readToken(PackType& type, CborVariant& v) {
	int minor;
	int length;
	uint64_t value;

	if (!hasData()) {
		LOGF("no data");
		return ENODATA;
	}
	uint8_t hdr = read();
	type = (PackType) (hdr >> 5);
	minor = hdr & 0x1F;
	if (minor < 24) {
		value = minor;
	} else if (minor < 28) {
		length = tokenSize[minor - 24];
		value = getUint64(length);
	} else if (minor < 31) {
		LOGF("invalid minor type CBOR");
		return E_INVAL;
	} else {
		value = 65535; // suppose very big length will be stopped by BREAK, side effect limited arrays and maps can also be breaked
	}
	if (type == P_SPECIAL)
		switch (minor) {
		case 21: //TRUE
		{
			type = P_BOOL;
			value = 1;
			break;
		}
		case 20: //FALSE
		{
			type = P_BOOL;
			value = 0;
			break;
		}
		case 22: //NILL
		{
			type = P_NILL;
			break;
		}
		case 26: //FLOAT32
		{
			type = P_FLOAT;
			break;
		}
		case 27: //FLOAT64
		{
			type = P_DOUBLE;
			break;
		}

		case 31: //BREAK
		{
			type = P_BREAK;
			break;
		}
		}
	v._uint64 = value;
	return E_OK;
}

uint64_t Cbor::getUint64(int length) {
	uint64_t l = 0;
	while (length) {
		l <<= 8;
		if (!hasData()) {
			LOGF(" no data ");
			return l;
		}
		l += read();
		length--;
	}
	return l;
}

Cbor::PackType Cbor::tokenToString(Str& str) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) != E_OK)
		return P_ERROR;
	switch (type) {
	case P_PINT: {
		str.append(v._uint64);
		return P_PINT;
	}
	case P_NINT: {
		str.append(v._int64);
		return P_NINT;
	}
	case P_BYTES: {
		str << "0x";
		uint32_t i;
		for (i = 0; i < v._uint64; i++)
			if (hasData())
				str.appendHex(read());
		return P_BYTES;
	}
	case P_STRING: {
		str << "\"";
		uint32_t i;
		for (i = 0; i < v._uint64; i++)
			if (hasData())
				str.append((char) read());
		str << "\"";
		return P_STRING;
	}
	case P_MAP: {
		int count = v._uint64;
		str << "{";
		for (int i = 0; i < count; i++) {
			if (i)
				str << ",";
			if (tokenToString(str) == P_BREAK)
				break;
			str << ":";
			tokenToString(str);
		}
		str << "}";
		return P_MAP;
	}
	case P_ARRAY: {
		int count = v._uint64;
		str << "[";
		for (int i = 0; i < count; i++) {
			if (i)
				str << ",";
			if (tokenToString(str) == P_BREAK)
				break;
		}
		str << "]";
		return P_ARRAY;
	}
	case P_TAG: {
		int count = v._uint64;
		str << "(";
		str << count;
		str << ":";
		tokenToString(str);
		str << ")";
		return P_TAG;
	}
	case P_BOOL: {
		if (v._bool)
			str << "true";
		else
			str << "false";
		return P_BOOL;
	}
	case P_NILL: {
		str << "null";
		return P_NILL;
	}
	case P_FLOAT: {
		union {
			float f;
			uint32_t i;
		};
		i = v._uint64;
		str << v._float;
		return P_FLOAT;
	}
	case P_DOUBLE: {
		union {
			double d;
			uint64_t i;
		};
		i = v._double;
		str << v._double;
//                str << f;
		return P_DOUBLE;
	}
	case P_BREAK: {
		str << "BREAK";
		return P_BREAK;
	}
	case P_SPECIAL: {
		return P_ERROR;
	}
	default: // avoid warnings about additional types > 7
	{
		return P_ERROR;
	}
	}

}
Erc Cbor::toString(Str& str) {
	PackType ct;
	offset(0);
	while (hasData()) {
		ct = tokenToString(str);
		if (ct == P_BREAK || ct == P_ERROR)
			return E_INVAL;
		if (hasData())
			str << ",";
	};
	return E_OK;
}

Cbor& Cbor::operator<<(int i) {
	return add(i);
}

Cbor& Cbor::add(int32_t i) {
	if (i >= 0)
		addToken(P_PINT, (uint64_t) i);
	else
		addToken(P_NINT, (uint64_t) -(i + 1));
	;
	return *this;
}

Cbor& Cbor::add(uint32_t i) {
	addToken(P_PINT, (uint64_t) i);
	return *this;
}

Cbor& Cbor::add(float fl) {
	union {
		float f;
		uint8_t b[4];
	};
	f = fl;
	addHeader(P_SPECIAL, 26);
	for (int i = 3; i >= 0; i--)
		write(b[i]);
	return *this;
}
Cbor& Cbor::add(double d) {
	union {
		double dd;
		uint8_t b[8];
	};
	dd = d;
	addHeader(P_SPECIAL, 27);
	for (int i = 7; i >= 0; i--)
		write(b[i]);
	return *this;
}
Cbor& Cbor::add(Bytes& b) {
//	INFO(" BYTES  %x : %d %d ",&b,b.length(),b.capacity());

	addToken(P_BYTES, b.length());
	b.offset(0);
	while (b.hasData())
		write(b.read());

	return *this;
}


Cbor& Cbor::add(uint8_t * data,int length) {
//	INFO(" BYTES  %x : %d %d ",&b,b.length(),b.capacity());

	addToken(P_BYTES,length);
	for(int i=0;i<length;i++){
		write(*(data+i));
	}
	return *this;
}

Cbor& Cbor::add(Str& str) {
	addToken(P_STRING, str.length());
	str.offset(0);
	while (str.hasData())
		write(str.read());

	return *this;
}

Cbor& Cbor::add(const char* s) {
	uint32_t size = strlen(s);
	addToken(P_STRING, size);
	for (uint32_t i = 0; i < size; i++)
		write(*s++);
	return *this;
}

Cbor& Cbor::operator<<(const char* s) {
	return add(s);
}

Cbor& Cbor::add(uint64_t i64) {
	addToken(P_PINT, i64);
	return *this;
}

Cbor& Cbor::operator<<(uint64_t ull) {
	return add(ull);
}

Cbor& Cbor::add(int64_t i64) {
	if (i64 >= 0)
		addToken(P_PINT, (uint64_t) i64);
	else
		addToken(P_NINT, (uint64_t) -(i64 + 1));
	return *this;
}

Cbor& Cbor::add(bool b) {
	if (b)
		addHeader(P_SPECIAL, 21);
	else
		addHeader(P_SPECIAL, 20);
	return *this;
}

Cbor& Cbor::addMap(int size) {
	if (size < 0)
		addHeader(P_MAP, 31);
	else
		addToken(P_MAP, size);
	return *this;
}

Cbor& Cbor::addArray(int size) {
	if (size < 0)
		addHeader(P_ARRAY, 31);
	else
		addToken(P_ARRAY, size);
	return *this;
}

Cbor& Cbor::addTag(int nr) {
	addToken(P_TAG, nr);
	return *this;
}

Cbor& Cbor::addBreak() {
	addHeader(P_SPECIAL, 31);
	return *this;
}

Cbor& Cbor::addNull() {
	addHeader(P_SPECIAL, 22);
	return *this;
}

void Cbor::addToken(PackType ctype, uint64_t value) {
	uint8_t majorType = (uint8_t) (ctype << 5);
	if (value < 24ULL) {
		write(majorType | value);
	} else if (value < 256ULL) {
		write(majorType | 24);
		write(value);
	} else if (value < 65536ULL) {
		write(majorType | 25);
		write(value >> 8);
		write(value);
	} else if (value < 4294967296ULL) {
		write(majorType | 26);
		write(value >> 24);
		write(value >> 16);
		write(value >> 8);
		write(value);
	} else {
		write(majorType | 27);
		write(value >> 56);
		write(value >> 48);
		write(value >> 40);
		write(value >> 32);
		write(value >> 24);
		write(value >> 16);
		write(value >> 8);
		write(value);
	}
}

void Cbor::addHeader(uint8_t major, uint8_t minor) {
	write((major << 5) | minor);
}

#include <cstdarg>
bool Cbor::vaddf(const char *fmt, va_list args) {
//	LOGF(" fmt : '%s'",fmt );

	while (*fmt != '\0') {
//		LOGF("%c",*fmt);
		if (*fmt == 'i') {
			int32_t i = va_arg(args, int32_t);
			add(i);
		} else if (*fmt == 'u') {
			uint32_t i = va_arg(args, uint32_t);
			add(i);
		} else if (*fmt == 's') {
			char * s = va_arg(args, char*);
			add(s);
		} else if (*fmt == 'l') {
			int64_t v = va_arg(args, int64_t);
			add(v);
		} else if (*fmt == 'd') {
			double v = va_arg(args, double);
			add(v);
		} /*else if (*fmt == 'f') {
		 float v = va_arg(args, float);
		 add(v);
		 } */else if (*fmt == 'b') {
			int v = va_arg(args, int);
			if (v)
				add(true);
			else
				add(false);
		} else if (*fmt == 'S') {
			Str* s = va_arg(args, Str*);
			add(*s);
		} else if (*fmt == 'B') {
			Bytes* bytes = va_arg(args, Bytes*);
			add(*bytes);
		} else
			return false;
		++fmt;
	}
	return true;
}

bool Cbor::addf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vaddf(fmt, args);
	va_end(args);
	return true;
}

Cbor& Cbor::putf(const char *fmt, ...) {
	clear();
	va_list args;
	va_start(args, fmt);
	vaddf(fmt, args);
	va_end(args);
	return *this;
}

bool Cbor::vscanf(const char *fmt, va_list args) {

	while (*fmt != '\0') {
		if (*fmt == 'i') {
			int32_t* pi = va_arg(args, int32_t*);
			if (get(*pi) == false)
				return false;
		} else if (*fmt == 'u') {
			uint32_t* pi = va_arg(args, uint32_t*);
			if (get(*pi) == false)
				return false;
		} else if (*fmt == 'f') {
			float* pv = va_arg(args, float*);
			if (get(*pv) == false)
				return false;
		} else if (*fmt == 'd') {
			double* pv = va_arg(args, double*);
			if (get(*pv) == false)
				return false;
		} else if (*fmt == 'S') {
			Str* s = va_arg(args, Str*);
			if (get(*s) == false)
				return false;
		} else if (*fmt == 's') {
			char * s = va_arg(args, char*);
			int length = va_arg(args, int);
			if (get(s, length) == false)
				return false;
		} else if (*fmt == 'b') {
			bool* pv = va_arg(args, bool*);
			if (get(*pv) == false)
				return false;
		} else if (*fmt == 'B') {
			Bytes* s = va_arg(args, Bytes*);
			if (get(*s) == false)
				return false;
		} else {
			LOGF("invalid format");
			return false;
		}
		++fmt;
	}

	return true;
}

bool Cbor::scanf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	bool b = vscanf(fmt, args);
	va_end(args);
	return b;
}

bool Cbor::skipToken() {
	PackType pt;
	CborVariant cv;
	if (readToken(pt, cv) == E_OK) {
		if (pt == P_BYTES || pt == P_STRING) { // skip remaining bytes
			int i;
			for (i = 0; i < cv._uint64; i++)
				read();
		}
		return true;
	}
	return false;
}

Cbor& Cbor::addKey(uint32_t key) {
	return add(key);
}

bool Cbor::gotoKey(uint32_t key) {
	uint32_t _key;
	offset(0);
	while (available()) {
		if (get(_key)) {
			if (_key == key)
				return true;
		} else {
			return false;
		};
		if (!skipToken())
			return false;
	}
	return false;
}

