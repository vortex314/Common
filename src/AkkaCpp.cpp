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

// typedef uint8_t Actor&;

uint8_t masterIdx = 1;
#include <CborQueue.h>
CborQueue cborQueue(1024);
//______________________________________________________
//
class Actor;

//______________________________________________________
//
/*
class Actor {
	uint8_t _idx;

public:
	Actor(int idx) {
		_idx = idx;
	}
	Actor* actor();

	Actor& left(Actor& ref);
	Actor& right(Actor& ref);
	Actor& right(int i);
	Actor& sender(Actor& sender);
	Actor& left();
	Actor& right();
	Actor& operator>>(Actor& dst);

	bool equals(Actor&& ref) {
		return _idx == ref._idx;
	}

	Actor& tell(Actor& src, Event event, int detail = 0);
	Actor& tell(Actor& src, Event event, int detail, Cbor& data);
	Actor& tell(Actor& src, Event event, const char* format, ...);
	void forward(Actor& src, Event event, Cbor& data);
};*/

//______________________________________________________
//

//______________________________________________________
//
#define MAX_ACTORS 255
class Actor {
	uint8_t _idx;
	Actor& _self;
	Actor& _left;
	Actor& _right;
	Actor& _sender;
	uint64_t _timeout;
	const char* _path;
	static Actor& deadLetterActor;

	static uint8_t _actorCount;
public:
	static Actor* _actors[MAX_ACTORS];
	static Actor* actor(int i) {
		return _actors[i];
	}


	Actor(const char* path) :
			_self(*this), _left(deadLetterActor), _right(
					deadLetterActor), _sender(deadLetterActor) {
		_idx=_actorCount++;
		_actors[_idx]=this;
		_path = path;
		_left = _right = deadLetterActor;
		_timeout = UINT64_MAX;
	}
	bool equals(Actor& ref) {
		return _idx == ref._idx;
	}

	Actor& tell(Actor& src, Event event, int detail = 0);
	Actor& tell(Actor& src, Event event, int detail, Cbor& data);
	Actor& tell(Actor& src, Event event, const char* format, ...);
	void forward(Actor& src, Event event, Cbor& data);
	const char* path() {
		return _path;
	}

	static Actor& create(const char* path) {
		return addActor(new Actor(path));
	}

	Actor& self() {
		return _self;
	}

	int index() {
		return _idx;
	}

	Actor& sender() {
		return _sender;
	}

	void sender(Actor& sender) {
		_sender = sender;
	}

	virtual void onReceive(Event event, int detail, Cbor& cbor) {

	}
	~Actor() {
		_actors[_idx] = deadLetterActor;
	}
	void unhandled(Event event, int detail, Cbor& cbor) {
		deadLetterActor.onReceive(event, detail, cbor);
	}
	void timeout(uint32_t msec) {
		_timeout = Sys::millis() + msec;
	}

};
uint8_t Actor::_actorCount = 1;
Actor* Actor::_actors[MAX_ACTORS];
//______________________________________________________
//

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
		cout << " from sender : " << sender().path() << endl;

	}
};
Actor& Actor::deadLetterActor = *new DeadLetterActor();
//______________________________________________________
//


Actor& Actor::tell(Actor& dst, Event event, int detail, Cbor& bytes) {
	union {
		struct {
			uint8_t dst;
			uint8_t src;
			uint8_t event;
			uint8_t detail;
		};
		uint32_t l;
	} w;
	w.dst = dst._idx;
	w.src = _idx;
	w.event = event;
	w.detail = detail;
	cborQueue.putf("UB", w, &bytes);
	return *this;
}
//______________________________________________________
//

#define MAX_RIGHTS 4
class Router: public Actor {
	Actor&* _rights[MAX_RIGHTS];
	Actor& _left;
	Router() {

	}
	void onReceive() {
		if (sender() == left()) {
			for (int i = 0; i < MAX_RIGHTS; i++)
				if (_rights[i] != 0) {
					_rights->sender(sender());
					_rights[i]->onReceive();
				}
		} else if ( sender()==right()){
			for (int i = 0; i < MAX_RIGHTS; i++)
				if (sender() == right(i)) {
					_left->setSender(sender())
					_left->onReceive();
					return;
				}
			deadLetterActor->onReceive();
		}
	}
	Actor& left() {
		return _left;
	}
	Actor& right(int i) {
		return _rights[i];
	}
};

class ChildActor: public Actor {
	ChildActor() :
			Actor("child") {

	}
public:
	static Actor& create() {
		return addActor(new ChildActor());
	}
	void onReceive(Event event, int detail, Cbor& cbor) {

	}
};

int main() {
	Actor& child = ChildActor::create();

	Cbor cbor;

	child.tell(child, TXD, 0, cbor);
	child >> child >> child;

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	return 0;
}
