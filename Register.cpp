/*
 * Register.cpp
 *
 *  Created on: 25 Apr 2019
 *      Author: lieven
 */

#include "Register.h"

Register::Register(const char* name, const char* format,uint32_t* regAddress)
	:  _name(name),_format(format),_regAddress(regAddress) {
	_reg = 0;
}
Register::Register(const char* name, const char* format)
	:  _name(name),_format(format),_regAddress(0) {
	_reg = 0;
}

Register::~Register() {
}

void Register::value(uint32_t reg) {
	_reg = reg;
}

void Register::show() {
	std::string str;
	uint32_t reg = 0;
	if ( _regAddress==0) reg = _reg;
	else reg=*_regAddress;
	unsigned int bitsValue=0;
	uint32_t bitCount=1;
	const char* fmt = _format;
	while (fmt != 0) {

		const char* next = strchr(fmt, ' ');
		if (reg & 0x80000000) {
			bitsValue += 1;
		}
		if (*fmt == '+') { // add this bit to total value
			bitCount++;
		} else if ( *fmt=='-')  { // ignore these bits
			bitCount=0;
		} else { // show accumulated value
			bitCount++;
			if (bitCount > 1 || ( bitsValue>0 && bitCount==1)  ) {
				if (next == 0)
					str.append(fmt);
				else
					str.append(fmt, next - fmt);
			}
			if (bitCount > 1) {
				str.append("=");
				char numstr[21]; // enough to hold all numbers up to 64-bits
				sprintf(numstr, "0x%X", bitsValue);
				str +=  numstr;
			} else if ( bitCount==1 && bitsValue != 0 ) {
				char numstr[21]; // enough to hold all numbers up to 64-bits
				sprintf(numstr, "%d", bitsValue);
//               str +=  numstr;
			}
			bitsValue = 0;
			bitCount = 0;
			str.append(",");
		}

		bitsValue <<= 1;
		reg <<= 1;
		fmt = next;
		if (fmt) fmt++;
	}
	INFO(" %10s = %08X |%s|", _name, _reg, str.c_str());
}
