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
#else
Logger& Logger::header(const char* file_source, const char* function) {
	if (length())
		*this << FLUSH;
	clear();
	append(Sys::millis());
	while (length() < 8)
		append(' ');
	append("| ");
	append(file_source);
	while (length() < 30)
		append(' ');
	append("| ");
	append(function);
	while (length() < 40)
		append(' ');
	append("| ");
	return *this;
}

extern "C" void uart0WriteBytes(uint8_t* data, uint32_t length);

Logger& Logger::operator<<(LogCmd cmd) {
	if (cmd == FLUSH) {
		append("\n");
		uart0WriteBytes((uint8_t*) c_str(), length());
		Sys::delayUs(10000);
		clear();
	}
	return *this;
}
#include <stdarg.h>
extern "C" int ets_vsnprintf(char *, size_t, const char *, va_list);

Logger& Logger::vlog(const char * format, va_list args) {

	return *this;
}

Logger& Logger::log(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char* buffer = (char*) (data() + length());
	uint32_t size = capacity() - length();
	ets_vsnprintf(buffer, size, fmt, args);
	length(length() + strlen(buffer));
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

Logger& Logger::operator<<(int i) {
	append(i);
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
		appendHex(bytes.peek(i));
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

extern "C" void SysLogger(int level, const char* file,
		const char* function, const char * fmt, ...) {
	Logger::logger->header(file, function);
	va_list args;
	va_start(args, fmt);
	Logger::logger->vlog(fmt, args);
	va_end(args);
	*Logger::logger << FLUSH;
}

