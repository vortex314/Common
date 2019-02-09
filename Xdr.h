#ifndef XDR_H_
#define XDR_H_
#include <assert.h>
#include <cstring>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <Sys.h>
#include <Uid.h>
#include <Log.h>

typedef uint16_t uid_type;

class Tag;

class Xdr {
		uint32_t _writeIdx;
		uint32_t _readIdx;
		uint32_t _capacity;
		uint32_t* _start;

	public:
		typedef enum { BOOL, INT, UINT, BYTES, FLOAT, OBJECT, XDR_1, RESERVED } Type;
		static const char* typeStrings[Type::XDR_1 + 1];

		uint32_t&  operator[](uint32_t idx) { return _start[idx];};

		Xdr(uint32_t size);
		Xdr(Xdr& src);
		~Xdr();
		inline void rewind() {_readIdx=0;};
		inline void end();
		void clear();
		uint32_t peek(uint32_t offset) const;
		uint32_t peek() const;

		bool skip();
		int writeInc(uint32_t );
		inline int readInc(uint32_t);
		inline bool hasSpace(uint32_t);
		int read(uint32_t& i);
		int read(uint32_t* pi,uint32_t cnt);
		int write(uint32_t i);
		int write(uint32_t* pi, uint32_t i);
		int write(uint8_t* pi, uint32_t i);
		int write(Tag tag);
		int add(const uid_type key, int v);
		int add(const uid_type key, bool b);
		int add(const uid_type key, double d);
		int add(const uid_type key, uint16_t v);
		int add(const uid_type key, uint32_t v);
		int add(const uid_type key, uint64_t v);
		int add(const uid_type key, int64_t v);
		int add(const uid_type key, uint8_t* bytes, uint32_t length);
		int add(const uid_type key, std::string& v);
		int add(const uid_type key, const char* s);
		int add(Xdr& xdr);

		template <typename T> Xdr& operator()(uid_type key, T v) {
			add(key,v);
			return *this;
		};

		template <typename T> Xdr& add(const char* key, T v) {
			add(Uid(key).id(),v);
			return *this;
		};

		int getNext(const uid_type key, bool& b);
		int getNext(const uid_type key, double& i);
		int getNext(const uid_type key, int32_t& i);
		int getNext(const uid_type key, uint16_t& i);
		int getNext(const uid_type key, uint32_t& i);
		int getNext(const uid_type key, uint64_t& i);
		int getNext(const uid_type key, int64_t& i);
		int getNext(const uid_type key, std::string& s);
		int getNext(const uid_type key, uint8_t* bytes, uint32_t& size);

		template <typename T> int get( Uid key, T& v) {
			rewind();
			return getNext(key.id(), v);
		};

		int vscanf(const char* fmt,...);
		bool hasData() const ;

		std::string toString();
		bool find(Tag& t) ;
		Xdr& operator=(const Xdr &)   ;

		uint32_t size() const;
		uint32_t capacity() const;
		void resize(uint32_t newSize);
		void poke(uint32_t offset,uint32_t value);

};

#define BYTES_TO_WORDS(lll) (lll&0x3) ? (lll>>2)+1 : (lll>>2)

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

		Tag(uint32_t i) ;
		Tag(Xdr::Type t, uint16_t l, Uid ui) ;
		void operator=(uint32_t i) ;
		std::string toString() ;
};

#endif
