#ifndef FAST_H
#define FAST_H
#include <errno.h>
#include <stdint.h>
#include <string>
/*
 * [TYPE][LENGTH][LENGTH][LENGTH]....[VALUE]
 * [____________________________]
 *
 * [TYPE][TAG][LENGTH][VALUE]
 *
 *
 * */

#define TYPEOF(U) ((Type)(U >> 24))
#define SIZEOF(U) ((uint32_t)U & 0xFFFFFF)
class Fast {
    uint32_t _offset;
    uint32_t _capacity;
    uint32_t _used;
    uint32_t* _start;

    inline int append(uint32_t w);
    inline int append(uint8_t* bytes, uint32_t length);
    int next(uint32_t& w);

  public:
    typedef enum {
        BYTES,
        BOOL,
        INT64,
        UINT64,
        FLOAT,
        STRING,
        UTC,
        NILL,
        RESERVED,
        ARRAY,
        OBJECT,
        KEY,
        KEY_SHORT,
        EXTENDED
    } Type;
    Fast(uint32_t size);
    ~Fast();
    int peek(Type& type, uint32_t& length);
    bool isType(Type t);

    int write(uint8_t* bytes, uint32_t size);
    int write(uint64_t v);
    int write(std::string& s);
    int write(float f);
    int write(double d);
    int write(uint32_t i);

    int read(uint8_t* bytes, uint32_t size);
    int read(uint64_t v);
    int read(std::string& s);
    int read(float f);
    int read(double d);
    int read(uint32_t i);
};

#endif // FAST_H
