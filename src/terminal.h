/* ccerror.h */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "thread.h"
#include <stdio.h>

class Terminal;
class Terminal_Serial;
class Terminal_TCPIP;

// 
// class Terminal
//

#define TERM_OK 0
#define TERM_QUIT 1
#define TERM_BADCOMMAND 2
#define MAX_ARGS 20

#define IN_BUFFER_SIZE 400

class Terminal : public Codecook_Thread 
{
protected: 

	char in_buffer[IN_BUFFER_SIZE], *out_buffer;
	
public:
	Terminal(char *s);
	~Terminal();
	virtual void * Handler();
	virtual int Read();
	virtual int Write();
	virtual void Close();
	virtual int RunCommand();
	int read_line(register int deskr, register char *s, 
		register int nbytes);
	int write_line(register int deskr, register char *s, 
		register int nbytes);
};

//
// class Terminal_Console (run from Codecook console)
//

class Terminal_Console : public Terminal
{

public:
	Terminal_Console();

	int Read();
	int Write();

};

// 
// class Terminal_RunScript
//

class Terminal_RunScript : public Terminal
{
char *script_file;
FILE *file;

public:
	Terminal_RunScript(char *file_name = "./conf/rc.conf");
	~Terminal_RunScript();

	int Read();
	int Write();
};


// 
// class Terminal_Serial
//

// 
// class Terminal_TCPIP
//

class Terminal_TCPIP : public Terminal 
{
	int tcp_port;
public:
	Terminal_TCPIP(int portnum);
	virtual ~Terminal_TCPIP();
	int Read();
	int Write();
	void Close();
};

#endif
