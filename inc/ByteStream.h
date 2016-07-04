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

class ByteStream {
private:

public:
//	IROM  Stream() {};
	 virtual Erc write(uint8_t b)=0;
	 virtual Erc write(Bytes& bytes)=0;
//	IROM  ~Stream(){};
	 virtual bool hasData()=0;
	 virtual bool hasSpace()=0;
	 virtual uint8_t read()=0;
	 virtual bool isConnected()=0;
//	 virtual void connect()=0;
	 virtual void disconnect()=0;

};

#endif	/* STREAM_H */

