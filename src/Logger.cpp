#include "Logger.h"
#ifdef __linux__
#include <iostream>
#include <sys/time.h>
#include <stdio.h>
#endif

const char *sLevel[] = { "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL" };

Logger* Logger::logger = new Logger(256);

Logger::Logger(int size) :
		Str(size) {
	_module = "";
	_logLevel = DEBUG;
	_level = INFO;
	_format = FORMAT_DEC;
}

#ifdef _linux
Logger& Logger::header(const char* file_source,const char* function) {
	if (_level >= _logLevel) {
		std::cout << logTime();
		std::cout << " | ";
		std::cout << sLevel[_level] << " | ";
		std::cout << _module << " - ";
		_str.offset(0);
		while (_str.hasData())
		std::cout << _str.read();
		std::cout << std::endl;
	}
	_str.clear();
	return *this;
}
Logger& Logger::operator<<(LogCmd cmd) {
	if ( cmd==FLUSH) {
		append("\n");
		uart0WriteBytes((uint8_t*)c_str(),length());
	}
}

const char* Logger::logTime() {
	static char line[100];
	struct timeval tv;
	struct timezone tz;
	struct tm *tm;
	gettimeofday(&tv, &tz);
	tm = localtime(&tv.tv_sec);
	sprintf(line, "%d/%02d/%02d %02d:%02d:%02d.%03ld", tm->tm_year + 1900,
			tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
			tv.tv_usec / 1000);

//   strftime (line, sizeof(line), "%Y-%m-%d %H:%M:%S.mmm", sTm);
	return line;
}
Logger& Logger::perror(const char* s) {
	level(ERROR);
	_str << s << " failed. errno = " << errno << ":" << strerror(errno);
	flush();
	return *this;
}
#endif

Logger& Logger::header(const char* file_source, const char* function) {
	uint8_t hour, min, sec;
	uint32_t msec;
	uint32_t days;
	uint64_t now = Sys::millis();
	if (length())
		*this << FLUSH;
	clear();
	msec = now % 1000;
	now /= 1000;
	sec = now % 60;
	now /= 60;

	min = now % 60;
	now /= 60;
	hour = now % 24;
	now /= 24;
	days = now;

	append(days);
	append('d');

	if (hour < 10)
		append(":0");
	else
		append(":");
	append((int32_t)hour);

	if (min < 10)
		append(":0");
	else
		append(":");
	append((int32_t)min);

	if (sec < 10)
		append(":0");
	else
		append(":");
	append((int32_t)sec);

	if (msec < 10)
		append(",00");
	else if (msec < 100)
		append(",0");
	else
		append(",");
	append(msec);

	while (length() < 10)
		append(' ');
	append("| ");
	append(file_source);
	while (length() < 32)
		append(' ');
	append("| ");
	append(function);
	while (length() < 42)
		append(' ');
	append("| ");
	return *this;
}

#ifdef ARDUINO_ARCH_ESP8266__
#include <Arduino.h>
Logger& Logger::operator<<(LogCmd cmd) {
	if (cmd == FLUSH) {
		_format = FORMAT_DEC;
		Serial.println( c_str());
		clear();
	} else if (cmd == HEX) {
		_format = FORMAT_HEX;
	} else if (cmd == DEC) {
		_format = FORMAT_DEC;
	}
	return *this;
}
#include <stdarg.h>


Logger& Logger::vlog(const char * fmt, va_list args) {

	char* buffer = (char*) (data() + length());
	uint32_t size = capacity() - length();
	vsnprintf(buffer, size, fmt, args);
	length(length() + strlen(buffer));
	va_end(args);
	return *this;
}
#ifdef ___1__
extern "C" void uart0WriteBytes(uint8_t* data, uint32_t length);

Logger& Logger::operator<<(LogCmd cmd) {
	if (cmd == FLUSH) {
		_format = FORMAT_DEC;
		append("\n");
		uart0WriteBytes((uint8_t*) c_str(), length());
		Sys::delayUs(10000);
		clear();
	} else if (cmd == LOG_HEX) {
		_format = FORMAT_HEX;
	} else if (cmd == LOG_DEC) {
		_format = FORMAT_DEC;
	}
	return *this;
}
#include <stdarg.h>
extern "C" int ets_vsnprintf(char *, size_t, const char *, va_list);

Logger& Logger::vlog(const char * fmt, va_list args) {

	char* buffer = (char*) (data() + length());
	uint32_t size = capacity() - length();
	ets_vsnprintf(buffer, size, fmt, args);
	length(length() + strlen(buffer));
	va_end(args);
	return *this;
}
#endif
Logger& Logger::log(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vlog(fmt, args);
	va_end(args);
	return *this;
}

#endif

void Logger::setLevel(Level l) {
	_logLevel = l;
}
Logger& Logger::level(int level) {
	_level = level;
	return *this;
}

Logger& Logger::hex() {
	_format = FORMAT_HEX;
	return *this;
}

Logger& Logger::dec() {
	_format = FORMAT_DEC;
	return *this;
}

Logger& Logger::operator<<(int i) {
	if (_format == FORMAT_HEX)
		appendHex((uint32_t) i);
	else
		append((int32_t)i);
	return *this;
}

Logger& Logger::operator<<(uint32_t i) {
	if (_format == FORMAT_HEX)
		appendHex(i);
	else
		append(i);
	return *this;
}

Logger& Logger::operator<<(uint64_t i) {
	if (_format == FORMAT_HEX)
		appendHex(i);
	else
		append(i);
	return *this;
}

Logger& Logger::operator<<(float f) {
	append(f);
	return *this;
}

Logger& Logger::operator<<(const char *s) {
	append(s);
	return *this;
}

Logger& Logger::operator<<(Str& str) {
	uint8_t b;
	str.offset(0);
	while (str.hasData()) {
		b = str.read();
		if (b == '\n')
			*this << FLUSH;
		else
			write(b);
	}
	return *this;
}

#ifdef _linux
Logger& Logger::flush() {

}

#endif

//_______________________________________________________________________________
//
//
//_______________________________________________________________________________

Logger& Logger::operator<<(Bytes& bytes) {
	uint32_t i;
	for (i = 0; i < bytes.length(); i++) {
		if (i)
			append(" ");
		appendHex((uint8_t) bytes.peek(i));
	}
	return *this;
}

#define isprint(c) (((c)>0x1F) && ((c)>0x7f))

Logger& Logger::dump(Bytes& bytes) {
	uint32_t i;

	for (i = 0; i < bytes.length(); i++) {
		uint8_t b;
		b = bytes.read();
		appendHex(b);
		*this << " ";
	}
	bytes.offset(0);
	for (i = 0; i < bytes.length(); i++) {
		uint8_t b;
		b = bytes.read();
		if (isprint((char )b))
			*this << (char) b;
		else
			*this << '.';
	}
	return *this;
}

extern "C" void SysLogger(int level, const char* file, const char* function,
		const char * fmt, ...) {
	Logger::logger->header(file, function);
	va_list args;
	va_start(args, fmt);
	Logger::logger->vlog(fmt, args);
	va_end(args);
	*Logger::logger << FLUSH;
}
