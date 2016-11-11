/*
 * Actor.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: lieven
 */

#include "Actor.h"

Actor* Actor::_first = 0;
#ifndef UINT_LEAST64_MAX
#define UINT_LEAST64_MAX 0xFFFFFFFFFFFFFFFFL
#endif // UINT_LEAST64_MAX

Actor::Actor(const char* name) {
	_timeout = UINT_LEAST64_MAX;
	_name = name;
	_state = 0;
	_ptLine = 0;
	_next = 0;
	if (first() == 0) {
		setFirst(this);
	} else {
		last()->setNext(this);
	}
}

Actor* Actor::last() {
	Actor* cursor = first();
	while (cursor->_next) {
		cursor = cursor->next();
	}
	return cursor;
}

Actor* Actor::first() {
	return Actor::_first;
}
Actor* Actor::next() {
	return _next;
}

void Actor::setNext(Actor* a) {
	_next = a;
}

void Actor::setFirst(Actor* f) {
	_first = f;
}

Actor::~Actor() {
}

void Actor::onEvent(Cbor& cbor) {
	ASSERT(false); // empty actor ?
}

