#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <Actor.h>
#include <CborQueue.h>

uint16_t constexpr H(char const *input)
{
    return *input ? static_cast<uint16_t>(*input) + 33 * H(input + 1) : 5381;
}

typedef void (Actor::*MethodHandler)(uint32_t,Cbor&);
typedef void (*StaticHandler)(uint32_t,Cbor&);
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

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
public:
    EventBus(uint32_t size) ;
    void publish(uint32_t header,Cbor& cbor);
    void publish(uint32_t header);
    void subscribe(uint32_t header,Actor* instance,MethodHandler handler);
    void subscribe(uint32_t header,StaticHandler handler);
};

extern EventBus eb;

#endif // EVENTBUS_H
