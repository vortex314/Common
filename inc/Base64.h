/*
 * Base64.h
 *
 *  Created on: Mar 18, 2016
 *      Author: lieven
 */

#ifndef SRC_BASE64_H_
#define SRC_BASE64_H_

#include <Erc.h>
#include <Str.h>


class Base64 {
private:
	static const char encodeCharacterTable[];
	static const char decodeCharacterTable[];
public:
	Base64();
	~Base64();
	static Erc Encode(Bytes& in, Str& out);
	static Erc Decode(Str& in, Bytes& out);
};

#endif /* SRC_BASE64_H_ */
