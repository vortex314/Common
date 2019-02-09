#ifndef XDR_H_
#define XDR_H_
#include <Log.h>
#include <Sys.h>
#include <Uid.h>
#include <assert.h>
#include <cstring>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <unordered_map>


class Tag;

class Xdr { /*: public Stream<uint32_t> */
		uint32_t _writeIdx;
		uint32_t _readIdx;
		uint32_t _capacity;
		uint32_t* _start;

	public:
		typedef enum {
			BOOL,
			INT,
			UINT,
			BYTES,
			FLOAT,
			OBJECT,
			XDR_1,
			RESERVED
		} Type;
		static const char* typeStrings[Type::XDR_1 + 1];

		uint32_t& operator[](uint32_t idx) { return _start[idx]; };

		Xdr(uint32_t size);
		Xdr(Xdr& src);
		~Xdr();
		inline void rewind() { _readIdx = 0; };
		inline void end();
		void clear();
		uint32_t peek(uint32_t offset) const;
		uint32_t peek() const;

		bool skip();
		int writeInc(uint32_t);
		inline int readInc(uint32_t);
		inline bool hasSpace(uint32_t);
		int read(uint32_t& i);
		int read(uint32_t* pi, uint32_t cnt);
		int write(uint32_t i);
		int write(uint32_t* pi, uint32_t i);
		int write(uint8_t* pi, uint32_t i);
		int write(Tag tag);
		int add(Uid key, int v);
		int add(Uid key, bool b);
		int add(Uid key, double d);
		int add(Uid key, uint16_t v);
		int add(Uid key, uint32_t v);
		int add(Uid key, uint64_t v);
		int add(Uid key, int64_t v);
		int add(Uid key, uint8_t* bytes, uint32_t length);
		int add(Uid key, std::string& v);
		int add(Uid key, const char* s);
		int add(Xdr& xdr);
		template <typename T> Xdr& operator()(Uid key, T v) {
			add(key, v);
			return *this;
		};

		int getNext(Uid key, bool& b);
		int getNext(Uid key, double& i);
		int getNext(Uid key, int32_t& i);
		int getNext(Uid key, uint16_t& i);
		int getNext(Uid key, uint32_t& i);
		int getNext(Uid key, uint64_t& i);
		int getNext(Uid key, int64_t& i);
		int getNext(Uid key, std::string& s);
		int getNext(Uid key, uint8_t* bytes, uint32_t& size);
		template <typename T> int get(Uid key, T& v) {
			rewind();
			return getNext(key, v);
		};

		int vscanf(const char* fmt, ...);
		bool hasData() const;

		std::string toString();
		bool find(Tag& t);
		Xdr& operator=(const Xdr&);

		uint32_t size() const;
		uint32_t capacity() const;
		void resize(uint32_t newSize);
		void poke(uint32_t offset, uint32_t value);
};

class Tag {
		static const char* typeStrings[Xdr::RESERVED + 1];

	public:
		union {
			struct {
				Xdr::Type type : 3;
				uint32_t length : 13;
				uint16_t uid : 16;
			};
			uint32_t ui32;
		};

		Tag(uint32_t i);
		Tag(Xdr::Type t, uint16_t l, Uid ui);
		void operator=(uint32_t i);
		std::string toString();
};

#endif
