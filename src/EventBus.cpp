#include "EventBus.h"




EventBus::EventBus(uint32_t size) : _queue(size)
{
}
void EventBus::publish(uint32_t header,Cbor& cbor)
{
    for(uint32_t i=0; i<subscriberCount; i++)
    {
        LOGF("%d %d == %d",i,subscribers[i].header , header);
        if ( subscribers[i].header == header)
        {
            if ( subscribers[i].actor == 0)
            {
                subscribers[i].staticHandler(header,cbor);
            }
        }
    }
}
void EventBus::publish(uint32_t header)
{
    for(uint32_t i=0; i<subscriberCount; i++)
    {
        LOGF("%d %d == %d",i,subscribers[i].header , header);
        if ( subscribers[i].header == header)
        {
            if ( subscribers[i].actor == 0)
            {
                Cbor cbor(1);
                subscribers[i].staticHandler(header,cbor);
            }
        }
    }
}
void EventBus::subscribe(uint32_t header,Actor* instance,MethodHandler handler)
{
}
void EventBus::subscribe(uint32_t header,StaticHandler handler)
{
    subscribers[subscriberCount].header =  header;
    subscribers[subscriberCount].staticHandler =  handler;
    subscribers[subscriberCount].actor =  0;
    subscriberCount++;
}


EventBus eb(10240);
