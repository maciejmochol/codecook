/* $Id: thread.h,v 1.14 2000/04/25 17:52:36 code Exp $ */

/* thread.h */

/* Copyright <C> 1999 by Codematic Solutions */

/* classes, methods, data structures for managing threads in codecook */

#ifndef _THREAD_H_
#define _THREAD_H_

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <setjmp.h>

#include <signal.h>
#define DO_SIGNAL_INFO

#include "codecook.h"
#include "itc.h"
#include "blocking.h"
#include "datastruct.h"

// Request used by threads and manager
// Req_Fault & Req_Terminate

// Predefined thread types

enum ThreadTypes { 
	UNKNOWN,
	DEVICE,
	MEANER,
	TERMINAL,
	ALARMSERV,
	NETMANAGER
};

char *GetThreadTypeName(ThreadTypes type);

// Req_Fault

class Req_Fault : public Request {
	int pid, sig;
public:
	Req_Fault(int _pid, int _sig) : pid(_pid), sig(_sig) { type = THREAD_FAULT; }
	int GetPid() { return pid; }
	int GetSig() { return sig; }
};

class Req_Terminate : public Request {
	int pid;
public:
	Req_Terminate(int _pid) : pid(_pid) { type = THREAD_TERMINATE; }
	int GetPid() { return pid; }
};

// Class Thread

typedef void * (*Thread_Func)(void*);
#ifdef DO_SIGNAL_INFO
typedef void   (*Signal_Func)(int, struct siginfo*, void*);
#else
typedef void   (*Signal_Func)(int);
#endif

struct retAddressS{
	jmp_buf retBuf;
};

/**
 * Class Thread is base class for threads in Codecook
 * Includes method for creating, terminating, signal-handling,
 * and communicating (with requests) 
 *
 * @group Core
 */ 
class Thread : public Monitor {

protected:
	pthread_t thread_id;
	int validity;
	bool running;
	retAddressS retAddress;
	ThreadTypes type;

protected:
	Request_Buffer req_buf;
	virtual void SetSignalHandling(int signal_no = 0); 
			// 0 = set all signals
	virtual Signal_Func GetFatalSignalFunc(void);
	virtual Signal_Func GetTerminateSignalFunc(void);
	virtual Signal_Func GetExitSignalFunc(void);
#ifdef DO_SIGNAL_INFO
	friend void Default_Fatal_Signal_Func(int sig_no, struct siginfo* info, void* something);
	friend void Default_Terminate_Signal_Func(int sig_no, struct siginfo* info, void* something);
	friend void Default_Exit_Signal_Func(int sig_no, struct siginfo* info, void* something);
#else 
	friend void Default_Fatal_Signal_Func(int sig_no);
	friend void Default_Terminate_Signal_Func(int sig_no);
	friend void Default_Exit_Signal_Func(int sig_no);
#endif

public:
	Thread(ThreadTypes _type);
	virtual ~Thread();
	
	void Start();
	void Join(void ** retval);
	void Cancel();
	void Detach();
	void Exit(void *retval);
	void Send_Request(Request *req);

	void SetValid()   { validity = 1; }
	void SetInValid() { validity = 0; }
	int  IsValid()    { return validity; } 
	bool IsRunning()  { return running; }
	ThreadTypes GetType() { return type; }
	retAddressS getRetAddress() { return retAddress; }
	void setRetAddress(retAddressS ra) { retAddress = ra; }

	virtual Thread_Func GetThreadFunc(void);
	pthread_t GetThreadId() const;
	void * PreHandler(); //
	virtual void * Handler();
	virtual void Send_Termination_Request();
	friend void * Default_Thread_Func(void * data);
	
	/* Information functions */
	long UnsharedDataSize();
};


class Codecook_Thread : public Thread
{
protected:
	char *name;

public:
	Codecook_Thread(char *, ThreadTypes _type);
	virtual ~Codecook_Thread();
	char *GetName() const;

	void Log(const char *, ...);
	void Logn(const char *, ...);
	void Logvn(const char *, va_list);

//to nizej jest do poprawki!!!!! : va_list etc...
	void Fatal(char *s, void *retval) { Log(s); Exit(retval); };
	void Fatal(char *s) { Fatal(s, NULL); };
};


struct SThreadInfo {
	Thread *thrPtr;
	int pid;
	pthread_t id;

	SThreadInfo(Thread *pThrPtr, int pPid, pthread_t pId = (pthread_t) 0) 
			{pid = pPid; thrPtr = pThrPtr; id = pId; }
};

class WorkingThreads : public Room {

TList<SThreadInfo> *threadList;

public:
	WorkingThreads();
	Thread *getThread(int pid, LockOrNot lockFlag = LOCK);
	SThreadInfo *getThreadInfo(int pid, LockOrNot lockFlag = LOCK);
	SThreadInfo *getThreadInfo(Thread *pThrPtr, LockOrNot lockFlag = LOCK );
	int getThreadPid(char *name, LockOrNot lockFlag = LOCK );
	char *getThreadTree(ThreadTypes type_to_show = UNKNOWN);
	void AddThread(Thread *pthrPtr, int pPid, pthread_t pId = (pthread_t)0);
	void EradicateThread(Thread *pthrPtr); // delete Thread
	void EradicateThread(int pPid);		// delete Thread
	void DelThread(Thread *pthrPtr); // just from the list
	void DelThread(int pPid);	 // just form the list

};


#endif
