#ifndef LOG_H
#define LOG_H

using namespace std;
#include <Str.h>

class Log: public Str {
	static Log& logger;
private:
	Log();
public:
	static Log& log();

	Log& flush();
	Log& dump(Bytes& bytes);
	Log& message(const char* header, Bytes& bytes);
};

#endif // LOG_H
