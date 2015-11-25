/*
 * CborQueue.cpp
 *
 *  Created on: Nov 24, 2015
 *      Author: lieven
 */

#include <CborQueue.h>

IROM CborQueue::CborQueue(uint32_t size) {
	_size = 0;
	_start = 0;
	_buffer.allocateBuffer(size);
}

IROM CborQueue::~CborQueue() {
	_buffer.freeBuffer();
}

IROM bool CborQueue::hasData() {
	return _buffer.hasData();
}

IROM bool CborQueue::hasSpace(uint32_t size) {
	return _buffer.hasSpace(size + 2);
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
#define MAX_SIZE 300
IROM Erc CborQueue::putMap(Cbor& cbor) {
	if (_size)
		return EBUSY;
	int reserved;
	int size = MAX_SIZE;
	_start = _buffer.reserve((int) size + 2, reserved);
//	INFO(" map to %d reserved ",reserved);
	cbor.map(_start + 2, reserved);
	_size = reserved;
	return E_OK;
}

IROM Erc CborQueue::putRelease(Cbor& cbor) {
	if (_size == 0)
		return ENOMEM;
	uint32_t size = cbor.length();
	*_start = size >> 8;
	*(_start + 1) = size & 0xFF;
	memcpy(_start + 2, cbor.data(), size);
	_buffer.commit(size + 2);
	cbor.map(0, 0);
	_size = 0;
	return E_OK;
}

IROM Erc CborQueue::getMap(Cbor& cbor) {
	if (_size)
		return EBUSY;
	if (!hasData())
		return ENOENT;
	uint32_t length;
	cbor.clear();
	_start = _buffer.getContiguousBlock(length);
	if (length == 0) {
		return ENOENT;
	}
	_size = *_start; // --------------- Big endian write of 16 bit size
	_size <<= 8;
	_size += *(_start + 1);
//	INFO(" map to : %X , %d", _start, _size);
	cbor.map(_start + 2, _size);

	return E_OK;
}

IROM Erc CborQueue::getRelease(Cbor& cbor) {
	if (_size) {
		cbor.map(0, 0);
		_buffer.decommitBlock(_size + 2); 	// --------------- lost message
		_size = 0;
		return E_OK;
	} else {
		return ENOENT;
	}
}
