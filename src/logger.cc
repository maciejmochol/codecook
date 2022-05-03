#include <stdio.h>

#include "logger.h"


// 
// Class Logger
//

void Logger::Log(const char *s, ...) {

	LockMe();

	va_list fmt_args;
	va_start(fmt_args, s);

	vfprintf(stderr, s, fmt_args);
	fprintf(stderr, "\n");

	UnlockMe();

}

void Logger::Logv(const char *s, va_list l) {

	LockMe();

	vfprintf(stderr, s, l);
	fprintf(stderr, "\n");

	UnlockMe();
}

void Logger::Logn(const char *s, ...) {

	LockMe();

	va_list fmt_args;
	va_start(fmt_args, s);

		vfprintf(stderr, s, fmt_args);

	va_end(fmt_args);

	UnlockMe();

}	

void Logger::Logvn(const char *s, va_list l) {

	LockMe();

		vfprintf(stderr, s, l);

	UnlockMe();
}

void Logger::LogNL(const char *s, ...) {

	va_list fmt_args;
	va_start(fmt_args, s);

	vfprintf(stderr, s, fmt_args);
	fprintf(stderr, "\n");

}

void Logger::LogvNL(const char *s, va_list l) {

	vfprintf(stderr, s, l);
	fprintf(stderr, "\n");

}

void Logger::LognNL(const char *s, ...) {

	va_list fmt_args;
	va_start(fmt_args, s);

		vfprintf(stderr, s, fmt_args);

	va_end(fmt_args);

}	

void Logger::LogvnNL(const char *s, va_list l) {

		vfprintf(stderr, s, l);
}
