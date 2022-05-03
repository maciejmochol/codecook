/* ccerror.h */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _MANAGER_H_
#define _MANAGER_H_

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "thread.h"
#include "protocol.h"

// 
// class Manager
//

//
// Manager juz nie jest klasa Codecook_Thread poniewaz
// uruchamia go watek main() i wykonuje sie w ramach niego
//


class Manager
{
	Request_Buffer req_buf;
public:
	Manager();
	
	void Send_Request(Request *req);
	void Start();

	void Log(const char *, ...);
	void Logn(const char *, ...);
};

//
// AdminListThreads command
//

class AdminListThreadsCommand : public ExecuteCommand {
public:
	AdminListThreadsCommand () { };
	~AdminListThreadsCommand () { };
	char * execute(Command *);
};

#endif
