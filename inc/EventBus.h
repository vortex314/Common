#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <Actor.h>
#include <CborQueue.h>

#define H1(s,i,x)   (x*65599u+(uint8_t)s[(i)<sizeof(s)?sizeof(s)-1-(i):sizeof(s)])
#define H4(s,i,x)   H1(s,i,H1(s,i+1,H1(s,i+2,H1(s,i+3,x))))
#define H16(s,i,x)  H4(s,i,H4(s,i+4,H4(s,i+8,H4(s,i+12,x))))
#define H64(s,i,x)  H16(s,i,H16(s,i+16,H16(s,i+32,H16(s,i+48,x))))
#define H256(s,i,x) H64(s,i,H64(s,i+64,H64(s,i+128,H64(s,i+192,x))))
#define HASH(s)    (uint16_t)((uint32_t)(H256(s,0,0)^(H256(s,0,0)>>16)))
#define H_OLD(s)    (uint16_t)((uint32_t)(H256(s,0,0)^(H256(s,0,0)>>16)))

uint16_t constexpr H(char const *input) {
	return *input ? static_cast<uint16_t>(*input) + 33 * H(input + 1) : 5381;
}

typedef void (Actor::*MethodHandler)(Cbor&);
typedef void (*StaticHandler)(Cbor&);
#define CALL_MEMBER_FUNC(object,ptrToMember)  ((*object).*(ptrToMember))

typedef struct
{
    uint32_t header;
    Actor* actor;
    union
    {
        StaticHandler staticHandler;
        MethodHandler methodHandler;
    };
} HandlerEntry2;


class EventBus
{
private:
    CborQueue _queue;
    HandlerEntry2 subscribers[100];
    uint32_t subscriberCount;
    bool _debug;
public:
    EventBus(uint32_t size) ;
    void publish(uint32_t header,Cbor& cbor);
    void publish(uint32_t header);
    void subscribe(uint32_t header,Actor* instance,MethodHandler handler);
    void subscribe(uint32_t header,StaticHandler handler);
    void eventLoop();
    void debug(bool on) { _debug=on;}
};

extern EventBus eb;

#endif // EVENTBUS_H
