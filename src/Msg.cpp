#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "BipBuffer.h"
#include "Msg.h"

// const Handler* os_uart1 =(Handler*)"UART1";
// const Handler* os_spi1 =(Handler*)"SPI1";
// const Handler* os_clock =(Handler*)"CLOCK";


#define ENVELOPE_SIZE	sizeof(Msg)

IROM Msg::Msg(uint32_t size) : Cbor(size)
{
    _signal = SIG_IDLE;
    _src=0;
}

bool Msg::is(void* src, Signal signal)
{
    if (signal==0 || signal == _signal)
    {
        if (src == 0 || src == _src)
        {
            return true;
        }
    }
    return false;
}

bool Msg::is(void* src, Signal signal,int v)
{
    if (signal==0 || signal == _signal)
    {
        if (src == 0 || src == _src)
        {
            rewind();
            int _v;
            if ( get(_v) && _v == v ) return true;
        }
    }
    return false;
}

Signal Msg::signal()
{
    return _signal;
}

void* Msg::src()
{
    return _src;
}

#if __WORDSIZE==64
#define PTR_CAST uint64_t
#elif __WORDSIZE==32
#define PTR_CAST uint32_t
#elif __WORDSIZE==16
#define PTR_CAST uint16_t
#endif


BipBuffer* Msg::_bb;

IROM bool Msg::init()
{
    if (!_bb)
    {
        _bb = new BipBuffer();
        _bb->allocateBuffer(1024);
        return true;
    }
    return false;
}

IROM Msg& Msg::create(void* src,Signal signal)
{
    _size=512+2;
    int reserved;
    if ( (_start =  _bb->reserve((int)_size,reserved)))
    {
        map(_start+2,reserved-2);
        add((uint64_t)src);
        add(signal);
    }
    return *this;
}

IROM Msg& Msg::send()
{
    uint32_t size=length()+2;

    *_start = size >>8;
    *(_start+1)= size & 0xFF;
    _bb->commit(size);
    map(0,0);
    return *this;
}

IROM void Msg::publish(void* src,Signal signal)
{
    Msg msg(0);
    msg.create(src,signal).send();
}

IROM void Msg::publish(void* src,Signal signal,int par)
{
    Msg msg(0);
    msg.create(src,signal) << par;
    msg.send();
}



bool Msg::receive()
{
    _size=512;
    _start=_bb->getContiguousBlock(_size);
    if ( _size == 0 ) return false;
    _size = *_start;     // Big endian write of 16 bit size
    _size <<=8;
    _size += *(_start+1);
//   _start =_bb->getContiguousBlock(_size);
    map(_start+2,_size-2);
    get((uint64_t&)_src);
    get((int&)_signal);
    _offset = offset();
    return true;
}

Msg& Msg::rewind()
{
    offset(_offset);
    return *this;
}

Msg& Msg::free()
{
    _bb->decommitBlock(_size);
    return *this;
}




