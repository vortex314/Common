/*
 * CborQueue.cpp
 *
 *  Created on: Nov 24, 2015
 *      Author: lieven
 */

#include <CborQueue.h>

IROM CborQueue::CborQueue(uint32_t size) {
	_buffer.allocateBuffer(size);
}

IROM CborQueue::~CborQueue() {
	_buffer.freeBuffer();
}

IROM bool CborQueue::hasData() {
	return _buffer.hasData();
}

IROM bool CborQueue::hasSpace(uint32_t size) {
	return hasSpace(size + 2);
}

IROM Erc CborQueue::put(Cbor& cbor) {
	uint32_t size = cbor.length();
	int reserved;
	uint8_t* start = _buffer.reserve((int) size + 2, reserved);
	if ((uint32_t) reserved < (size + 2)) {
		_buffer.commit(0);
		return ENOMEM;
	}
	*start = size >> 8;
	*(start + 1) = size & 0xFF;
	memcpy(start + 2, cbor.data(), size);
	_buffer.commit(size + 2);
	return E_OK;
}

IROM Erc CborQueue::get(Cbor& cbor) {
	if (!hasData())
		return ENOENT;
	uint32_t length;
	cbor.clear();
	uint8_t* start = _buffer.getContiguousBlock(length);
	if (length == 0) {
		return ENOENT;
	}
	uint32_t size = *start; // --------------- Big endian write of 16 bit size
	size <<= 8;
	size += *(start + 1);

	if (size > (uint32_t) cbor.capacity()) {
		_buffer.decommitBlock(size + 2); 	// --------------- lost message
		return ENOMEM;
	} else {
		cbor.write(start + 2, 0, size); 	//-------------- copy to cbor
		cbor.offset(0);						//---------------- rewind
		_buffer.decommitBlock(size + 2);
		return E_OK;
	}

}

