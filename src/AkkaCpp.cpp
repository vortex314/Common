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
	TIMEOUT,
	STOP,
	RESTART,
	INIT,
	TXD,
	RXD,
	CONNECT,
	DISCONNECT,
	CONNECTED,
	DISCONNECTED,
} Event;
typedef enum {
	DEAD_LETTER, BROADCAST
};

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
			_self(*this), _left(deadLetterActor), _right(deadLetterActor), _sender(
					deadLetterActor) {
		_idx = _actorCount++;
		_actors[_idx] = this;
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

	Actor& self() {
		return _self;
	}

	Actor& right(Actor& r) {
		_right = r;
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
		deadLetterActor.onReceive(event, detail, cbor);
	}
	virtual void onInit() {

	}
	~Actor() {
		_actors[_idx] = deadLetterActor;
		Logger::logger << " Deleted Actor : " + path() + "." + _idx;
	}
	void unhandled(Event event, int detail, Cbor& cbor) {
		deadLetterActor.onReceive(event, detail, cbor);
	}
	void timeout(uint32_t msec) {
		_timeout = Sys::millis() + msec;
	}
	bool timeout() {
		return _timeout > Sys::millis();
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
	Actor& _rights[MAX_RIGHTS];
	Actor& _left;
	Router() {
		_left = deadLetterActor;
		for (int i = 0; i < MAX_RIGHTS; i++)
			_rights[i] = deadLetterActor;
	}

	void onReceive(Event event, int detail, Cbor& cbor) {
		if (sender() == left()) {
			for (int i = 0; i < MAX_RIGHTS; i++)
				if (_rights[i] != 0) {
					_rights[i].sender(sender());
					_rights[i].onReceive(event, detail, cbor);
				}
		} else {
			for (int i = 0; i < MAX_RIGHTS; i++)
				if (sender() == right(i)) {
					_left.sender(sender());
					_left.onReceive(event, detail, cbor);
					return;
				}
			deadLetterActor.onReceive(event, detail, cbor);
		}
	}
	Actor& left() {
		return _left;
	}
	Actor& right(int i) {
		return _rights[i];
	}
};

void EventLoop() {
	while (true) {
		while (cborQueue.hasData()) {
			Cbor cbor();
			Header h;
			cborQueue.get(cbor);
			cbor.get(h.l);
			if (h.dst > Actor::_actorCount) {

			} else if (h.dst == BROADCAST) {
			} else {
				Actor::_actors[h.dst]->sender(*Actor::_actors[h.src]);
				Actor::_actors[h.dst]->onReceive((Event) h.event,
						(int) h.detail, cbor);
			}
		}
		for (int i = 0; i < Actor::_actorCount; i++) {
			Actor* actor = Actor::_actors[i];
			if (actor == 0)
				break;
			else {
				if (actor->timeout()) {
					Cbor& cbor(0);
					Actor::deadLetterActor.tell(*actor, TIMEOUT, 0, cbor);
				}
			}
		}
	}
}

class ChildActor: public Actor {
	Actor& _parent;
public:
	ChildActor(Actor& parent) :
			Actor("child") {
		_parent = parent;
	}
	~ChildActor() {

	}
	void onReceive(Event event, int detail, Cbor& cbor) {
		if (event == TXD) {
			_parent.tell(self(), TXD, 0, cbor)
		}
	}
};
class ParentActor: public Actor {
public:
	ParentActor() :
			Actor("child") {

	}
	~ParentActor() {

	}
	void onReceive(Event event, int detail, Cbor& cbor) {

	}
};

int main() {
	Actor& child = ChildActor();

	Cbor cbor;

	child.tell(child, TXD, 0, cbor);
	child >> child >> child;

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	return 0;
}
