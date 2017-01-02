#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <Actor.h>
#include <CborQueue.h>

#define FNV_PRIME  16777619
#define FNV_PRIME_64 1099511628211ull
#define FNV_OFFSET_64 14695981039346656037ull
#define FNV_OFFSET 2166136261

constexpr uint32_t fnv1(uint32_t h, const char* s) {
    return (*s == 0) ?
           h : fnv1((h * FNV_PRIME) ^ static_cast<uint32_t>(*s), s + 1);
}

constexpr uint16_t H(const char* s) {
//    uint32_t  h = fnv1(FNV_OFFSET, s) ;
    return (fnv1(FNV_OFFSET, s) & 0xFFFF);
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

class Subscriber {
public:
    Actor* _actor;
    union {
        StaticHandler _staticHandler;
        MethodHandler _methodHandler;
    };
    Subscriber* _nextSubscriber;
    Subscriber();
    Subscriber* next();
};

class EventFilter {

public:
    typedef enum  { EF_ANY,EF_REQUEST,EF_EVENT,EF_REPLY,EF_KV } type;
    type _type;
    uint16_t _object;
    uint16_t _value;

    Subscriber* _firstSubscriber;
    EventFilter* _nextFilter;
    EventFilter(type t,uint16_t o,uint16_t v);
    Subscriber* firstSubscriber();
    Subscriber* lastSubscriber();
    Subscriber* addSubscriber();
    void subscribe( Actor* instance, MethodHandler handler);
    void subscribe( StaticHandler handler);
    void subscribe(Actor* actor);
    void invokeAllSubscriber(Cbor& );

    EventFilter* next();
    bool match(Cbor& cbor);

    static bool isEvent(Cbor& cbor,uint16_t src,uint16_t ev);
    static bool isRequest(Cbor& cbor,uint16_t dst,uint16_t req);
    static bool isReply(Cbor& cbor,uint16_t src,uint16_t req);
    static bool isReplyCorrect(Cbor& cbor,uint16_t src,uint16_t req);

} ;

#define EB_DST H("dst")
#define EB_SRC H("src")
#define EB_REQUEST H("request")
#define EB_REPLY H("reply")
#define EB_EVENT H("event")
#define EB_ERROR H("error")
#define EB_REGISTER H("register")

class EventBus {
private:
    CborQueue _queue;
    EventFilter* _firstFilter;

    EventFilter* firstFilter();
    EventFilter* findFilter(EventFilter::type ,uint16_t o,uint16_t v);
    EventFilter* lastFilter();
    EventFilter& addFilter(EventFilter::type ,uint16_t o,uint16_t v);



    Cbor _txd;
    Cbor _rxd;

public:
    EventBus(uint32_t size,uint32_t msgSize);
    Erc initAll();
    bool match(uint32_t header,uint16_t dst,uint16_t src,uint16_t op);

    void publish(uint16_t header, Cbor& cbor);
    void publish(uint16_t src,uint16_t event);
    void publish(Cbor& cbor);

    EventFilter& filter(uint16_t key,uint16_t value);
    EventFilter& onRequest(uint16_t dst);
    EventFilter& onRequest(uint16_t dst,uint16_t req);
    EventFilter& onReply(uint16_t dst,uint16_t repl);
    EventFilter& onEvent(uint16_t src,uint16_t ev);
    EventFilter& onAny();
    EventFilter& onDst(uint16_t dst);
    EventFilter& onSrc(uint16_t src);

    void eventLoop();
//   EventFilter* findFilter(EventFilter::type ,uint16_t o,uint16_t v);

    Cbor& request(uint16_t dst,uint16_t req,uint16_t src);
    Cbor& reply(uint16_t dst,uint16_t repl,uint16_t src);
    Cbor& reply();
    Cbor& event(uint16_t src,uint16_t ev);
    // Cbor& data();                  //  eb.request(H("mqtt"),H("connect"),H("motor")).addKeyValue(H("host"),"test.mosquitto.org");eb.send(); eb.
    bool isEvent(uint16_t ev,uint16_t src);
    bool isRequest(uint16_t dst,uint16_t req);
    bool isReply(uint16_t src,uint16_t req);
    bool isReplyCorrect(uint16_t src,uint16_t req);
    void send();
};

extern EventBus eb;

#endif // EVENTBUS_H
