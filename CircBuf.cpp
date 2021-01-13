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
// start[readPos] => location of next read

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

int CircBuf::write(uint8_t b) {
    if ((writePos - readPos) >= (limit - 1))
        return -EAGAIN;
    //	Board::disableInterrupts();
    start[writePos % limit] = b;
    writePos++;
    //	Board::enableInterrupts();
    return 0;
}

int CircBuf::writeFromIsr(uint8_t b) {
    if ((writePos - readPos) >= (limit - 1))
        return -EAGAIN;
    start[writePos % limit] = b;
    writePos++;
    return 0;
}

int CircBuf::readFromIsr() {
    if (readPos < writePos) {
        readPos++;
        return start[(readPos - 1) % limit];
    }
    return -1;
}

int CircBuf::read() {
    //		Board::disableInterrupts();
    if (readPos < writePos) {
        readPos++;
        return start[(readPos - 1) % limit];
    }
    return -1;
    //		Board::enableInterrupts();
}

uint32_t CircBuf::size() { return (writePos - readPos); }

uint32_t CircBuf::space() { return limit - size(); }

bool CircBuf::hasSpace() { return size() < (limit - 1); }

bool CircBuf::hasSpace(uint32_t sz) { return size() < sz; }

bool CircBuf::hasData() { // not in  as it will be called in interrupt
    return writePos > readPos;
}
