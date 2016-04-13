//============================================================================
// Name        : AkkaCpp.cpp
// Author      : Lieven Merckx
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <malloc.h>
#include <iostream>
#include <Cbor.h>
#include <stdint.h>
using namespace std;

typedef union {
	uint32_t code;
	uint8_t b[4];
	struct {
		uint8_t dst;
		uint8_t src;
		uint8_t event;
		uint8_t detail;
	};
} Header;

typedef enum {
	TIMEOUT, TXD, RXD, CONNECT, DISCONNECT, CONNECTED, DISCONNECTED
} Event;

// typedef uint8_t ActorRef;

uint8_t masterIdx = 1;
#include <CborQueue.h>
CborQueue cborQueue(1024);
//______________________________________________________
//
class Actor;

//______________________________________________________
//

class ActorRef {
	uint8_t _idx;

public:
	ActorRef(int idx) {
		_idx = idx;
	}
	Actor* actor();

	ActorRef left(ActorRef ref);
	ActorRef right(ActorRef ref);
	ActorRef right(int i);
	ActorRef sender(ActorRef sender);
	ActorRef left();
	ActorRef right();
	ActorRef operator>>(ActorRef dst);

	bool equals(ActorRef& ref) {
		return _idx == ref._idx;
	}

	ActorRef tell(ActorRef src, Event event, int detail = 0);
	ActorRef tell(ActorRef src, Event event, int detail, Cbor& data);
	ActorRef tell(ActorRef src, Event event, const char* format, ...);
	void forward(ActorRef src, Event event, Cbor& data);
};

//______________________________________________________
//

//______________________________________________________
//
#define MAX_ACTORS 255
class Actor {
	uint8_t _idx;
	ActorRef _self;
	ActorRef _left;
	ActorRef _right;
	ActorRef _sender;
	uint64_t _timeout;
	const char* _path;
	static ActorRef deadLetterActorRef;

	static uint8_t _actorCount;
public:
	static Actor* _actors[MAX_ACTORS];
	static Actor* actor(int i) {
		return _actors[i];
	}

	static ActorRef addActor(Actor* actor) {
		_actors[actor->index()] = actor;
		return *new ActorRef(_actorCount);
	}

	Actor(const char* path) :
			_self(_actorCount++), _left(deadLetterActorRef), _right(
					deadLetterActorRef), _sender(deadLetterActorRef) {
		_idx = _self.actor()->_idx;
		_path = path;
		_left = _right = deadLetterActorRef;
		_timeout = UINT64_MAX;
	}

	const char* path() {
		return _path;
	}

	static ActorRef create(const char* path) {
		return addActor(new Actor(path));
	}

	ActorRef self() {
		return _self;
	}

	int index() {
		return _idx;
	}

	ActorRef sender() {
		return _sender;
	}

	void sender(ActorRef sender) {
		_sender = sender;
	}

	virtual void onReceive(Event event, int detail, Cbor& cbor){

	}
	 ~Actor() {
	}
	void unhandled(Event event, int detail, Cbor& cbor) {
		deadLetterActorRef.actor()->onReceive(event, detail, cbor);
	}
	void timeout(uint32_t msec) {
		_timeout = Sys::millis() + msec;
	}

};
uint8_t Actor::_actorCount=0;
Actor* Actor::_actors[MAX_ACTORS];
//______________________________________________________
//
Actor* ActorRef::actor(){
	return Actor::actor(_idx);
}
//______________________________________________________
//
class DeadLetterActor: public Actor {
public:
	DeadLetterActor() :
			Actor("DeadLetterActor") {
		Actor::_actors[0] = this;

	}
	void onReceive(Event event, int detail, Bytes& bytes) {
		cout << " Null Actor received event " << event;
		cout << " from sender : " << sender().actor()->path() << endl;

	}
};
ActorRef Actor::deadLetterActorRef = *new ActorRef(0);
//______________________________________________________
//
class Router {
public:
	Router& addDest(ActorRef& ref);
	void route(ActorRef& src, Event event, Cbor& data);
};

ActorRef ActorRef::tell(ActorRef dst, Event event, int detail, Cbor& bytes) {
	union {
		struct {
			uint8_t dst;
			uint8_t src;
			uint8_t event;
			uint8_t detail;
		};
		uint32_t l;
	} w;
	w.dst=dst._idx;
	w.src=_idx;
	w.event=event;
	w.detail=detail;
	cborQueue.putf("UB",w,&bytes);
return *this;
}
//______________________________________________________
//
/*
 #define MAX_RIGHTS 4
 class Router: public ActorInterface {
 ActorRef* _rights[MAX_RIGHTS];
 ActorRef _left;
 Router() {

 }
 void onReceive() {
 if (sender() == _left) {
 for(int i=0;i<MAX_RIGHTS);i++
 )
 if (_rights[i] != 0)
 _rights[i]->onreceive();
 } else {
 for (int i = 0; i < MAX_RIGHTS; i++)
 if (sender() == _rights[i]) {
 _left->setSender(sender())
 _left->onReceive();
 return;
 }
 deadLetterActor->onReceive();
 }
 }
 ActorRef left() {
 return _left;
 }
 ActorRef right(int i) {
 return _rights[i];
 }
 };
 */
class ChildActor: public Actor {
	ChildActor() :
			Actor("child") {

	}
public:
	static ActorRef create() {
		return addActor(new ChildActor());
	}
	void onReceive(Event event,int detail,Cbor& cbor){

	}
};

int main() {
	ActorRef child = ChildActor::create();

	Cbor cbor;

	child.tell(child, TXD, 0,cbor);

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	return 0;
}
