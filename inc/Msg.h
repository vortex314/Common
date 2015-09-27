/*
 * Msg.h
 *
 *  Created on: 12-sep.-2014
 *      Author: lieven2
 */

#ifndef SIG_H_
#define SIG_H_

//#include "Event.h"

typedef enum Signal {
	SIG_INIT, SIG_IDLE, SIG_ERC,
	SIG_TICK,
	SIG_CONNECTED,
	SIG_DISCONNECTED,
	SIG_RXD,
	SIG_TXD,
	SIG_START,
	SIG_STOP,
	SIG_DO

} Signal;

extern
const char* strSignal[] ;

#ifdef __cplusplus
class Handler;
#include "Bytes.h"
#include "BipBuffer.h"

#include "Cbor.h"

class Msg: public Cbor {
public:
	static BipBuffer* _bb;
	static bool _init;
public:
	void* _src;
	Signal _signal;
	uint8_t* _start;
	uint32_t _size; // includes 2 first bytes length
	uint32_t _offset;

	Msg(uint32_t size);
	bool is(void * src, Signal signal);
	bool is(void * src, Signal signal, int v1);
	static bool init();
	Msg& create(void * src, Signal signal);
	static void publish(void * src, Signal signal);
	static void publish(void * src, Signal signal, int par);
	Msg& rewind();
	Msg& send();
	bool receive();
	Msg& free();
	Signal signal();
	void* src();
};

#endif

#endif /* SIG_H_ */
