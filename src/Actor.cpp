/*
 * Actor.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: lieven
 */

#include "Actor.h"

Actor::Actor(const char* name) {
	_timeout = UINT_LEAST64_MAX;
	_name = name;
	_state = 0;
	_ptLine = LineNumberInvalid;
//	LOGF(" Actor ctor : %s [%d]", _name, _id);
}

Actor::~Actor() {
}

void Actor::onEvent(Cbor& cbor) {
}

void Actor::setup(){
}

