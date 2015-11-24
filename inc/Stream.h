/*
 * File:   Streams.h
 * Author: lieven2
 *
 * Created on 15 september 2013, 17:58
 */

#ifndef STREAM_H
#define	STREAM_H
#include <stdint.h>
#include "Erc.h"
#include "Bytes.h"

class Stream {
private:

public:
	IROM Stream(){};
	IROM virtual Erc write(uint8_t b)=0;
	IROM virtual Erc write(uint8_t* pb, uint32_t length)=0;
	IROM virtual Erc write(Bytes& bytes)=0;
	IROM  ~Stream(){};
	IROM virtual bool hasData()=0;
	IROM virtual bool hasSpace()=0;
	IROM virtual uint8_t read()=0;
	IROM virtual bool isConnected()=0;
	IROM virtual void connect()=0;
	IROM virtual void disconnect()=0;

};

#endif	/* STREAM_H */

