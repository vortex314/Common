/*
 * MqttSeq.cpp
 *
 *  Created on: 23-aug.-2014
 *      Author: lieven2
 */

#include "Mqtt.h"
#include "Prop.h"
#include "stdint.h"
extern PropMgr propMgr;

//*************************************************** GLOBAL VARS *************
Log log;
/*****************************************************************************
 *  Generate next message id
 ******************************************************************************/
uint16_t gMessageId = 1;

uint16_t Mqtt::nextMessageId()
{
    return ++gMessageId;
}

//____________________________________________________________________________
//
//____________________________________________________________________________

//____________________________________________________________________________
//       M      Q       T       T
//  sends MQTT_CONNECTED, MQTT_DISCONNECTED
//  listens for TCPCONNECTED, TCP_DISCONNECTED
//____________________________________________________________________________

Mqtt::Mqtt(Link& link) :
    Handler("Mqtt"), _link(link), _prefix(SIZE_TOPIC), _mqttOut(
        SIZE_MQTT)
{
    _mqttPublisher = new MqttPublisher(*this);
    _mqttSubscriber = new MqttSubscriber(*this);
    _mqttSubscription = new MqttSubscription(*this);
//	_mqttPinger = new MqttPinger(*this);

    _mqttPublisher->stop();
    _mqttSubscriber->stop();
//	_mqttPinger->stop();
    _mqttSubscription->stop();

    _retries = 0;
    timeout(100);
    _isConnected = false;
}
Mqtt::~Mqtt()
{
}

bool Mqtt::isConnected()
{
    return _isConnected;
}

void Mqtt::setPrefix(const char* s)
{
    _prefix.clear() << s;
}

void Mqtt::sendConnect()
{
    Str str(8);
    str << "false";
    Str online(30);
    online << _prefix << "system/online";
    _mqttOut.Connect(MQTT_QOS2_FLAG, _prefix.c_str(), MQTT_CLEAN_SESSION,
                     online.c_str(), str, "", "",
                     TIME_KEEP_ALIVE / 1000);
    _link.send(_mqttOut);
}

Handler* Mqtt::publish(Str& topic, Bytes& message, Flags flags)
{
    return _mqttPublisher->publish(topic, message, flags);
}

Handler* Mqtt::subscribe(Str& topic)
{
    return _mqttSubscription->subscribe(topic);
}

//________________________________________________________________________________________________
//
//
//________________________________________________________________________________________________
Msg msg(0);

bool Mqtt::dispatch(Msg& msg)
{
    PT_BEGIN()
DISCONNECTED:
    {
        _link.disconnect();
        _isConnected = false;
//		_mqttPublisher->stop(); don't start if nothing to publish !!
        _mqttSubscriber->stop();
        _mqttSubscription->stop();
        msg.create(this, SIG_DISCONNECTED).send();
        while (true) // DISCONNECTED STATE
        {
            _link.connect();
            timeout(TIME_CONNECT);
            PT_YIELD_UNTIL(_link.isConnected() || timeout()); //  wait Uart connected
            goto LINK_CONNECTED;
        }
    }
LINK_CONNECTED:
    {
        while (true) // LINK_CONNECTED
        {
            sendConnect();
            timeout(TIME_WAIT_CONNECT);
            PT_YIELD_UNTIL(
                msg.is(&_link, SIG_RXD, MQTT_MSG_CONNACK) || !_link.isConnected() || timeout()); // wait reply or timeout on connect send
            if (!_link.isConnected() || timeout())
                goto DISCONNECTED;
            if (msg.is(&_link, SIG_RXD, MQTT_MSG_CONNACK))
            {
                Msg::publish(this, SIG_CONNECTED);
                _isConnected = true;
                _mqttSubscriber->restart();
                goto PING_SLEEP;
            }
        }
    }
PING_SLEEP:
    {
        timeout(TIME_PING);
        PT_YIELD_UNTIL(timeout() || !_link.isConnected());
        if (!_link.isConnected())
            goto DISCONNECTED;
        _retries = 0;
        goto PINGING;
    }
PINGING:
    {
        for (_retries = 0; _retries < MAX_RETRIES; _retries++)
        {
            _mqttOut.PingReq();
            _link.send(_mqttOut);
            timeout(TIME_WAIT_REPLY);
            PT_YIELD_UNTIL(
                (msg.is(&_link ,SIG_RXD,MQTT_MSG_PINGRESP) || timeout()));
            if (msg.sig() == SIG_RXD)
                goto PING_SLEEP;
        }
        goto DISCONNECTED;
    }
    PT_END()
}

