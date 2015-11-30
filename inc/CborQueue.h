/*
 * CborQueue.h
 *
 *  Created on: Nov 24, 2015
 *      Author: lieven
 */

#ifndef INC_CBORQUEUE_H_
#define INC_CBORQUEUE_H_

#include "Erc.h"
#include "Cbor.h"
#include <BipBuffer.h>

class CborQueue {
private:
	BipBuffer _buffer;
//	Cbor cbor;
	uint32_t _size;
	uint8_t* _start;
	Erc getMap(Cbor& cbor);
	Erc getRelease(Cbor& cbor);
	Erc putMap(Cbor& cbor);
	Erc putRelease(Cbor& cbor);
public:
	CborQueue(uint32_t size) IROM;
	virtual ~CborQueue() IROM;
	Erc put(Cbor& cbor) IROM;
	Erc putf(const char * format, ...) IROM;
	Erc get(Cbor& cbor) IROM;
	Erc getf(const char * format, ...) IROM;

	bool hasData() IROM;
	bool hasSpace(uint32_t size) IROM;
	uint32_t getCapacity();
	uint32_t getUsed();
};

#endif /* INC_CBORQUEUE_H_ */
