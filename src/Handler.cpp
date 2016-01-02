/*
 * Handler.cpp
 *
 *  Created on: 20-dec.-2014
 *      Author: lieven2
 */
#define __STDC_LIMIT_MACROS
#include "Handler.h"
#include "Sys.h"
#include "Msg.h"
Handler* Handler::_firstChild = 0;
/*IROM Handler::Handler() {
 _timeout = UINT64_MAX;
 _name = "UNDEFINED";
 _next = 0;
 //      _firstChild = 0;
 restart();
 reg(this);
 }*/

Handler::Handler(const char* name) {
	_timeout = UINT64_MAX;
	strncpy(_name, name, sizeof(_name));
	_next = 0;
//      _firstChild = 0;
	restart();
	reg(this);
}

Handler::~Handler() {
//		unreg(this);
}

IROM void Handler::timeout(uint32_t msec) {
	_timeout = Sys::millis() + msec;
}

IROM bool Handler::timeout() {
	return _timeout < Sys::millis();
}

IROM uint64_t Handler::getTimeout() {
	return _timeout;
}

IROM const char* Handler::getName() {
	return _name;
}
//_________________________________________________________________________________________________
//
//       HANDLER LIST
//_________________________________________________________________________________________________
IROM Handler* Handler::first() {
	return _firstChild;
}

IROM Handler* Handler::next() {
	return _next;
}

IROM void Handler::reg(Handler* hdlr) {
	if (_firstChild == 0)
		_firstChild = hdlr;
	else {
		Handler* cursor = _firstChild;
		while (cursor->_next != 0) {
			cursor = cursor->_next;
		}
		cursor->_next = hdlr;
	}
}

IROM void Handler::unreg(Handler* hdlr) {
	Handler* prev;
	INFO("");
	prev = _firstChild;	// let's hope it is not the first

	while (prev != 0) {
		if (prev->_next == hdlr) {
			prev->_next = prev->_next->_next;
			break;
		}
		prev = prev->_next;
	}
}

//_________________________________________________________________________________________________
//
//       LISTENER LIST
//_________________________________________________________________________________________________
//Handler* hdlr;
IROM void Handler::dispatchToChilds(Msg& msg) {
	Handler* hdlr;
	if (first() == 0)
		ERROR(" no handlers ");
	for (hdlr = first(); hdlr != 0; hdlr = hdlr->next()) {
		msg.rewind();
//	INFO("%s dispatch",hdlr->getName());
		if (hdlr->isRunning())
			hdlr->dispatch(msg);
	}
}

extern "C" IROM int HandlerTimeouts() {
	return Handler::timeouts();
}

// return true if any handler has a timeout
IROM bool Handler::timeouts() {
	Handler* hdlr;
	if (first() == 0)
		ERROR(" no handlers ");
	for (hdlr = first(); hdlr != 0; hdlr = hdlr->next()) {
		if (hdlr->timeout())
			return true;
	}
	return false;
}
