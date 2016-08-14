/*
 * CircBuf.h
 *
 *  Created on: 19-aug.-2012
 *      Author: lieven
 */

#ifndef CIRCBUF_H_
#define CIRCBUF_H_
//#include "base.h"
#include <stdint.h>
#include <errno.h>
#include <Sys.h>

#define POW 4
#define CIRCBUF_SIZE (1<<POW)
#define CIRCBUF_MASK (CIRCBUF_SIZE-1)
#define IRAM

class CircBuf {
private:
	uint8_t* start;
	uint16_t readPos;
	uint16_t writePos;
	uint16_t limit;
public:
	 IRAM CircBuf(int size);
	 IRAM ~CircBuf();
	int IRAM write(uint8_t b);
	int  IRAM writeFromIsr(uint8_t b);
	int IRAM read();
	int IRAM readFromIsr();
	bool IRAM  hasSpace();
	bool IRAM hasSpace(uint32_t size);
	bool  IRAM hasData();
	void IRAM clear();
	uint32_t IRAM  size();
	uint32_t IRAM space();
};

#endif /* CIRCBUF_H_ */
