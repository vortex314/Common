#include "Slip.h"

Slip::Slip(uint32_t size) : Bytes(size)
{
    //ctor
}

Slip::~Slip()
{
    //dtor
}


//PUBLIC
//_________________________________________________________________________

Slip& Slip::addCrc() //PUBLIC
//_________________________________________________________________________
{
	offset(-1); // position at end
	uint16_t crc = Fletcher16(_start, _limit);
	write(crc & 0xFF);
	write(crc >> 8);
	return *this;
}

uint16_t Slip::Fletcher16(uint8_t *begin, int length) {
	uint16_t sum1 = 0;
	uint16_t sum2 = 0;
	uint8_t *end = begin + length;
	uint8_t *index;

	for (index = begin; index < end; index++) {
		sum1 = (sum1 + *index) % 255;
		sum2 = (sum2 + sum1) % 255;
	}

	return (sum2 << 8) | sum1;
}
//_________________________________________________________________________

bool Slip::isGoodCrc() //PUBLIC
//_________________________________________________________________________
{
	if (_limit < 3)
		return false; // need at least 3 Slip
	uint16_t crc = Fletcher16(_start, _limit - 2);
	if ((*(_start + _limit - 2) == (crc & 0xFF))
			&& ((*(_start + _limit - 1) == (crc >> 8))))
		return true;
	return false;
}

//_________________________________________________________________________

Slip& Slip::removeCrc() //PUBLIC
//_________________________________________________________________________
{
	_limit -= 2;
	return *this;
}
//PUBLIC

//PUBLIC
//_________________________________________________________________________

Slip& Slip::decode() //PUBLIC
//_________________________________________________________________________
{
	uint8_t *p, *q;
	uint8_t *_capacity = _start + _limit;
	for (p = _start; p < _capacity; p++) {
		if (*p == ESC) {
			*p = (uint8_t) (*(p + 1) ^ 0x20);
			for (q = p + 1; q < _capacity; q++)
				*q = *(q + 1);
			_capacity--;
			//		p++; // skip next uint8_t could also be an escape
		}
	}
	_limit = _capacity - _start;
	return *this;
}

//_________________________________________________________________________

Slip& Slip::encode() //PUBLIC
//_________________________________________________________________________
{
	uint8_t *p, *q;
	uint8_t *_capacity = _start + _limit;
	for (p = _start; p < _capacity; p++) {
		if ((*p == SOF) || (*p == ESC)) {
			for (q = _capacity; q > p; q--)
				*(q + 1) = *q;
			_capacity++;
			*(p + 1) = *p ^ 0x20;
			*p = ESC;
		}
	}
	_limit = _capacity - _start;
	return *this;
}

//_________________________________________________________________________

Slip& Slip::frame() //PUBLIC
//_________________________________________________________________________
{
	uint8_t *q;
	uint8_t *end = _start + _limit;
	for (q = end; q >= _start; q--)
		*(q + 1) = *q;
	*_start = SOF;
	*(end + 1) = SOF;
	end += 2;
	_limit = end - _start;
	return *this;
}

bool Slip::fill(uint8_t b) {
	if (b == SOF) {
		if (_offset > 0)
			return true;
		else {
			return false;  // don't add SOF
		}
	} else
		write(b);
	return false;
}
