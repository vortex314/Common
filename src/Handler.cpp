/*
 * Handler.cpp
 *
 *  Created on: 20-dec.-2014
 *      Author: lieven2
 */
#define __STDC_LIMIT_MACROS
#include "Handler.h"
#include "Sys.h"
#include <Logger.h>
#include "Msg.h"
Handler* Handler::_firstChild = 0;
/* Handler::Handler() {
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
	unreg(this);
}

 void Handler::timeout(uint32_t msec) {
	_timeout = Sys::millis() + msec;
}

 bool Handler::timeout() {
	return _timeout < Sys::millis();
}

 uint64_t Handler::getTimeout() {
	return _timeout;
}

 const char* Handler::getName() {
	return _name;
}
//_________________________________________________________________________________________________
//
//       HANDLER LIST
//_________________________________________________________________________________________________
 Handler* Handler::first() {
	return _firstChild;
}

 Handler* Handler::next() {
	return _next;
}

 void Handler::reg(Handler* hdlr) {
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

 void Handler::unreg(Handler* hdlr) {
	if (_firstChild == hdlr)
		_firstChild = hdlr->_next;
	else {
		Handler* cursor = _firstChild;
		while (cursor->_next != 0) {
			if (cursor->_next == hdlr) {
				cursor->_next = hdlr->_next;
				break;
			}
			cursor = cursor->_next;
		}
	}
}

//_________________________________________________________________________________________________
//
//       LISTENER LIST
//_________________________________________________________________________________________________
//Handler* hdlr;

extern "C" void uart0WriteWait(uint8_t TxChar);

 void Handler::dispatchToChilds(Msg& msg) {
	Handler* hdlr;
	if (first() == 0)
		ERROR(" no handlers ");
	for (hdlr = first(); hdlr != 0; hdlr = hdlr->next()) {
		msg.rewind();
//	INFO("%s dispatch",hdlr->getName());
		if (hdlr->isRunning()) {
			uart0WriteWait(hdlr->_name[0]);
			uart0WriteWait(hdlr->_name[1]);
			uart0WriteWait(hdlr->_name[2]);
			uart0WriteWait('\r');
			uart0WriteWait('\n');
			hdlr->dispatch(msg);
		}
	}
}

extern "C"  int HandlerTimeouts() {
	return Handler::timeouts();
}

// return true if any handler has a timeout
 bool Handler::timeouts() {
	Handler* hdlr;
	if (first() == 0)
		ERROR(" no handlers ");
	for (hdlr = first(); hdlr != 0; hdlr = hdlr->next()) {
		if (hdlr->timeout())
			return true;
	}
	return false;
}
