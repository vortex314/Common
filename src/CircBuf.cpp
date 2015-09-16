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
#include "Board.h"

//#include "Message.h"
//#include "assert.h"

IROM CircBuf::CircBuf(int size) {
    start = new uint8_t[size];
    ASSERT(start != 0);
    readPos = 0;
    writePos = 1;
    limit = size;
}

IROM void CircBuf::clear() {
    readPos = 0;
    writePos = 1;
}

IROM CircBuf::~CircBuf() {
    delete[] start;
}
#include "Board.h"
IROM int CircBuf::write(uint8_t b) {
    uint16_t newPos = (writePos + 1) % limit;
    if (newPos == readPos)
        return -EAGAIN;
    Board::disableInterrupts();
    start[writePos] = b;
    writePos = newPos; // last operation ( hopefully atomic to ISR)
    Board::enableInterrupts();
    return 0;
}

IROM int CircBuf::writeFromIsr(uint8_t b) {
    uint16_t newPos = (writePos + 1) % limit;
    if (newPos == readPos)
        return -EAGAIN;
    start[writePos] = b;
    writePos = newPos; // last operation ( hopefully atomic to ISR)
    return 0;
}

IROM int CircBuf::readFromIsr() {
    uint16_t newPos = (readPos + 1) % limit;
    int value;
    if (newPos == writePos)
        return -1;
    else {
        value = start[newPos];
        readPos = newPos; // last operation ( hopefully atomic to ISR)
        return value;
    }
}

IROM int CircBuf::read() {
    uint16_t newPos = (readPos + 1) % limit;
    int value;
    if (newPos == writePos)
        return -1;
    else {
    	 Board::disableInterrupts();
        value = start[newPos];
        readPos = newPos; // last operation ( hopefully atomic to ISR)
        Board::enableInterrupts();
        return value;
    }
}

IROM uint32_t CircBuf::size() {
    if (writePos < readPos) {
        return writePos + limit - readPos - 1;
    } else
        return writePos - readPos - 1;
}

IROM uint32_t CircBuf::space() {
    return limit-size();
}


IROM bool CircBuf::hasSpace() {
    return ((writePos + 1) % limit) != readPos;
}

IROM bool CircBuf::hasData() {
    return ( ((readPos + 1) % limit) != writePos);
}

