/*
 * Actor.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: lieven
 */

#include "Actor.h"

Actor* Actor::_first=0;

Actor::Actor(const char* name) {
	_timeout = UINT_LEAST64_MAX;
	_name = name;
	_state = 0;
	_ptLine = 0;
	_next = 0;
	if (_first == 0) {
		_first = this;
	} else {
		findLast()->_next = this;
	}
}


Actor* Actor::findLast() {
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


Actor::~Actor() {
}

void Actor::onEvent(Cbor& cbor) {
}

void Actor::setupAll(){
	for (Actor* actor = Actor::first(); actor; actor = actor->next()) {
			actor->setup();
		}
}

void Actor::setup(){
}


