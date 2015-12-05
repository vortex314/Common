/*
 * Json.h
 *
 *  Created on: 26-okt.-2014
 *      Author: lieven2
 */

#ifndef JSON_H_
#define JSON_H_

#include "Str.h"
#include "jsmn.h"
#include <stdarg.h>
// #define DOUBLE
#define JSON_MAX_TOKENS 10

class Json : public Str  {
public:
//	Json(Str& str);
//	Json(Bytes& bytes);
	IROM Json(uint32_t size);
	IROM virtual ~Json();
	IROM Json& clear();

	IROM Json& add(int i);
	IROM Json& add(uint32_t i);
#ifdef DOUBLE
	IROM Json& add(float f);
	IROM Json& add(double d);
#endif
	IROM Json& add(Bytes& b);
	IROM Json& add(Str& str);
	IROM Json& add( char const* s);
	IROM Json& add(uint64_t i64);
	IROM Json& add(int64_t i64);
	IROM Json& add(bool b);
	IROM Json& addMap(int size);
	IROM Json& addMap();
	IROM Json& addArray(int size);
	IROM Json& addArray();
	IROM Json& addKey(const char* s);
	IROM Json& addTag(int nr);
	IROM Json& addBreak();
	IROM Json& addNull();

	IROM Erc parse();
	IROM bool get(double& d);
	IROM bool get(bool& bl);
	IROM bool get(Str& str);
	IROM bool get(Bytes& str);
	IROM bool get(int64_t& ul);
	IROM bool get( char* s,int length);
	IROM bool getArray();
	IROM bool getMap();
	IROM bool scanf(const char* fmt,...);
	IROM bool vscanf(const char *fmt, va_list args);
//	Erc readToken(PackType& type,Variant& variant);
	IROM Erc toString(Str& str);

protected:
private:
//	void addToken(PackType type, uint64_t data);
	IROM void addHeader(uint8_t major, uint8_t minor);
	IROM uint64_t getUint64(int length);
	IROM void addComma();
	IROM void mapToken(Str& str);
//	PackType tokenToString(Str& str);
	uint8_t _break[JSON_MAX_TOKENS]; // nested objects within object index, highest bit==on -> array, else map
	int _breakIndex;
	jsmn_parser _parser;
	jsmntok_t _tokens[JSON_MAX_TOKENS];
	int _tokenCount;
	int _tokenIndex;
};

#endif /* JSON_H_ */
