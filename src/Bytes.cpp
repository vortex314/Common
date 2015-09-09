/*
 * ByteBuffer.cpp
 *
 *  Created on: 24-aug.-2012
 *      Author: lieven
 */
#include <malloc.h>
#include "Bytes.h"
#ifdef __linux__
#include <stdlib.h>
#else
//#include "new.h"
#endif
void myMemcpy(uint8_t *dst, uint8_t* src, int length) {
	for (int i = 0; i < length; i++)
		dst[i] = src[i];
}

Bytes::Bytes() {
	_start = 0;
	_offset = 0;
	_limit = 0;
	_capacity = 0;
	isMemoryOwner = false;
}

Bytes::Bytes(uint8_t *st, uint32_t length) {
	_start = st;
	_offset = 0;
	_limit = length;
	_capacity = length;
	isMemoryOwner = false;
}

void Bytes::map(uint8_t* st, uint32_t length) {
	_start = st;
	_offset = 0;
	_limit = length;
	_capacity = length;
	isMemoryOwner = false;
}

/* Bytes::Bytes() {
 _start = (uint8_t*) 0;
 _offset = 0;
 _limit = 0;
 _capacity = 0;
 isMemoryOwner = false;
 }*/

Bytes::Bytes(uint32_t size) {
	_start = 0;
	if (size > 0) {
		_start = (uint8_t*)malloc(size); // (uint8_t*) Sys::malloc(size);
		ASSERT(_start != 0);
	}
	_offset = 0;
	_limit = 0;
	_capacity = size;
	isMemoryOwner = true;
}

Bytes::~Bytes() {
	if (isMemoryOwner)
		if ( _start)
		free( _start);
}
Bytes::Bytes(Bytes& src) {
	_start = (uint8_t*)malloc(src._capacity);
	_offset = 0;
	_limit = src._limit;
	_capacity = src._capacity;
	myMemcpy(_start, src._start, src._limit);
	isMemoryOwner = true;
}

Bytes& Bytes::clone(Bytes& src) {
	myMemcpy(_start, src._start, _capacity);
	_offset = 0;
	_limit = (_capacity > src._limit) ? src._limit : _capacity;
	return *this;
}

Bytes& Bytes::sub(Bytes* parent, uint32_t length) {
	_start = parent->_start + parent->_offset;
	_offset = 0;
	if (length <= (parent->_capacity - parent->_offset))
		_limit = length;
	else
		_limit = parent->_capacity - parent->_offset;
	_capacity = _limit;
	isMemoryOwner = false;
	return *this;
}

Bytes& Bytes::append(Bytes& b) {
	b.offset(0);
	while (b.hasData())
		write(b.read());
	return *this;
}

Bytes& Bytes::append(const char s[]) {
	while (*s != '\0') {
		write((uint8_t) (*s));
		s++;
	}
	return *this;
}

Bytes& Bytes::operator=(Bytes& s) {
	clear();
	return append(s);
}

Bytes& Bytes::operator=(const char* s) {
	clear();
	return append(s);
}



Bytes& Bytes::move(int32_t dist) {
	if ((_offset + dist) > _limit)
		_offset = _limit;
	else
		_offset += dist;
		return *this;
}

/* ByteBuffer::ByteBuffer(ByteBuffer& in) {
 start = in.start;
 _offset = 0;
 _limit = in._limit;
 _capacity = in._capacity;
 isMemoryOwner = false;
 }*/

uint8_t* Bytes::data() const {
	return _start;
}

int Bytes::capacity() {
	return _capacity;
}

uint32_t Bytes::length() const {
	return _limit;
}

int Bytes::offset(int32_t pos) {
	if (pos < 0)
		_offset = _limit;
	else if ((uint32_t) pos < _capacity)
		_offset = pos;
	return 0;
}

Erc Bytes::insert(uint32_t offset, Bytes* data) {
	if (data->_limit + _limit > _capacity)
		return E_LACK_RESOURCE;
	if (offset > _limit)
		return E_INVAL;
	// move last part further
	uint32_t delta = data->length();
	uint32_t i;
	for (i = _limit; i >= offset; i--)
		_start[i + delta] = _start[i];
	// insert data
	for (i = 0; i < delta; i++)
		_start[offset + i] = data->_start[i];
	return E_OK;
}

int Bytes::offset() {
	return _offset;
}

int Bytes::length(int l) {
	_offset = 0;
	_limit = l;
	return 0;
}

int Bytes::available() {
	if (_offset < _limit)
		return _limit - _offset;
	else
		return 0;
}

Erc Bytes::write(uint8_t value) {
	if (_offset < _capacity) {
		_start[_offset++] = value;
		_limit = _offset;
	} else
		return E_LACK_RESOURCE;
	return 0;
}

Erc Bytes::write(uint8_t* data, int offset, int length) {
	for (int i = 0; i < length; i++) {
		int erc;
		erc = write(data[offset + i]);
		if (erc)
			return erc;
	}
	return 0;
}

Erc Bytes::write(Bytes* data) {
	return write(data->_start, 0, data->_limit);
}

Erc Bytes::read(uint8_t* data) {
	if (_offset < _limit)
		*data = _start[_offset++];
	else
		return E_AGAIN;
	return 0;
}

uint8_t Bytes::read() {
	if (_offset < _limit)
		return _start[_offset++];
	return '-';
}

Bytes& Bytes::clear() {
	_offset = 0;
	_limit = 0;
	return *this;
}

bool Bytes::equals(const uint8_t* pb,uint32_t length){
	if ( length != _limit ) return false;
	for(uint32_t i=0;i<length;i++){
		if ( _start[i] != pb[i]) return false;
	}
	return true;
}


int Bytes::poke(uint32_t idx, uint8_t b) {
	if (idx > _limit)
		return E_LACK_RESOURCE;
	_start[idx] = b;
	return 0;
}

int Bytes::peek() {
	return _start[_offset];
}

int Bytes::peek(int32_t offset) {
	return _start[offset];
}

bool Bytes::hasData() {
	return _offset < _limit;
}

bool Bytes::hasSpace() {
	return _limit < _capacity;
}
const char *HEX = "0123456789ABCDEF";
#include "Str.h"
void Bytes::toString(Str& str) {
	uint32_t i;
	uint8_t b;
	for (i = 0; i < _limit; i++) {
		b = *(_start + i);
		str.append(HEX[b >> 4]);
		str.append(HEX[b & 0xF]);
		str.append(' ');
	}
}


