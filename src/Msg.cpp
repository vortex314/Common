#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "BipBuffer.h"
#include "Msg.h"

// const Handler* os_uart1 =(Handler*)"UART1";
// const Handler* os_spi1 =(Handler*)"SPI1";
// const Handler* os_clock =(Handler*)"CLOCK";


#define ENVELOPE_SIZE	sizeof(Msg)

Msg::Msg(uint32_t size) : Cbor(size)
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

Signal Msg::sig()
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

bool Msg::init()
{
    if (!_bb)
    {
        _bb = new BipBuffer();
        _bb->allocateBuffer(1024);
        return true;
    }
    return false;
}

Msg& Msg::alloc(int size)
{
    _size=size+2;
    int reserved;
    if ( (_start =  _bb->reserve((int)_size,reserved)))
    {
        map(_start+2,reserved);
    }
    return *this;
}

Msg& Msg::send()
{
    uint32_t size=length()+2;

    *_start = size >>8;
    *(_start+1)= size & 0xFF;
    _bb->commit(size);
    map(0,0);
    return *this;
}



Msg& Msg::receive()
{
    _size=512;
    _start=_bb->getContiguousBlock(_size);
    _size = *_start;     // Big endian write of 16 bit size
    _size <<=8;
    _size += *(_start+1);
 //   _start =_bb->getContiguousBlock(_size);
    map(_start+2,_size-2);
    get(_src);
    get(_signal);
    return *this;
}

Msg& Msg::free()
{
    _bb->decommitBlock(_size);
    return *this;
}


