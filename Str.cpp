
/*
 * String.cpp
 *
 *  Created on: 25-jun.-2013
 *      Author: lieven2
 */

#include "Str.h"
#include <Log.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern "C" void ftoa(float f, char *str, uint8_t precision);
//extern "C" char *itoa(int value, char *str, int base);
char *itoa(int value, char *str, int base)
{
    sprintf(str,"%d",value);
}

#define DOUBLE

/*
File: tinyprintf.c

Copyright (C) 2004  Kustaa Nyholm

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*
 * Configuration
 */

/* Enable long int support */
#define PRINTF_LONG_SUPPORT

/* Enable long long int support (implies long int support) */
#define PRINTF_LONG_LONG_SUPPORT

/* Enable %z (size_t) support */
#define PRINTF_SIZE_T_SUPPORT

/*
 * Configuration adjustments
 */
#ifdef PRINTF_SIZE_T_SUPPORT
#include <sys/types.h>
#endif

#ifdef PRINTF_LONG_LONG_SUPPORT
#define PRINTF_LONG_SUPPORT
#endif

/* __SIZEOF_<type>__ defined at least by gcc */
#ifdef __SIZEOF_POINTER__
#define SIZEOF_POINTER __SIZEOF_POINTER__
#endif
#ifdef __SIZEOF_LONG_LONG__
#define SIZEOF_LONG_LONG __SIZEOF_LONG_LONG__
#endif
#ifdef __SIZEOF_LONG__
#define SIZEOF_LONG __SIZEOF_LONG__
#endif
#ifdef __SIZEOF_INT__
#define SIZEOF_INT __SIZEOF_INT__
#endif

#ifdef __GNUC__
#define _TFP_GCC_NO_INLINE_ __attribute__((noinline))
#else
#define _TFP_GCC_NO_INLINE_
#endif

/*
 * Implementation
 */
struct param {
    char lz : 1;         /**<  Leading zeros */
    char alt : 1;        /**<  alternate form */
    char uc : 1;         /**<  Upper case (for base16 only) */
    char align_left : 1; /**<  0 == align right (default), 1 == align left */
    unsigned int width;  /**<  field width */
    char sign;           /**<  The sign to display (if any) */
    unsigned int base;   /**<  number base (e.g.: 8, 10, 16) */
    char *bf;            /**<  Buffer to output */
};

