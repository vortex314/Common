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
public:
	CborQueue(uint32_t size) IROM ;
	virtual ~CborQueue() IROM;
	Erc put(Cbor& cbor) IROM;
	Erc get(Cbor& cbor) IROM;
	bool hasData() IROM;
	bool hasSpace(uint32_t size) IROM;
};

#endif /* INC_CBORQUEUE_H_ */
