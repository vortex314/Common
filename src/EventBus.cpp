#include "EventBus.h"

Cbor* timeoutEvent;

EventBus::EventBus(uint32_t size) :
    _queue(size), _firstFilter(0)
{
    timeoutEvent=new Cbor(6);
    timeoutEvent->addKeyValue(0, H("timeout"));
    publish(H("setup"));
}

void EventBus::publish(uint16_t header, Cbor& cbor)
{
    Cbor msg(0);
    _queue.putMap(msg);
    msg.addKey(0).add(header);
    msg.append(cbor);
    _queue.putRelease(msg);
}

void EventBus::publish(Cbor& cbor)
{
    Cbor msg(0);
    _queue.putMap(msg);
    msg.append(cbor);
    _queue.putRelease(msg);
}

void EventBus::publish(uint16_t header)
{
    Cbor msg(0);
    _queue.putMap(msg);
    msg.addKey(0).add(header);
    _queue.putRelease(msg);
}
#ifdef __linux__
extern const char* hash2string(uint32_t hash);
void logCbor(Cbor& cbor)
{
    Str str(2048);
    /*    str.clear();
     cbor.toString(str);
     LOGF("--message : %s ",str.c_str());*/
    cbor.offset(0);
    uint32_t key;
    str.clear();
    Cbor::PackType ct;
    cbor.offset(0);
    while (cbor.hasData())
    {
        cbor.get(key);
        str.append('"').append(hash2string(key)).append("\"[").append(key).append("]:");
        if (key == 0)
        {
            cbor.get(key);
            str.append('"').append(hash2string(key)).append("\"[").append(key).append("]:");
        }
        else
        {
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



void EventBus::invokeAllSubscriber(Cbor& cbor,EventFilter* filter)
{
    for (Subscriber* sub=firstSubscriber(filter); sub != 0; sub=nextSubScriber(sub))
    {
        if (sub->actor == 0)
        {
            sub->staticHandler(cbor);
        }
        else
        {
            if (sub->methodHandler == 0)
                sub->actor->onEvent(cbor);
            else
                CALL_MEMBER_FUNC(sub->actor,sub->methodHandler)(
                    cbor);
        }
    }
}
//____________________________________________________________________
//

Cbor cbor(256);

void EventBus::eventLoop()
{
//	Cbor cbor(0);
    uint32_t header = 0;
//	while ((_queue.getMap(cbor) == 0) && cbor.getKeyValue((uint16_t) 0, header)) {
    while ((_queue.get(cbor) == 0) && cbor.getKeyValue((uint16_t) 0, header)) // handle all events
    {
#ifdef __linux__
        if (_debug)
            logCbor(cbor);
#endif
        EventFilter* filter;
        if ( (filter=findFilter(header)) ) // handle all matching filters
        {
            invokeAllSubscriber(cbor,filter);
        }
        if ( (filter=findFilter(0)) )     // handle all total subscribers
        {
            invokeAllSubscriber(cbor,filter);
        }
    }

    for (Actor* actor = Actor::first(); actor; actor = actor->next()) // handle all actor timeouts
    {
        if (actor->timeout())
        {
            actor->onEvent(*timeoutEvent);
        }
    }
}
//____________________________________________________________________
//
Subscriber* EventBus::addSubscriber(uint32_t header)
{
    EventFilter* cursorFilter;
    Subscriber* cursorSubscriber;
    if ( _firstFilter == 0 )
    {
        _firstFilter=new EventFilter(header);
        cursorFilter=_firstFilter;
    }
    else
    {
        cursorFilter = findFilter(header);
        if ( cursorFilter==0)
        {
            cursorFilter = lastFilter()->nextFilter = new EventFilter(header);
        }
    }
    if ( cursorFilter->firstSubscriber==0)
    {
        cursorSubscriber = cursorFilter->firstSubscriber=new Subscriber();
    }
    else
    {
        cursorSubscriber = lastSubscriber(cursorFilter)->nextSubscriber = new Subscriber();
    }
    return cursorSubscriber;
}
//____________________________________________________________________
//
void EventBus::subscribe(uint16_t header, Actor* instance,
                         MethodHandler handler)
{
    Subscriber* sub = addSubscriber(header);
    sub->actor = instance;
    sub->methodHandler = handler;
}
//____________________________________________________________________
//
void EventBus::subscribe(uint16_t header, StaticHandler handler)
{
    Subscriber* sub = addSubscriber(header);
    sub->staticHandler = handler;
    sub->actor = 0;
}
//____________________________________________________________________
//
void EventBus::subscribe(Actor* instance)
{
    subscribe(0, instance, 0);
}
//____________________________________________________________________
//
EventFilter* EventBus::firstFilter()
{
    return _firstFilter;
}
EventFilter* EventBus::nextFilter(EventFilter* current)
{
    return current->nextFilter;
}
//____________________________________________________________________
//
EventFilter* EventBus::findFilter(uint32_t header)
{
    for(EventFilter* ef=firstFilter(); ef!=0; ef=nextFilter(ef))
    {
        if ( ef->filter == header ) return ef;
    }
    return 0;
}
//____________________________________________________________________
//
EventFilter* EventBus::lastFilter()
{
    for(EventFilter* ef=firstFilter(); ef!=0; ef=nextFilter(ef))
    {
        if ( ef->nextFilter==0 ) return ef;
    }
    ASSERT(false); // shouldn't arrive here
    return 0;
}
//____________________________________________________________________
//
Subscriber* EventBus::firstSubscriber(EventFilter* filter)
{
    return filter->firstSubscriber;
}
Subscriber* EventBus::nextSubScriber(Subscriber* current)
{
    return current->nextSubscriber;
}
//____________________________________________________________________
//
Subscriber* EventBus::lastSubscriber(EventFilter* filter)
{
    for(Subscriber* sub=firstSubscriber(filter); sub!=0; sub=nextSubScriber(sub))
    {
        if ( sub->nextSubscriber ==0 ) return sub;
    }
    ASSERT(false); // shouldn't come here
    return 0;
}

EventFilter::EventFilter(uint32_t header) : filter(header),firstSubscriber(0),nextFilter(0)
{

}
//____________________________________________________________________
//


Subscriber::Subscriber() : nextSubscriber(0)
{
}
