#include "EventBus.h"

Cbor timeoutEvent(6);

EventBus::EventBus(uint32_t size) :
		_queue(size), _debug(false) {
	subscriberCount = 0;
	timeoutEvent.addKeyValue(0, H("timeout"));
	publish(H("setup"));
}

void EventBus::publish(uint16_t header, Cbor& cbor) {
	Cbor msg(0);
	_queue.putMap(msg);
	msg.addKey(0).add(header);
	msg.append(cbor);
	_queue.putRelease(msg);
}

void EventBus::publish(Cbor& cbor) {
	Cbor msg(0);
	_queue.putMap(msg);
	msg.append(cbor);
	_queue.putRelease(msg);
}

void EventBus::publish(uint16_t header) {
	Cbor msg(0);
	_queue.putMap(msg);
	msg.addKey(0).add(header);
	_queue.putRelease(msg);
}
#ifdef __linux__
extern const char* hash2string(uint32_t hash);
void logCbor(Cbor& cbor) {
	Str str(2048);
	/*    str.clear();
	 cbor.toString(str);
	 LOGF("--message : %s ",str.c_str());*/
	cbor.offset(0);
	uint32_t key;
	str.clear();
	Cbor::PackType ct;
	cbor.offset(0);
	while (cbor.hasData()) {
		cbor.get(key);
		str.append('"').append(hash2string(key)).append("\"[").append(key).append("]:");
		if (key == 0) {
			cbor.get(key);
			str.append('"').append(hash2string(key)).append("\"[").append(key).append("]:");
		} else {
			ct = cbor.tokenToString(str);
			if (ct == Cbor::P_BREAK || ct == Cbor::P_ERROR)
				break;
		}
		if (cbor.hasData())
			str << ",";
	};
	LOGF("--- %s", str.c_str());
	/*
	 while ( cbor.hasData())
	 {
	 if ( cbor.get(key))
	 {
	 LOGF("---key : %d:%s ",key,hash2string(key));

	 }
	 else break;
	 if ( key==0 && cbor.get(key))
	 {
	 LOGF("----value : %d:%s ",key,hash2string(key));
	 }
	 else
	 cbor.skipToken();
	 }*/
}
#endif

extern void usart_send_string(const char *s);

void EventBus::eventLoop() {
	Cbor cbor(0);

	uint32_t header = 0;
	while ((_queue.getMap(cbor) == 0) && cbor.getKeyValue((uint16_t) 0, header)) {
#ifdef __linux__
		if (_debug)
			logCbor(cbor);
#endif
		for (uint32_t i = 0; i < subscriberCount; i++) {
			//           LOGF("%d %d == %d",i,subscribers[i].header , header);
			if (subscribers[i].header == header || subscribers[i].header == 0) {
				if (subscribers[i].actor == 0) {
					subscribers[i].staticHandler(cbor);
				} else {
					if (subscribers[i].methodHandler == 0)
						subscribers[i].actor->onEvent(cbor);
					else
						CALL_MEMBER_FUNC(subscribers[i].actor,subscribers[i].methodHandler)(
								cbor);
				}
			}
		}
		_queue.getRelease(cbor);
	}

	for (Actor* actor = Actor::first(); actor; actor = actor->next()) {
		if (actor->timeout())
			actor->onEvent(timeoutEvent);
	}
}

void EventBus::subscribe(uint16_t header, Actor* instance,
		MethodHandler handler) {
	subscribers[subscriberCount].header = header;
	subscribers[subscriberCount].actor = instance;
	subscribers[subscriberCount].methodHandler = handler;
	subscriberCount++;
}
void EventBus::subscribe(uint16_t header, StaticHandler handler) {
	subscribers[subscriberCount].header = header;
	subscribers[subscriberCount].staticHandler = handler;
	subscribers[subscriberCount].actor = 0;
	subscriberCount++;
}
void EventBus::subscribe(Actor* instance) {
	subscribe(0, instance, 0);
}


