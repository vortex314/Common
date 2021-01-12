/*
 * CircBuf.cpp
 *
 *  Created on: 19-aug.-2012
 *      Author: lieven
 */
//#include "base.h"
#include "CircBuf.h"
#include "Erc.h"
#include "Sys.h"

// start[writePos] => location of next write
// start[readPos] => location of last read 

CircBuf::CircBuf(int size) {
    start = new uint8_t[size];
    //	ASSERT(start != 0);
    readPos = 0;
    writePos = 0;
    limit = size;
}

void CircBuf::clear() {
    readPos = 0;
    writePos = 0;
}

CircBuf::~CircBuf() { delete[] start; }

int CircBuf::write(uint8_t b) { // not in  as it will be called in interrupt
    if ((writePos - readPos) == limit)
        return -EAGAIN;
    //	Board::disableInterrupts();
    start[writePos++ % limit] = b;
    //	Board::enableInterrupts();
    return 0;
}

int CircBuf::writeFromIsr(uint8_t b) {
    if ((writePos - readPos) == limit)
        return -EAGAIN;
    start[writePos++ % limit] = b;
    return 0;
}

int CircBuf::readFromIsr() {
    if (readPos < writePos)
        return start[++readPos % limit];
    return -1;
}

int CircBuf::read() {
    //		Board::disableInterrupts();
    if (readPos < writePos)
        return start[++readPos % limit];
    return -1;
    //		Board::enableInterrupts();
}

uint32_t CircBuf::size() { return writePos - readPos; }

uint32_t CircBuf::space() { return limit - size(); }

bool CircBuf::hasSpace() { return size() < limit; }

bool CircBuf::hasSpace(uint32_t sz) { return size() < sz; }

bool CircBuf::hasData() { // not in  as it will be called in interrupt
    return writePos > readPos;
}
