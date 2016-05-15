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

 Msg::Msg(uint32_t size) :
		Cbor(size) {

//	INFO(" Msg ctor : %d : %d ", size, _capacity);
	_signal = SIG_IDLE;
	_src = 0;
}

 bool Msg::is(const void* src, Signal signal) {
	if ((src == 0) || (src == _src)) {
		if ((signal == 0) || (signal == _signal)) {
			return true;
		}
	}
	return false;
}

 bool Msg::is(const void* src, Signal signal, int v) {
	if (src == 0 || src == _src) {
		if (signal == 0 || signal == _signal) {
			rewind();
			int _v;
			if (get(_v) && _v == v) {
				rewind();
				return true;
			}
		}
	}
	return false;
}

 Signal Msg::signal() {
	return _signal;
}

 void* Msg::src() {
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

CborQueue* Msg::_queue = 0;
Msg* __msg;
bool Msg::_init = false;

 bool Msg::init() {
	if (_init)
		return true;
	if (!_queue) {
		_queue = new CborQueue(1024);
		__msg = new Msg(20);
		_init = true;
		return true;
	}
	return false;
}

 Msg& Msg::create(const void* src, Signal signal) {
	clear();
//	INFO("msg capacity : %d ",capacity());
	add((PTR_CAST) src);
	add(signal);
//	INFO("msg create %x : %d ",src,signal);
//	INFO("msg length : %d ",length());
	return *this;
}

extern "C" bool system_os_post(uint8_t prio, uint32_t p1, uint32_t par);

#define MSG_TASK_PRIO      1

 void Msg::wakeup(){
#ifdef __ESP8266__
	system_os_post((uint8_t) MSG_TASK_PRIO, 0, 0);
#endif
}

 Msg& Msg::send() {
	_queue->put(*this);
	wakeup();
//	INFO(" send %d bytes ",_size);
	clear();
	return *this;
}

  CborQueue& Msg::queue(){
		return *_queue;
	}

 Erc Msg::publish(const void* src, Signal signal) {
	init();
	Erc erc = _queue->putf("uu", src, signal);
	wakeup();
	return erc;
}

 Erc Msg::publish(const void* src, Signal signal, int par) {
	init();
	Erc erc =_queue->putf("uui", src, signal, par);
	wakeup();
	return erc;
}

 bool Msg::receive() {
	if (_queue->hasData()) {
		if (_queue->get(*this) == E_OK) {
			get((PTR_CAST &) _src);
			get((int&) _signal);
			_offset = offset();
			return true;
		}
	}
	return false;
}

 Msg& Msg::rewind() {
	offset(_offset);
	return *this;
}

