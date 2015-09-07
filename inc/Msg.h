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
	SIG_INIT , SIG_IDLE , SIG_ERC ,
//	SIG_TIMEOUT = 1 << 3,
	SIG_TICK ,
	SIG_CONNECTED ,
	SIG_DISCONNECTED ,
	SIG_RXD ,
	SIG_TXD ,
	SIG_START ,
	SIG_STOP ,
	SIG_DO

} Signal;

#ifdef __cplusplus
class Handler;

#define OS_UART1 (Handler*)1
#define OS_CLOCK (Handler*)2
#else
#define OS_UART1 (void*)1
#define OS_CLOCK (void*)2
#endif

#ifdef __cplusplus
#include "Bytes.h"
#include "BipBuffer.h"



class Msg {
public:
	Handler* src;
	Signal signal;
	int param;
	void* data;
//	Msg();
	bool is(Handler * src, int sigMask, int param, void* data);
	bool is(Handler * src, int sigMask);
	bool is(Handler * src, Signal signal);
	Signal sig();
};

class MsgQueue {
private:
	static BipBuffer bb;
public:

public:
	static void publish(Msg& msg);
	static void publishFromIsr(Msg& msg);
	static void publish(Handler* src, Signal signal);
	static void publish(Handler* src, Signal signal, int param, void* data);
	static bool get(Msg& msg);
};
#endif

#endif /* SIG_H_ */
