#include <stdint.h>
#include <string.h>

#include "BipBuffer.h"
#include "Msg.h"

// const Handler* os_uart1 =(Handler*)"UART1";
// const Handler* os_spi1 =(Handler*)"SPI1";
// const Handler* os_clock =(Handler*)"CLOCK";

const char* strSignal[] = { "SIG_ALL", "SIG_INIT", "SIG_IDLE", "SIG_ERC",
		"SIG_TICK", "SIG_CONNECTED", "SIG_DISCONNECTED", "SIG_RXD", "SIG_TXD",
		"SIG_START", "SIG_STOP", "SIG_DO" };

#define ENVELOPE_SIZE	sizeof(Msg)

IROM Msg::Msg(uint32_t size) :
		Cbor(size) {

//	INFO(" Msg ctor : %d : %d ", size, _capacity);
	_signal = SIG_IDLE;
	_src = 0;
}

IROM bool Msg::is(const void* src, Signal signal) {
	if ((src == 0) || (src == _src)) {
		if ((signal == 0) || (signal == _signal)) {
			return true;
		}
	}
	return false;
}

IROM bool Msg::is(const void* src, Signal signal, int v) {
	if (src == 0 || src == _src) {
		if (signal == 0 || signal == _signal) {
			rewind();
			int _v;
			if (get(_v) && _v == v)
				return true;
		}
	}
	return false;
}

IROM Signal Msg::signal() {
	return _signal;
}

IROM void* Msg::src() {
	return _src;
}

#ifdef __CYGWIN__
#define  __WORDSIZE 64
#else
#define  __WORDSIZE 32
#endif

#if __WORDSIZE==64
#define PTR_CAST uint64_t
#elif __WORDSIZE==32
#define PTR_CAST uint32_t
#elif __WORDSIZE==16
#define PTR_CAST uint16_t
#endif

BipBuffer* Msg::_bb = 0;
Msg* __msg;
bool Msg::_init = false;

IROM bool Msg::init() {
	if (_init)
		return true;
	if (!_bb) {
		_bb = new BipBuffer();
		_bb->allocateBuffer(1024);
		__msg = new Msg(20);
		_init = true;
		return true;
	}
	return false;
}

IROM Msg& Msg::create(const void* src, Signal signal) {
	clear();
//	INFO("msg capacity : %d ",capacity());
	add((PTR_CAST) src);
	add(signal);
//	INFO("msg create %x : %d ",src,signal);
//	INFO("msg length : %d ",length());
	return *this;
}

extern "C" bool system_os_post(uint8_t prio, uint32_t p1, uint32_t par);

#define MSG_TASK_PRIO        		1

IROM Msg& Msg::send() {
	_size = length();
//	INFO(" send %d bytes ",_size);
	int reserved;
	_start = _bb->reserve((int) _size + 2, reserved);
	if ((uint32_t)reserved < (_size + 2)) {
		WARN("Bipbuffer alloc fails");
		return *this;
	}
	*_start = _size >> 8;
	*(_start + 1) = _size & 0xFF;
	memcpy(_start + 2, data(), _size);
	_bb->commit(_size + 2);
	clear();

	system_os_post((uint8_t) MSG_TASK_PRIO, 0, 0);

	return *this;
}

IROM void Msg::publish(const void* src, Signal signal) {
	init();
//	INFO(" publish msg capacity %d bytes ",__msg->capacity());
	__msg->create(src, signal).send();
}

IROM void Msg::publish(const void* src, Signal signal, int par) {
	init();
	__msg->create(src, signal) << par;
	__msg->send();
}

IROM bool Msg::receive() {
	uint32_t length;
	clear();
	_start = _bb->getContiguousBlock(length);
	if (length == 0) {
//		WARN("No message ");
		return false;
	}
	_size = *_start; // Big endian write of 16 bit size
	_size <<= 8;
	_size += *(_start + 1);
//	INFO(" received %d bytes ",_size);
//   _start =_bb->getContiguousBlock(_size);
	write(_start + 2, 0, _size);
	offset(0);
	get((PTR_CAST &) _src);
	get((int&) _signal);
//	INFO("msg recv %x : %d ",_src,_signal);
	_offset = offset();
	_bb->decommitBlock(_size + 2);
	return true;
}

IROM Msg& Msg::rewind() {
	offset(_offset);
	return *this;
}

IROM Msg& Msg::free() {
//	_bb->decommitBlock(_size + 2);
	return *this;
}
