/*
 * Uart.h
 *
 *  Created on: Sep 17, 2015
 *      Author: lieven
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "Erc.h"
#include "Bytes.h"
#include "CircBuf.h"


class Stream {
	virtual Erc write(Bytes& data)=0;
	virtual Erc write(uint8_t b)=0;
	virtual bool hasData()=0;
	virtual bool hasSpace()=0;
	virtual uint8_t read()=0;
};

class Uart : public Stream {

public:
	Uart();
	~Uart();
	virtual void init(uint32_t baudrate)=0;
	Erc write(Bytes& data)=0;
	virtual Erc write(uint8_t b)=0;
	virtual bool hasData()=0;
	virtual bool hasSpace()=0;
	virtual uint8_t read()=0;
	virtual void receive(uint8_t b);


};

#endif /* UART_H_ */
