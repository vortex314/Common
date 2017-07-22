/*
 * String.h
 *
 *  Created on: 25-jun.-2013
 *      Author: lieven2
 */

#ifndef STRING_H_
#define STRING_H_

#include "Erc.h"
#include "Bytes.h"
#include <stdarg.h>

class Log;

class Str : public Bytes
{
public:
//    Str();
    Str(int size);
//    Str(const char*s );
    Str(Bytes& bytes);
    Str(const char* const s);
    Str(uint8_t* data, int capacity);
    const char* c_str();
    Str& clear();
    Str& set(const char* const s);
    Str& operator<<(const char* s);
    Str& operator+(const char* s) { return append(s);};
    Str& operator+=(const char* s) ;
    Str& operator<<(char ch);
    Str& operator<<(int i);
    Str& operator<<(Str& s);
    Str& operator<<(float f);
    Str& operator<<(double d);
    Str& append(int64_t val)
    {
        if ( val < 0 ) append('-');
        append((uint64_t)(0-val));
        return *this;
    }
    Str& operator<<(uint64_t val)
    {
        return append(val);
    };
    Str& operator+=(Str& s);
    Str& operator=(const char* const s);
    Str& operator=(Str& s);
    Str& append(const char* s);
    Str& append(Str& str);
    Str& append(char s);
    Str& append(void* ptr);
    Str& append(uint64_t val);
    Str& append(uint32_t val);
    Str& append(int32_t val);
    Str& append(float f);
    Str& append(double d);
    Str& append(bool b);
    Str& appendHex(uint8_t byte);
    Str& appendHex(uint32_t word);
    Str& appendHex(uint64_t word);
    Str& appendHex(uint8_t* byte,uint32_t length,char separator);
	Str& format(const char* fmt,va_list va);
    Str& substr(Str& master,uint32_t offset);

    bool operator==(Str& str);
    bool operator==(const char* s);
    bool find(const char *str);
    bool endsWith(const char* end);
    bool startsWith(Str& str);
    bool startsWith(const char* s);
    bool equals(const char* str);
    bool isNumber();
    bool isHex();

    Erc parse(uint64_t* pval);
    Erc parseHex(Bytes& bytes);
    Erc parse(uint32_t* pval);
    Erc parse(int32_t* pval);
    Erc parse(double& d);
    Erc parse(int64_t& ll);
    Erc parseHex(uint8_t* pb);
    static bool ishex(uint8_t ch);
    static bool isdigit(uint8_t ch);
    static uint8_t hexToNibble(uint8_t ch);
private:

};

Str& operator+(Str& lhs,Str& rhs);



#endif /* STRING_H_ */
