/*
 * BipBuffer.h
 *
 *  Created on: 12-sep.-2014
 *      Author: lieven2
 */

#ifndef BIPBUFFER_H_
#define BIPBUFFER_H_
#include <stdint.h>
#include <string.h>
#include "Sys.h"


class BipBuffer {
private:
	uint8_t* pBuffer;
	int ixa;
	int sza;
	int ixb;
	int szb;
	int buflen;
	int ixResrv;
	int szResrv;

public:
	BipBuffer() IROM ;
	~BipBuffer() IROM;
	bool allocateBuffer(int buffersize) IROM;
	void clear()  IROM;
	void freeBuffer()  IROM;
	uint8_t* reserve(int size, int& reserved) IROM;
	void commit(int size) IROM;
	uint8_t* getContiguousBlock(uint32_t& size) IROM;
	void decommitBlock(int size) IROM;
	int getCommittedSize() const  IROM;
	int getReservationSize() const IROM;
	int getBufferSize() const IROM;
	bool isInitialized() const IROM;
	bool hasSpace(uint32_t) IROM;
	 bool hasData() IROM;
private:
	int getSpaceAfterA() const  IROM;

	int getBFreeSpace() const  IROM;
};


#endif /* BIPBUFFER_H_ */
