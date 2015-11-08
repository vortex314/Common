#ifndef LOGGER_H
#define LOGGER_H
#include <time.h>
#include <Str.h>
#include <errno.h>
#include <string.h>
class Logger
{
public :
    enum Level { DEBUG, INFO,WARN,ERROR, FATAL};
    Logger(int size) ;
    Logger& level(int level);
    void setLevel(Level l);
    Logger& module(const char * m);
    Logger& log(const char *s);
    Logger& log(int i);
    Logger& operator<<(const char *s );
    Logger& operator<<(int i );
    Logger& operator<<(Bytes& b);
    Logger& operator<<(Str& str);
    Logger& flush();
    Logger& debug();
    Logger& warn();
    Logger& info();
    Logger& error();
    Logger& fatal();
    Logger& perror(const char* s);
    const char* logTime();

private :
    int _level;
    int _logLevel;
    const char* _module;
    time_t _time;
    Str _str;
};
#define LOG_FILE __FILE__
#define LOG_DATE __DATE__
#endif // LOGGER_H