#ifdef PRINTF_LONG_LONG_SUPPORT
static void _TFP_GCC_NO_INLINE_ ulli2a(unsigned long long int num,
                                       struct param *p)
{
    int n = 0;
    unsigned long long int d = 1;
    char *bf = p->bf;
    while (num / d >= p->base)
        d *= p->base;
    while (d != 0) {
        int dgt = num / d;
        num %= d;
        d /= p->base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (p->uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}

static void lli2a(long long int num, struct param *p)
{
    if (num < 0) {
        num = -num;
        p->sign = '-';
    }
    ulli2a(num, p);
}
#endif

#ifdef PRINTF_LONG_SUPPORT
static void uli2a(unsigned long int num, struct param *p)
{
    int n = 0;
    unsigned long int d = 1;
    char *bf = p->bf;
    while (num / d >= p->base)
        d *= p->base;
    while (d != 0) {
        int dgt = num / d;
        num %= d;
        d /= p->base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (p->uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}

static void li2a(long num, struct param *p)
{
    if (num < 0) {
        num = -num;
        p->sign = '-';
    }
    uli2a(num, p);
}
#endif

static void ui2a(unsigned int num, struct param *p)
{
    int n = 0;
    unsigned int d = 1;
    char *bf = p->bf;
    while (num / d >= p->base)
        d *= p->base;
    while (d != 0) {
        int dgt = num / d;
        num %= d;
        d /= p->base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (p->uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}

static void i2a(int num, struct param *p)
{
    if (num < 0) {
        num = -num;
        p->sign = '-';
    }
    ui2a(num, p);
}

static int a2d(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    else if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    else
        return -1;
}

static char a2u(char ch, const char **src, int base, unsigned int *nump)
{
    const char *p = *src;
    unsigned int num = 0;
    int digit;
    while ((digit = a2d(ch)) >= 0) {
        if (digit > base)
            break;
        num = num * base + digit;
        ch = *p++;
    }
    *src = p;
    *nump = num;
    return ch;
}

static void putchw(Str &str, struct param *p)
{
    char ch;
    int n = p->width;
    char *bf = p->bf;

    /* Number of filling characters */
    while (*bf++ && n > 0)
        n--;
    if (p->sign)
        n--;
    if (p->alt && p->base == 16)
        n -= 2;
    else if (p->alt && p->base == 8)
        n--;

    /* Fill with space to align to the right, before alternate or sign */
    if (!p->lz && !p->align_left) {
        while (n-- > 0)
            str.append(' ');
    }

    /* print sign */
    if (p->sign)
        str.append(p->sign);

    /* Alternate */
    if (p->alt && p->base == 16) {
        str.append('0');
        str.append((p->uc ? 'X' : 'x'));
    } else if (p->alt && p->base == 8) {
        str.append('0');
    }

    /* Fill with zeros, after alternate or sign */
    if (p->lz) {
        while (n-- > 0)
            str.append('0');
    }

    /* Put actual buffer */
    bf = p->bf;
    while ((ch = *bf++))
        str.append(ch);

    /* Fill with space to align to the left, after string */
    if (!p->lz && p->align_left) {
        while (n-- > 0)
            str.append(' ');
    }
}



Str& Str::format(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);	/* bf being the last argument before '...' */
    Str&  me=format(fmt,ap);
    va_end(ap);
    return me;
}

Str &Str::format(const char *fmt, va_list va)
{
    struct param p;
#ifdef PRINTF_LONG_SUPPORT
    char bf[23]; /* long = 64b on some architectures */
#else
    char bf[12]; /* int = 32b on some architectures */
#endif
    char ch;
    p.bf = bf;

    while ((ch = *(fmt++))) {
        if (ch != '%') {
            append(ch);
        } else {
#ifdef PRINTF_LONG_SUPPORT
            char lng = 0; /* 1 for long, 2 for long long */
#endif
            /* Init parameter struct */
            p.lz = 0;
            p.alt = 0;
            p.width = 0;
            p.align_left = 0;
            p.sign = 0;

            /* Flags */
            while ((ch = *(fmt++))) {
                switch (ch) {
                case '-':
                    p.align_left = 1;
                    continue;
                case '0':
                    p.lz = 1;
                    continue;
                case '#':
                    p.alt = 1;
                    continue;
                default:
                    break;
                }
                break;
            }

            /* Width */
            if (ch >= '0' && ch <= '9') {
                ch = a2u(ch, &fmt, 10, &(p.width));
            }

            /* We accept 'x.y' format but don't support it completely:
             * we ignore the 'y' digit => this ignores 0-fill
             * size and makes it == width (ie. 'x') */
            if (ch == '.') {
                p.lz = 1; /* zero-padding */
                /* ignore actual 0-fill size: */
                do {
                    ch = *(fmt++);
                } while ((ch >= '0') && (ch <= '9'));
            }

#ifdef PRINTF_SIZE_T_SUPPORT
#ifdef PRINTF_LONG_SUPPORT
            if (ch == 'z') {
                ch = *(fmt++);
                if (sizeof(size_t) == sizeof(unsigned long int))
                    lng = 1;
#ifdef PRINTF_LONG_LONG_SUPPORT
                else if (sizeof(size_t) == sizeof(unsigned long long int))
                    lng = 2;
#endif
            } else
#endif
#endif

#ifdef PRINTF_LONG_SUPPORT
                if (ch == 'l') {
                    ch = *(fmt++);
                    lng = 1;
#ifdef PRINTF_LONG_LONG_SUPPORT
                    if (ch == 'l') {
                        ch = *(fmt++);
                        lng = 2;
                    }
#endif
                }
#endif
            switch (ch) {
            case 0:
                goto abort;
            case 'f':
                char str[80];
                ftoa(va_arg(va, double), str, 5);
                append(str);
                break;
            case 'u':
                p.base = 10;
#ifdef PRINTF_LONG_SUPPORT
#ifdef PRINTF_LONG_LONG_SUPPORT
                if (2 == lng)
                    ulli2a(va_arg(va, unsigned long long int), &p);
                else
#endif
                    if (1 == lng)
                        uli2a(va_arg(va, unsigned long int), &p);
                    else
#endif
                        ui2a(va_arg(va, unsigned int), &p);
                putchw(*this, &p);
                break;
            case 'd':
            case 'i':
                p.base = 10;
#ifdef PRINTF_LONG_SUPPORT
#ifdef PRINTF_LONG_LONG_SUPPORT
                if (2 == lng)
                    lli2a(va_arg(va, long long int), &p);
                else
#endif
                    if (1 == lng)
                        li2a(va_arg(va, long int), &p);
                    else
#endif
                        i2a(va_arg(va, int), &p);
                putchw(*this, &p);
                break;
#ifdef SIZEOF_POINTER
            case 'p':
                p.alt = 1;
#if defined(SIZEOF_INT) && SIZEOF_POINTER <= SIZEOF_INT
                lng = 0;
#elif defined(SIZEOF_LONG) && SIZEOF_POINTER <= SIZEOF_LONG
                lng = 1;
#elif defined(SIZEOF_LONG_LONG) && SIZEOF_POINTER <= SIZEOF_LONG_LONG
                lng = 2;
#endif
#endif
            case 'x':
            case 'X':
                p.base = 16;
                p.uc = (ch == 'X') ? 1 : 0;
#ifdef PRINTF_LONG_SUPPORT
#ifdef PRINTF_LONG_LONG_SUPPORT
                if (2 == lng)
                    ulli2a(va_arg(va, unsigned long long int), &p);
                else
#endif
                    if (1 == lng)
                        uli2a(va_arg(va, unsigned long int), &p);
                    else
#endif
                        ui2a(va_arg(va, unsigned int), &p);
                putchw(*this, &p);
                break;
            case 'o':
                p.base = 8;
                ui2a(va_arg(va, unsigned int), &p);
                putchw(*this, &p);
                break;
            case 'c':
                append((char)(va_arg(va, int)));
                break;
            case 's':
                p.bf = va_arg(va, char *);
                putchw(*this, &p);
                p.bf = bf;
                break;
            case '%':
                append(ch);
            default:
                break;
            }
        }
    }
abort:
    ;
    return *this;
}

#if TINYPRINTF_DEFINE_TFP_PRINTF
static putcf stdout_putf;
static void *stdout_putp;

void init_printf(void *putp, putcf putf)
{
    stdout_putf = putf;
    stdout_putp = putp;
}

void tfp_printf(char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    tfp_format(stdout_putp, stdout_putf, fmt, va);
    va_end(va);
}
#endif

#if TINYPRINTF_DEFINE_TFP_SPRINTF
struct _vsnprintf_putcf_data {
    size_t dest_capacity;
    char *dest;
    size_t num_chars;
};

static void _vsnprintf_putcf(void *p, char c)
{
    struct _vsnprintf_putcf_data *data = (struct _vsnprintf_putcf_data *)p;
    if (data->num_chars < data->dest_capacity)
        data->dest[data->num_chars] = c;
    data->num_chars++;
}

int tfp_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
    struct _vsnprintf_putcf_data data;

    if (size < 1)
        return 0;

    data.dest = str;
    data.dest_capacity = size - 1;
    data.num_chars = 0;
    tfp_format(&data, _vsnprintf_putcf, format, ap);

    if (data.num_chars < data.dest_capacity)
        data.dest[data.num_chars] = '\0';
    else
        data.dest[data.dest_capacity] = '\0';

    return data.num_chars;
}

int tfp_snprintf(char *str, size_t size, const char *format, ...)
{
    va_list ap;
    int retval;

    va_start(ap, format);
    retval = tfp_vsnprintf(str, size, format, ap);
    va_end(ap);
    return retval;
}

struct _vsprintf_putcf_data {
    char *dest;
    size_t num_chars;
};

static void _vsprintf_putcf(void *p, char c)
{
    struct _vsprintf_putcf_data *data = (struct _vsprintf_putcf_data *)p;
    data->dest[data->num_chars++] = c;
}

int tfp_vsprintf(char *str, const char *format, va_list ap)
{
    struct _vsprintf_putcf_data data;
    data.dest = str;
    data.num_chars = 0;
    tfp_format(&data, _vsprintf_putcf, format, ap);
    data.dest[data.num_chars] = '\0';
    return data.num_chars;
}

int tfp_sprintf(char *str, const char *format, ...)
{
    va_list ap;
    int retval;

    va_start(ap, format);
    retval = tfp_vsprintf(str, format, ap);
    va_end(ap);
    return retval;
}
#endif
//
// cvt.c - IEEE floating point formatting routines for FreeBSD
// from GNU libc-4.6.27
//
#define CVTBUFSIZE 40
// eflag : exponent flag
#ifdef DOUBLE
static char *cvt(double arg, int ndigits, int *decpt, int *sign, char *buf,
                 int eflag)
{
    int r2;
    double fi, fj;
    char *p, *p1;

    if (ndigits < 0)
        ndigits = 0;
    if (ndigits >= CVTBUFSIZE - 1)
        ndigits = CVTBUFSIZE - 2;
    r2 = 0;
    *sign = 0;
    p = &buf[0];
    if (arg < 0) {
        *sign = 1;
        arg = -arg;
    }
    arg = modf(arg, &fi);
    p1 = &buf[CVTBUFSIZE];

    if (fi != 0) {
        p1 = &buf[CVTBUFSIZE];
        while (fi != 0) {
            fj = modf(fi / 10, &fi);
            *--p1 = (int)((fj + .03) * 10) + '0';
            r2++;
        }
        while (p1 < &buf[CVTBUFSIZE])
            *p++ = *p1++;
    } else if (arg > 0) {
        while ((fj = arg * 10) < 1) {
            arg = fj;
            r2--;
        }
    }
    p1 = &buf[ndigits];
    if (eflag == 0)
        p1 += r2;
    *decpt = r2;
    if (p1 < &buf[0]) {
        buf[0] = '\0';
        return buf;
    }
    while (p <= p1 && p < &buf[CVTBUFSIZE]) {
        arg *= 10;
        arg = modf(arg, &fj);
        *p++ = (int)fj + '0';
    }
    if (p1 >= &buf[CVTBUFSIZE]) {
        buf[CVTBUFSIZE - 1] = '\0';
        return buf;
    }
    p = p1;
    *p1 += 5;
    while (*p1 > '9') {
        *p1 = '0';
        if (p1 > buf) {
            ++*--p1;
        } else {
            *p1 = '1';
            (*decpt)++;
            if (eflag == 0) {
                if (p > buf)
                    *p = '0';
                p++;
            }
        }
    }
    *p = '\0';
    return buf;
}
#endif
/*
Str::Str() :
    Bytes(0)
{
}
*/
Str::Str(int size) : Bytes(size) {}

Str::Str(uint8_t *pstart, int size) : Bytes(pstart, size) {}
#include <string.h>
Str::Str(const char *s) : Bytes((uint8_t *)s, strlen(s))
{
}

Str &Str::set(const char *const s)
{
    Bytes::clear();
    append(s);
    return *this;
}

Str &Str::clear()
{
    Bytes::clear();
    return *this;
}

bool Str::equals(const char *s)
{
    uint32_t i;
    uint32_t slen = strlen(s);
    if (slen != length())
        return false;
    for (i = 0; i < slen && i < length(); i++)
        if (s[i] != peek(i))
            return false;
    return true;
}
#include <cstring>
bool Str::find(const char *s)
{
    char *pch = strstr((char *)c_str(), s);
    if (pch) {
        return true;
    }
    return false;
}

bool Str::endsWith(const char *s)
{
    int sl = strlen(s);
    int offset = length() - sl;
    if (offset < 0)
        return false;
    int i;
    for (i = 0; i < sl; i++)
        if (s[i] != peek(i + offset))
            return false;
    return true;
}

bool Str::startsWith(Str &s)
{
    if (s.length() > length())
        return false;
    uint32_t i;
    for (i = 0; i < s.length(); i++)
        if (s.peek(i) != peek(i))
            return false;
    return true;
}

bool Str::startsWith(const char *const s)
{
    Str ss(s);
    return startsWith(ss);
    /*   if ( s.length() > length()) return false;
    int i;
    for(i=0; i<s.length(); i++)
    if (s.peek(i) !=peek(i)) return false;
    return true;*/
}

Str &Str::operator<<(char ch)
{
    write(ch);
    return *this;
}

Str &Str::operator<<(int i)
{
    append((int32_t)i);
    return *this;
}

Str &Str::operator<<(Str &s)
{
    write(s._start, 0, s._limit);
    return *this;
}

Str &Str::operator+=(const char *s)
{
    return append(s);
}

Str &Str::operator+=(Str &s)
{
    *this << s;
    return *this;
}

const char *hexChar = "0123456789ABCDEF";

char nibbleToHex(uint8_t value)
{
    return hexChar[value & 0xF];
}

Str &Str::appendHex(uint8_t *byt, uint32_t length, char separator)
{
    for (uint32_t i = 0; i < length; i++) {
        if (i && separator)
            append(separator);
        appendHex(byt[i]);
    }
    return *this;
}

Str &Str::appendHex(Bytes &bytes)
{
    bytes.offset(0);
    while (bytes.hasData()) {
        appendHex(bytes.read());
    };
    return *this;
}

Str &Str::appendHex(uint8_t byt)
{
    write(hexChar[byt >> 4]);
    write(hexChar[byt & 0xF]);
    return *this;
}

Str &Str::appendHex(uint16_t word)
{
    appendHex((uint8_t)(word >> 8));
    appendHex((uint8_t)(word));
    return *this;
}

Str &Str::appendHex(uint32_t word)
{
    appendHex((uint8_t)(word >> 24));
    appendHex((uint8_t)(word >> 16));
    appendHex((uint8_t)(word >> 8));
    appendHex((uint8_t)(word));
    return *this;
}

Str &Str::appendHex(uint64_t word)
{
    uint32_t msb = word >> 32;
    appendHex(msb);
    appendHex((uint32_t)(word));
    return *this;
}

#ifdef DOUBLE
Str &Str::operator<<(double d)
{
    append(d);
    return *this;
}

Str &Str::operator<<(float d)
{
    append(d);
    return *this;
}
#endif

#ifdef DOUBLE
#include <cstdio>
Str &Str::append(double d)
{
    char buf[80];
#ifdef ESP8266_OPEN_RTOS
    int decpt;
    int sign;
    int i;
    cvt(d, sizeof(buf), &decpt, &sign, buf, 1);
    if (sign)
        append('-');
    for (i = 0; i < decpt; i++)
        append(buf[i]);
    append('.');
    for (uint32_t j = 0; (j + i < strlen(buf)) && j < 6; j++)
        append(buf[j + i]);
#else
    gcvt(d,80,buf);
    append(buf);
#endif

    return *this;
}

Str &Str::append(float f)
{
    double d = f;
    append(d);
    return *this;
}

#endif
Str &Str::operator=(Str &s)
{
    clear();
    s.offset(0);
    while (s.hasData()) {
        write(s.read());
    };
    return *this;
}
Str &Str::operator=(const char *s)
{
    clear();
    return append(s);
}
Str &Str::operator<<(const char *s)
{
    return append(s);
}

bool Str::operator==(Str &str)
{
    if (str.length() != length())
        return false;
    uint32_t i;
    for (i = 0; i < str.length(); i++)
        if (str.peek(i) != peek(i))
            return false;
    return true;
}

bool Str::operator==(const char *str)
{
    if (strlen(str) != length())
        return false;
    uint32_t i = 0;
    while (str[i]) {
        if (str[i] != peek(i))
            return false;
        i++;
    };
    return true;
}

Str &Str::append(const char *s)
{
    while (*s != '\0') {
        write((uint8_t)(*s));
        s++;
    }
    return *this;
}

Str &Str::append(Str &s)
{
    write(s._start, 0, s._limit);
    return *this;
}

Str &Str::append(char s)
{
    write((uint8_t)(s));
    return *this;
}

#define to_digit(c) ((c) - '0')
#define is_digit(c) ((unsigned)to_digit(c) <= 9)
#define to_char(n) ((n) + '0')

bool ishex(uint8_t c)
{
    return (c >= '0' || c <= '9') || (c >= 'A' || c <= 'F') ||
           (c >= 'a' || c <= 'f');
}

Str &Str::append(uint64_t val)
{
#define MAX_CHAR 22
    char str[MAX_CHAR];
    str[MAX_CHAR - 1] = '\0';
    char *cp = str + MAX_CHAR - 1;
    do {
        *--cp = to_char(val % 10);
        val /= 10;
    } while (val != 0);
    append(cp);
    return *this;
}

Str &Str::append(uint32_t val)
{
#define MAX_CHAR_INT32 10
    char str[MAX_CHAR_INT32];
    str[MAX_CHAR_INT32 - 1] = '\0';
    char *cp = str + MAX_CHAR_INT32 - 1;
    do {
        *--cp = to_char(val % 10);
        val /= 10;
    } while (val != 0);
    append(cp);
    return *this;
}

Str &Str::append(int32_t val)
{
    uint64_t v = val;
    if (val < 0) {
        write('-');
        v = -val;
    } else
        v = val;
    append(v);
    return *this;
}

Str &Str::append(bool b)
{
    if (b)
        append("true");
    else
        append("false");
    return *this;
}

Str &Str::append(void *ptr)
{
    union {
        void *ptr;
        uint8_t b[sizeof(int)];
    } u;
    u.ptr = ptr;
    append("0x");
    uint32_t i;
    for (i = 0; i < sizeof(int); i++)
        appendHex(u.b[i]);
    return *this;
}

Str &Str::substr(Str &mstr, uint32_t offset)
{
    mstr.offset(offset);
    while (mstr.hasData())
        write(mstr.read());
    return *this;
}

const char *Str::c_str()
{
    if (_limit < _capacity)
        *(_start + _limit) = '\0';
    else
        ERROR(" cannot put zero terminator ");
    return (char *)_start;
}

bool Str::isdigit(uint8_t v)
{
    return v >= '0' && v <= '9';
}

Erc Str::parse(Str& val)
{
    val=*this;
    return E_OK;
}

Erc Str::parse(int32_t& val)
{
    val  = atoi(c_str());
    return E_OK;
}

Erc Str::parse(uint64_t& pval)
{
    uint64_t val = 0;
    while (hasData()) {
        if (isdigit(peek())) {
            val = val * 10;
            val += read() - '0';
        } else
            return E_INVAL;
    }
    pval = val;
    return E_OK;
}

Erc Str::parse(uint32_t& pval)
{
    uint64_t val = 0;
    parse(val);
    pval = val;
    return E_OK;
}

Erc Str::parse(double& d)
{
    char buffer[40];
    uint32_t len=length();
    if ( len<sizeof(buffer)) {
        memcpy(buffer,data(),len);
        buffer[len]='\0';
        d =  atof(buffer);
        return E_OK;
    }
    return EINVAL;
}

Erc Str::parse(float& f)
{
    double d;
    Erc erc = parse(d);
    f=d;
    return erc;
}

Erc Str::parse(bool& val)
{
    if ( *this=="true") {
        val=true;
        return E_OK;
    } else  if ( *this=="false") {
        val=false;
        return E_OK;
    } else {
        return EINVAL;
    }
}

Erc Str::parseHex(Bytes &bytes)
{
    while (true) {
        uint8_t b = 0;
        if (hasData() && ishex(peek())) {
            b = hexToNibble(read());
        } else {
            break;
        };
        if (hasData() && ishex(peek())) {
            b = b << 4;
            b += hexToNibble(read());
        } else {
            return E_NO_DATA;
        }
        bytes.write(b);
    }
    return E_OK;
}

Erc Str::parseHex(uint8_t *pb)
{
    uint8_t b = 0;
    int i;
    for (i = 0; i < 2; i++)
        if (hasData())
            if (ishex(peek())) {
                b = b << 4;
                b = hexToNibble(read());
            } else
                return E_INVAL;
        else
            return E_LACK_RESOURCE;
    *pb = b;
    return E_OK;
}

#include <cstdlib>
Erc Str::parse(int64_t &ll)
{
    //  ll=atoll(c_str());
    return E_OK;
}

bool Str::isNumber()
{
    offset(0);
    enum {
        BEGIN,
        WHOLE,
        FRACT,
        EXP_BEGIN,
        EXP_VALUE
    } state = BEGIN;
    while (hasData()) {
        char ch = read();
        switch (state) {
        case BEGIN: {
            if (ch == '+' || ch == '-' || isdigit(ch))
                state = WHOLE;
            else
                return false;
            break;
        }
        case WHOLE: {
            if (isdigit(ch))
                state = WHOLE;
            else if (ch == '.')
                state = FRACT;
            else if (ch == 'E' || ch == 'e')
                state = EXP_BEGIN;
            else
                return false;
            break;
        }
        case FRACT: {
            if (isdigit(ch))
                state = FRACT;
            else if (ch == 'E' || ch == 'e')
                state = BEGIN;
            else
                return false;
        }
        case EXP_BEGIN: {
            if (ch == '+' || ch == '-' || isdigit(ch))
                state = EXP_VALUE;
            else
                return false;
        }
        case EXP_VALUE: {
            if (isdigit(ch))
                state = EXP_VALUE;
            else
                return false;
        }
        }
    }
    return true;
}

bool Str::ishex(uint8_t c)
{
    return (c >= '0' || c <= '9') || (c >= 'A' || c <= 'F') ||
           (c >= 'a' || c <= 'f');
}

uint8_t Str::hexToNibble(uint8_t ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return (ch - 'A') + 10;
    if (ch >= 'a' && ch <= 'f')
        return (ch - 'a') + 10;
    return 0;
}
/*
 * String.cpp
 *
 *  Created on: 25-jun.-2013
 *      Author: lieven2
 */

extern "C" void ftoa(float f, char *str, uint8_t precision)
{
    uint8_t i, j, divisor = 1;
    int8_t log_f;
    int32_t int_digits = (int)f; // store the integer digits
    float decimals;
    char s1[12];

    memset(str, 0, strlen(str));
    memset(s1, 0, 10);

    if (f < 0) {
        // if a negative number
        str[0] = '-'; // start the char array with '-'
        str[1]=0;
        f = abs(f);   // store its positive absolute value
    }
    log_f = ceil(log10(f)); // get number of digits before the decimal
    if (log_f > 0) {
        // log value > 0 indicates a number > 1
        if (log_f == precision) {
            // if number of digits = significant figures
            f += 0.5;        // add 0.5 to round up decimals >= 0.5
            itoa(f, s1, 10); // itoa converts the number to a char array
            strcat(str, s1); // add to the number string
        } else if ((log_f - precision) >
                   0) {
            // if more integer digits than significant digits
            i = log_f - precision; // count digits to discard
            divisor = 10;
            for (j = 0; j < i; j++)
                divisor *= 10; // divisor isolates our desired integer digits
            f /= divisor;    // divide
            f += 0.5;        // round when converting to int
            int_digits = (int)f;
            int_digits *= divisor; // and multiply back to the adjusted value
            itoa(int_digits, s1, 10);
            strcat(str, s1);
        } else {
            // if more precision specified than integer digits,
            itoa(int_digits, s1, 10); // convert
            strcat(str, s1);          // and append
        }
    }

    else {
        // decimal fractions between 0 and 1: leading 0
        s1[0] = '0';
        strcat(str, s1);
    }

    if (log_f < precision) {
        // if precision exceeds number of integer digits,
        decimals = f - (int)f; // get decimal value as float
        strcat(str, ".");      // append decimal point to char array

        i = precision - log_f; // number of decimals to read
        for (j = 0; j < i; j++) {
            // for each,
            decimals *= 10; // multiply decimals by 10
            if (j == (i - 1))
                decimals += 0.5;           // and if it's the last, add 0.5 to round it
            itoa((int)decimals, s1, 10); // convert as integer to character array
            strcat(str, s1);             // append to string
            decimals -= (int)decimals;   // and remove, moving to the next
        }
    }
}


Erc Str::copyTo(char* buffer,uint32_t maxLength)
{
    if ( maxLength < (length()-1)) return E2BIG;
    memcpy(buffer,data(),length());
    buffer[length()]='\0';
    return E_OK;
}
