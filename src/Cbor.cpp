#include "Cbor.h"
//#include "Packer.h"

IROM Cbor::Cbor() :
		Bytes() {
}

IROM Cbor::Cbor(uint32_t size) :
		Bytes(size) {
	//dtor
}

IROM Cbor::~Cbor() {
	//dtor
}

// <type:5><minor:3>[<length:0-64>][<data:0-n]
// if minor<24 => length=0
int tokenSize[] = { 1, 2, 4, 8 };

IROM bool Cbor::get(bool& bl) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) != E_OK)
		return false;
	if (type == P_BOOL) {
		bl = v._bool;
		return true;
	}
	return false;
}

IROM bool Cbor::get(int32_t& i) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) != E_OK)
		return false;
	if (type == P_NINT) {
		i = v._int64;
		return true;
	} else if (type == P_PINT) {
		i = v._uint64;
		return true;
	}
	return false;
}

IROM bool Cbor::get(uint32_t& i) {
	CborVariant v;
	PackType type;
	if ((readToken(type, v) == E_OK) && (type == P_PINT)) {
		i = v._uint64;
		return true;
	}
	return false;
}
IROM bool Cbor::get(uint64_t& l) {
	CborVariant v;
	PackType type;
	if ((readToken(type, v) == E_OK) && (type == P_PINT)) {
		l = v._uint64;
		return true;
	}
	return false;
}

IROM bool Cbor::get(float& fl) {
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
	}
	return false;
}
IROM bool Cbor::get(double& d) {
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
	}
	return false;
}

IROM bool Cbor::get(Bytes& bytes) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) == E_OK && type == P_BYTES) {
		bytes.clear();
		for (int i = 0; i < v._length; i++)
			bytes.write(read()); // skip data
		return true;
	}
	return false;
}

IROM bool Cbor::get(Str& str) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) == E_OK && type == P_STRING) {
		str.clear();
		for (int i = 0; i < v._length; i++)
			str.write(read()); // skip data
		return true;
	}
	return false;

}

IROM bool Cbor::get(char* s, int length) {
	CborVariant v;
	PackType type;
	if (readToken(type, v) == E_OK && type == P_STRING) {
		for (int i = 0; i < v._length; i++)
			if (i < length)
				*s++ = read();
			else
				read(); // skip data
		*s = '\0';
		return true;
	}
	return false;

}

IROM Erc Cbor::readToken(PackType& type, CborVariant& v) {
	int minor;
	int length;
	uint64_t value;

	if (!hasData())
		return E_NO_DATA;
	uint8_t hdr = read();
	type = (PackType) (hdr >> 5);
	minor = hdr & 0x1F;
	if (minor < 24) {
		value = minor;
	} else if (minor < 28) {
		length = tokenSize[minor - 24];
		value = getUint64(length);
	} else if (minor < 31) {
		return E_INVAL;
	} else {
		value = 65535; // suppoze very big length will be stopped by BREAK, side effect limited arrays and maps can also be breaked
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

IROM uint64_t Cbor::getUint64(int length) {
	uint64_t l = 0;
	while (length) {
		l <<= 8;
		l += read();
		length--;
	}
	return l;
}

IROM Cbor::PackType Cbor::tokenToString(Str& str) {
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
		int i;
		for (i = 0; i < v._length; i++)
			if (hasData())
				str.appendHex(read());
		return P_BYTES;
	}
	case P_STRING: {
		str << "\"";
		int i;
		for (i = 0; i < v._length; i++)
			if (hasData())
				str.append((char) read());
		str << "\"";
		return P_STRING;
	}
	case P_MAP: {
		int count = v._length;
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
		int count = v._length;
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
		int count = v._length;
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
IROM Erc Cbor::toString(Str& str) {
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


IROM Cbor& Cbor::operator<<(int i) {
	return add(i);
}

IROM Cbor& Cbor::add(int32_t i) {
	if (i >= 0)
		addToken(P_PINT, (uint64_t) i);
	else
		addToken(P_NINT, (uint64_t) -(i + 1));
	;
	return *this;
}

IROM Cbor& Cbor::add(uint32_t i) {
	addToken(P_PINT, (uint64_t) i);
	return *this;
}

IROM Cbor& Cbor::add(float fl) {
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
IROM Cbor& Cbor::add(double d) {
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
IROM Cbor& Cbor::add(Bytes& b) {
	addToken(P_BYTES, b.length());
	b.offset(0);
	while (b.hasData())
		write(b.read());

	return *this;
}
IROM Cbor& Cbor::add(Str& str) {
	addToken(P_STRING, str.length());
	str.offset(0);
	while (str.hasData())
		write(str.read());

	return *this;
}
#include <cstring>
IROM Cbor& Cbor::add(const char* s) {
	uint32_t size = strlen(s);
	addToken(P_STRING, size);
	for (uint32_t i = 0; i < size; i++)
		write(*s++);
	return *this;
}

IROM Cbor& Cbor::operator<<(const char* s) {
	return add(s);
}

IROM Cbor& Cbor::add(uint64_t i64) {
	addToken(P_PINT, i64);
	return *this;
}

IROM Cbor& Cbor::operator<<(uint64_t ull) {
	return add(ull);
}

IROM Cbor& Cbor::add(int64_t i64) {
	if (i64 >= 0)
		addToken(P_PINT, (uint64_t) i64);
	else
		addToken(P_NINT, (uint64_t) -(i64 + 1));
	return *this;
}

IROM Cbor& Cbor::add(bool b) {
	if (b)
		addHeader(P_SPECIAL, 21);
	else
		addHeader(P_SPECIAL, 20);
	return *this;
}

IROM Cbor& Cbor::addMap(int size) {
	if (size < 0)
		addHeader(P_MAP, 31);
	else
		addToken(P_MAP, size);
	return *this;
}

IROM Cbor& Cbor::addArray(int size) {
	if (size < 0)
		addHeader(P_ARRAY, 31);
	else
		addToken(P_ARRAY, size);
	return *this;
}

IROM Cbor& Cbor::addTag(int nr) {
	addToken(P_TAG, nr);
	return *this;
}

IROM Cbor& Cbor::addBreak() {
	addHeader(P_SPECIAL, 31);
	return *this;
}

IROM Cbor& Cbor::addNull() {
	addHeader(P_SPECIAL, 22);
	return *this;
}

IROM void Cbor::addToken(PackType ctype, uint64_t value) {
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

IROM void Cbor::addHeader(uint8_t major, uint8_t minor) {
	write((major << 5) | minor);
}

#include <cstdarg>
IROM bool Cbor::vaddf(const char *fmt, va_list args) {

	while (*fmt != '\0') {
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
			Bytes* s = va_arg(args, Bytes*);
			add(*s);
		} else
			return false;
		++fmt;
	}
	return true;
}

IROM bool Cbor::addf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vaddf(fmt, args);
	va_end(args);
	return true;
}

IROM bool Cbor::vscanf(const char *fmt, va_list args) {

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
		} else
			return false;
		++fmt;
	}

	return true;
}

IROM bool Cbor::scanf(const char *fmt, ...) {
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
			for (i = 0; i < cv._length; i++)
				read();
		}
		return true;
	}
	return false;
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

