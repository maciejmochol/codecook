#ifndef __LOGGER_H__
#define __LOGGER_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdarg.h>
#include "blocking.h"

class Logger : public Monitor {
public:
	Logger() { };
	~Logger() { };

	void Log(const char *s, ...);
	void Logv(const char *, va_list);

	void Logn(const char *s, ...); // No NEWLINE
	void Logvn(const char *, va_list); // No NEWLINE

	void LogNL(const char *s, ...); // NO LOCK
	void LogvNL(const char *, va_list); // NL LOCK

	void LognNL(const char *s, ...); // No NEWLINE, NO LOCK
	void LogvnNL(const char *, va_list); // No NEWLINE, NO LOCK
};


#endif
