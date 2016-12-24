#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <Actor.h>
#include <CborQueue.h>

constexpr uint64_t fnv1(uint64_t h, const char* s)
{
    return (*s == 0) ?
           h : fnv1((h * 1099511628211ull) ^ static_cast<uint64_t>(*s), s + 1);
}

constexpr uint16_t H(const char* s)
{
    return fnv1(14695981039346656037ull, s) & 0xFFFF;
}
/*
 * **
 * ----- not good method -- makes compiler slow or run out of memory
 */
/*
#define H1(s,i,x)   (x*65599u+(uint8_t)s[(i)<sizeof(s)?sizeof(s)-1-(i):sizeof(s)])
#define H4(s,i,x)   H1(s,i,H1(s,i+1,H1(s,i+2,H1(s,i+3,x))))
#define H16(s,i,x)  H4(s,i,H4(s,i+4,H4(s,i+8,H4(s,i+12,x))))
#define H64(s,i,x)  H16(s,i,H16(s,i+16,H16(s,i+32,H16(s,i+48,x))))
#define H256(s,i,x) H64(s,i,H64(s,i+64,H64(s,i+128,H64(s,i+192,x))))
#define HASH(s)    (uint16_t)((uint32_t)(H256(s,0,0)^(H256(s,0,0)>>16)))
#define H_OLD(s)    (uint16_t)((uint32_t)(H256(s,0,0)^(H256(s,0,0)>>16)))

//#define H(__s__) HASH(__s__)
*/

/** doesn't always precompile **/
/*
uint16_t constexpr HH(char const *input) {
	return *input ? static_cast<uint16_t>(*input) + 33 * HH(input + 1) : 5381;
}
*/
typedef void (Actor::*MethodHandler)(Cbor&);
typedef void (*StaticHandler)(Cbor&);
#define CALL_MEMBER_FUNC(object,ptrToMember)  ((*object).*(ptrToMember))

class Subscriber
{
public:
    Actor* actor;
    union
    {
        StaticHandler staticHandler;
        MethodHandler methodHandler;
    };
    Subscriber* nextSubscriber;
    Subscriber();
};

class EventFilter
{
public:
    uint32_t filter;
    Subscriber* firstSubscriber;
    EventFilter* nextFilter;
    EventFilter(uint32_t header);
} ;

class EventBus
{
private:
    CborQueue _queue;
    EventFilter* _firstFilter;

    EventFilter* firstFilter();
    EventFilter* nextFilter(EventFilter* current);
    EventFilter* findFilter(uint32_t header);
    EventFilter* lastFilter();
    void invokeAllSubscriber(Cbor& ,EventFilter* filter);

    Subscriber* firstSubscriber(EventFilter* filter);
    Subscriber* nextSubScriber(Subscriber* current);
    Subscriber* lastSubscriber(EventFilter* filter);
    Subscriber* addSubscriber(uint32_t header);

public:
    EventBus(uint32_t size);
    Erc initAll();
    void publish(uint16_t header, Cbor& cbor);
    void publish(uint16_t header);
    void publish(Cbor& cbor);
    void subscribe(uint16_t header, Actor* instance, MethodHandler handler);
    void subscribe(uint16_t header, StaticHandler handler);
    void subscribe(Actor* actor);
    void eventLoop();
    Cbor& request(uint16_t dst,uint16_t src,uint16_t req);
    Cbor& reply(uint16_t dst,uint16_t src,uint16_t repl);
    Cbor& event(uint16_t ev,uint16_t src);
    Cbor& data();                  //  eb.request(H("mqtt"),H("connect"),H("motor")).addKeyValue(H("host"),"test.mosquitto.org");eb.send();
    EventBus& send();

};

extern EventBus eb;

#endif // EVENTBUS_H
