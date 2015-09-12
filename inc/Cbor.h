#ifndef CBOR_H
#define CBOR_H
#include "Bytes.h"
//#include "Packer.h"
#include "Str.h"
#include "errno.h"
#include <cstdarg>
#include "platform.h"



class Cbor: public Bytes
{
public:
    typedef union CborVariant
    {
        uint64_t _uint64;
        int64_t _int64;
        double _double;
        float _float;
        bool _bool;
        int _length;
    } CborVariant;
    typedef enum
    {
        P_PINT = 0, P_NINT, P_BYTES, P_STRING, P_ARRAY, P_MAP, P_TAG, P_SPECIAL = 7, // major types
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

    Cbor& operator<<(uint32_t i)
    {
        return add(i);
    }
    Cbor& add(int i);
    Cbor& operator<<(int i);
    Cbor& operator<<(const char *s);
    Cbor& operator<<(uint64_t l);
    Cbor& add(uint32_t i);
    Cbor& add(float f);
    Cbor& add(double d);
    Cbor& add(Bytes& b);
    Cbor& add(Str& str);
    Cbor& add(const char* s);
    Cbor& add(uint64_t i64);
    Cbor& add(int64_t i64);
    Cbor& add(bool b);
    Cbor& addMap(int size);
    Cbor& addArray(int size);
    Cbor& addTag(int nr);
    Cbor& addBreak();
    Cbor& addNull();

    bool addf(const char *fmt,...);
    bool scanf(const char* fmt,... );
    void sprintf(Str& s);

    bool get(bool& bl);
    bool get(uint32_t& i);
    bool get(uint64_t& l);
    bool get(float& f);
    bool get(double& d);
    bool get(int32_t& i);
    bool get(char*s,int length );
    bool get(Bytes& bytes);
    bool get(Str& str);


    Erc readToken(PackType& type,CborVariant& variant);
    Erc toString(Str& str);
    Bytes& bytes()
    {
        return *this;
    };

protected:
private:
    void addToken(PackType type, uint64_t data);
    void addHeader(uint8_t major, uint8_t minor);
    uint64_t getUint64(int length);
    PackType tokenToString(Str& str);
};

#endif // CBOR_H