bool MqttSubscription::dispatch(Msg& msg)
{
    PT_BEGIN()
    _messageId = Mqtt::nextMessageId();
    for (_retries = 0; _retries < MAX_RETRIES; _retries++)
    {
        sendSubscribe();
        timeout(TIME_WAIT_REPLY);
        PT_YIELD_UNTIL(
            msg.is(&_mqtt._link, SIG_RXD, MQTT_MSG_SUBACK) || msg.is(&_mqtt._link, SIG_DISCONNECTED ) || timeout());
        if (msg.is(&_mqtt._link, SIG_RXD, MQTT_MSG_SUBACK))
        {
            int id;
            if ( msg.get(id) && id == _messageId)
            {
                Msg::publish(this, SIG_ERC);
                PT_EXIT()
                ;
            }
        }
        else if (msg.is(&_mqtt._link, SIG_DISCONNECTED))
        {
            Msg::publish(this, SIG_ERC, ECONNABORTED);
            PT_EXIT()
            ;
        }
    }
    Msg::publish(this, SIG_ERC, EAGAIN);
    PT_EXIT()
    ;
    PT_END()
    ;
}


//____________________________________________________________________________
//       PUBLISHER
//____________________________________________________________________________
MqttPublisher::MqttPublisher(Mqtt& mqtt) :
    Handler("MqttPublisher"), _mqtt(mqtt), _topic(SIZE_TOPIC), _message(
        SIZE_MESSAGE)
{
    _messageId = 0;
    _retries = 0;
    _state = ST_READY;
}

Handler* MqttPublisher::publish(Str& topic, Bytes& msg, Flags flags)
{
    if (!_mqtt.isConnected())
        return 0;
    if (isRunning())
        return 0;
    _retries = 0;
    _topic = topic;
    _message = msg;
    _messageId = Mqtt::nextMessageId();
    _flags = flags;
    restart();
    return this;
}

void MqttPublisher::sendPublish()
{
    uint8_t header = 0;
    if (_flags.qos == QOS_0)
    {
        _state = ST_READY;
    }
    else if (_flags.qos == QOS_1)
    {
        header += MQTT_QOS1_FLAG;
        timeout(TIME_WAIT_REPLY);
    }
    else if (_flags.qos == QOS_2)
    {
        header += MQTT_QOS2_FLAG;
        timeout(TIME_WAIT_REPLY);
    }
    if (_flags.retained)
        header += MQTT_RETAIN_FLAG;
    if (_retries)
    {
        header += MQTT_DUP_FLAG;
    }
    _mqtt._mqttOut.Publish(header, _topic, _message, _messageId);
    _mqtt._link.send(_mqtt._mqttOut);
}

void MqttPublisher::sendPubRel()
{
    _mqtt._mqttOut.PubRel(_messageId);
    _mqtt._link.send(_mqtt._mqttOut);
}

bool MqttPublisher::dispatch(Msg& msg)
{
    PT_BEGIN()
//READY:
    {
        _state = ST_READY;
        PT_YIELD_UNTIL(msg.is(0, SIG_TICK));
        _state = ST_BUSY;
        if (_flags.qos == QOS_0)
        {
            sendPublish();
            Msg::publish(this, SIG_ERC);
            PT_EXIT()
            ;
        }
        else if (_flags.qos == QOS_1)
            goto QOS1_ACK;
        else if (_flags.qos == QOS_2)
            goto QOS2_REC;
        PT_EXIT()
        ;
    }
QOS1_ACK:
    {
        for (_retries = 0; _retries < MAX_RETRIES; _retries++)
        {
            sendPublish();
            timeout(TIME_WAIT_REPLY);
            PT_YIELD_UNTIL(
                msg.is(&_mqtt._link,SIG_RXD ,MQTT_MSG_PUBACK) || timeout());
            if (msg.is(&_mqtt._link, SIG_RXD, MQTT_MSG_PUBACK))
            {
                int id;
                msg.get(id);
                if (id == _messageId)
                {
                    Msg::publish(this, SIG_ERC);
                    PT_EXIT()
                    ;
                }
            }
        }
        Msg::publish(this, SIG_ERC, ETIMEDOUT);
        PT_EXIT()
        ;
    }
QOS2_REC:
    {
        for (_retries = 0; _retries < MAX_RETRIES; _retries++)
        {
            sendPublish();
            timeout(TIME_WAIT_REPLY);
            PT_YIELD_UNTIL(
                msg.is(&_mqtt._link,SIG_RXD ,MQTT_MSG_PUBREC) || timeout());
            if (msg.is(&_mqtt._link, SIG_RXD, MQTT_MSG_PUBREC))
            {
                int id;
                msg.get(id);
                if (id == _messageId)
                {
                    goto QOS2_COMP;
                }
            }
        }
        Msg::publish(this, SIG_ERC, ETIMEDOUT);
        PT_EXIT()
        ;
    }
QOS2_COMP:
    {
        for (_retries = 0; _retries < MAX_RETRIES; _retries++)
        {
            sendPubRel();
            timeout(TIME_WAIT_REPLY);
            PT_YIELD_UNTIL(
                msg.is(&_mqtt._link,SIG_RXD ,MQTT_MSG_PUBCOMP) || timeout());
            if (msg.is(&_mqtt._link, SIG_RXD, MQTT_MSG_PUBCOMP))
            {
                int id;
                if ( msg.get(id) && id == _messageId)
                {
                    Msg::publish(this, SIG_ERC);
                    PT_EXIT()
                    ;
                }
            }
        }
        Msg::publish(this, SIG_ERC, ETIMEDOUT);
        PT_EXIT()
        ;
    }
    PT_END()
}

//____________________________________________________________________________
//       SUBSCRIBER
//____________________________________________________________________________
MqttSubscriber::MqttSubscriber(Mqtt &mqtt) :
    Handler("Subscriber"), _mqtt(mqtt), _topic(SIZE_TOPIC), _message(SIZE_MESSAGE)
{
    _messageId = 0;
    _retries = 0;
}

void MqttSubscriber::sendPubRec()
{
    _mqtt._mqttOut.PubRec(_messageId);
    _mqtt._link.send(_mqtt._mqttOut);
    timeout(TIME_WAIT_REPLY);
}
#include "Prop.h"
extern PropMgr propMgr;

void MqttSubscriber::callBack()
{
//TODO propMgr.onPublish(_topic, _message);
}

// #define PT_WAIT_FOR( ___pt, ___signals, ___timeout ) listen(___signals,___timeout);PT_YIELD(___pt);

bool MqttSubscriber::dispatch(Msg& msg)
{
    PT_BEGIN()

READY:
    {
        timeout(100000000);
        PT_YIELD_UNTIL(
            msg.is(&_mqtt._link, SIG_RXD, MQTT_MSG_PUBLISH) || !_mqtt.isConnected() || timeout());
        if (msg.is(&_mqtt._link, SIG_RXD, MQTT_MSG_PUBLISH))
        {
            int _qos;
            msg.scanf("iiSS",_messageId,_qos,&_topic,&_message);

            if (_qos == MQTT_QOS0_FLAG)
            {

                callBack();
            }
            else if (_qos == MQTT_QOS1_FLAG)
            {

                callBack();
                _mqtt._mqttOut.PubAck(_messageId);
                _mqtt._link.send(_mqtt._mqttOut);

            }
            else if (_qos == MQTT_QOS2_FLAG)
            {
                goto QOS2_WAIT_REL;

            }
        }
        else if (!_mqtt.isConnected())
        {
            PT_EXIT()
            ;
        }
        goto READY;
    }
QOS2_WAIT_REL:
    {
        for (_retries = 0; _retries < MAX_RETRIES; _retries++)
        {
            sendPubRec();
            timeout(TIME_WAIT_REPLY);
            PT_YIELD_UNTIL(
                !_mqtt.isConnected() || msg.is(&_mqtt._link,SIG_RXD ,MQTT_MSG_PUBREL) || timeout());
            if (msg.is(&_mqtt._link, SIG_RXD, MQTT_MSG_PUBREL))
            {
                callBack();
                msg.scanf("i",_messageId);
                _mqtt._mqttOut.PubComp(_messageId);
                _mqtt._link.send(_mqtt._mqttOut);
                goto READY;
            }
        }
        goto READY;
    }
    PT_END()
}

//____________________________________________________________________________
//       SUBSCRIPTION
//____________________________________________________________________________
MqttSubscription::MqttSubscription(Mqtt & mqtt) :
    Handler("Subscription"), _mqtt(mqtt), _topic(SIZE_TOPIC)
{
    _retries = 0;
    _messageId = 0;
// listen(&_mqtt);
}

Handler* MqttSubscription::subscribe(Str& topic)
{
    if (isRunning() || !_mqtt.isConnected())
        return 0;
    _topic = topic;
    restart();
    return this;
}

void MqttSubscription::sendSubscribe()
{
    _mqtt._mqttOut.Subscribe(MQTT_QOS1_FLAG, _topic, _messageId, QOS_2);
    _mqtt._link.send(_mqtt._mqttOut);
    _retries++;
}

