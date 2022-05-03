/* $Id: thread.cc,v 1.17 2000/11/05 21:20:55 code Exp $ */

/* thread.cc */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>

#include "thread.h"
#include "device_list.h"
#include "logger.h"
#include "cstable.h"
#include "manager.h"
#include "parsing.h"
#include "xmlgen.h"

//
// GetThreadTypeName

char *GetThreadTypeName(ThreadTypes type)
{
	static char * nmDEVICE = "Device";
	static char * nmUNKNOWN = "Unknown";
	static char * nmMEANER = "Meaner";
	static char * nmTERMINAL = "Terminal";
	static char * nmALARMSERV = "AlarmServer";
	static char * nmNETMANAGER = "NetManager";

	switch (type) {
		case DEVICE:		return nmDEVICE;
		case MEANER:		return nmMEANER;
		case TERMINAL:		return nmTERMINAL;
		case ALARMSERV:		return nmALARMSERV;
		case NETMANAGER:	return nmNETMANAGER;
		default:		return nmUNKNOWN;
	};
}

//
// 
// Class Thread
//


Thread::Thread(ThreadTypes _type) : req_buf()
{
	type = _type;
}

Thread::~Thread()
{
	void *retval;

	Join(&retval);
}

void Thread::Start()
{
	pthread_create(&thread_id, NULL, GetThreadFunc(), this);
}

void Thread::Join(void ** retval)
{
	pthread_join(thread_id, retval);
}

void Thread::Cancel()
{
	pthread_cancel(thread_id);
}

void Thread::Exit(void *retval)
{
	pthread_exit(retval);
}

void Thread::Detach()
{
	pthread_detach(thread_id);
}

void Thread::Send_Request(Request *req)
{
	req_buf.Put(req);
}

Thread_Func Thread::GetThreadFunc(void) { return Default_Thread_Func; }

Signal_Func Thread::GetFatalSignalFunc(void) { 
	return Default_Fatal_Signal_Func; 
}

Signal_Func Thread::GetTerminateSignalFunc(void) { 
	return Default_Terminate_Signal_Func; 
}

Signal_Func Thread::GetExitSignalFunc(void) { 
	return Default_Exit_Signal_Func; 
}

void * Thread::PreHandler(){
// Here starts a new thread..
char *ptr;

	cstable->threadList->AddThread( this, getpid(), thread_id );
	SetSignalHandling(); // I know they share it, but it is a fine place

	//SetValid();

	int ret;

	if( (ret = setjmp(retAddress.retBuf) != 0 ))
	{
		if(ret == SIGINT){ // "Manager"

			// Tu  powinien byc SAVE!!!	
			cstable->logger->Log("Quitting..");

		} else { // FAULT SIGNAL
			Request *Req = new Req_Fault( getpid(), ret );
			cstable->manager->Send_Request( Req ); 
		}

	return NULL;
	} 
	
	//SetInValid();	

	running = 1;
	try {
		ptr = (char*) Handler();
	}
	catch (exc_Codecook &exc) {
		cout << "Thread " << getpid() << " " << GetThreadId() <<  " got exception !!! " << exc << endl;
	}
	running = 0;

	Send_Termination_Request();

return NULL;
}

void Thread::Send_Termination_Request()
{
	Request *Req = new Req_Terminate( getpid() );
	cstable->manager->Send_Request( Req ); 
}

void * Thread::Handler()
{
	return NULL;
}

void Thread::SetSignalHandling(int signal_no)
{

sigset_t lSet;

	sigfillset( &lSet );

	struct sigaction lSA; 

		lSA.sa_mask    = lSet;
#ifdef DO_SIGNAL_INFO
		lSA.sa_flags   = SA_RESTART | SA_SIGINFO;
		lSA.sa_sigaction = GetTerminateSignalFunc();
#else
		lSA.sa_flags   = SA_RESTART;
		lSA.sa_handler = GetTerminateSignalFunc();
#endif
		lSA.sa_restorer= NULL ;

	for(int i=1; i<32; i++)
		sigaction(i, &lSA, NULL); // Termination Request (?)

#ifdef DO_SIGNAL_INFO
		lSA.sa_sigaction = GetFatalSignalFunc(); 
					// Fatal Request ( Thread Suicide ) 
#else
		lSA.sa_handler = GetFatalSignalFunc(); 
#endif

		sigaction(SIGSEGV, &lSA, NULL);
		sigaction(SIGBUS, &lSA, NULL);
		sigaction(SIGFPE, &lSA, NULL);
		sigaction(SIGILL, &lSA, NULL);

		//lSA.sa_handler = GetExitSignalFunc(); // CTRL-C wychodzi
		lSA.sa_handler = SIG_DFL;
		sigaction(SIGINT, &lSA, NULL);
}


void * Default_Thread_Func(void *d) { 

Thread *t = (Thread*) d;

	return t->PreHandler();
};

#ifdef DO_SIGNAL_INFO

extern int main();

void Default_Fatal_Signal_Func(int signal, struct siginfo* info, void* something) { 

// zabezpieczenie przed podwojnym SIGSEGV

	static int got_signal = 0;
	if (got_signal) raise(SIGKILL); // aaaarrrrghhhhhhhh..... harakiri
	got_signal = 1;

// zobacz <siginfo.h>
// niestety nie dziala to w srodowisku wielowatkowym poniewaz na kazdy signal
// handler pthread naklada swoja funkcje pthread_sighandler, ktora nie
// uwzglednia struktury sa_sigaction (SA_SIGINFO)
// trzeba im maila wyslac

	int moved_stack;

	moved_stack = (int) &info;
	moved_stack += 6 * 4;

	info =  *((struct siginfo**) moved_stack);

	moved_stack = (int) &something;
	moved_stack += 6 * 4;

//	something =  *((void**) moved_stack);

// mimo tego zastosowalem trick, ktory polega na tym, ze wlazimy na stos 
// funkcji pthread_sighandler, ktora dostaje wlasciwe parametry dzieki 
// ustawieniu SA_SIGINFO
// Problem polega na tym, ze nie daje ona ich do naszej funkcji

// Bardzo niebezpieczna akcja, jesli zmieni sie np. liczba zmiennych lokalnych
// funkcji pthread_sighandler w nastepnej edycji libpthread to bedzie dupa

// code 08.12.1999

	char buf[100];

	switch (signal) {
		case SIGSEGV: sprintf(buf, "SIGSEGV (invalid address %p, code = %d)", 
					&(info->si_addr), info->si_code);
				break;
		default:	sprintf(buf, "signal %d was generated", signal);
				break;
	}

	cstable->logger->Log(buf);

	Thread *tmpThr = cstable->threadList->getThread( getpid() );
	longjmp( (tmpThr->getRetAddress()).retBuf, signal );

}

void Default_Terminate_Signal_Func(int signal, struct siginfo* info, void* something) { 

	Request *Req = new Req_Terminate( getpid() );
	cstable->manager->Send_Request( Req ); 
}

void Default_Exit_Signal_Func(int pSignal, struct siginfo* info, void* something) { 

	Codecook_Thread *tmpCThr = (Codecook_Thread*)  
			cstable->threadList->getThread( getpid() );

	//cstable->logger->Log( tmpCThr->GetName() );	

	if( !strcmp(tmpCThr->GetName(), "Manager") )
		longjmp( (tmpCThr->getRetAddress()).retBuf, pSignal );
	// - w innych watkach poza managerem sygnal nie robi nic..

}

#else

void Default_Fatal_Signal_Func(int signal) { 

	Thread *tmpThr = cstable->threadList->getThread( getpid() );
	longjmp( (tmpThr->getRetAddress()).retBuf, signal );

}

void Default_Terminate_Signal_Func(int signal) { 

	cstable->logger->Log("Got terminate signal, terminating");

	Request *Req = new Req_Terminate( getpid() );
	cstable->manager->Send_Request( Req ); 
}

void Default_Exit_Signal_Func(int pSignal) { 

	Codecook_Thread *tmpCThr = (Codecook_Thread*)  
			cstable->threadList->getThread( getpid() );

	//cstable->logger->Log( tmpCThr->GetName() );	

	if( !strcmp(tmpCThr->GetName(), "Manager") )
		longjmp( (tmpCThr->getRetAddress()).retBuf, pSignal );
	// - w innych watkach poza managerem sygnal nie robi nic..

}

#endif

pthread_t Thread::GetThreadId() const {
	return thread_id;
};

long Thread::UnsharedDataSize()
{
	char proc_name[256];
	int size, resident, share;
	int trs, lrs, drs, dt;

	snprintf(proc_name, sizeof(proc_name), "/proc/%d/statm", getpid());

	FILE *file = fopen(proc_name, "r");

	    if(!file) {
			cstable->logger->Log("UnsharedDataSize: Cannot open file");
			return -1;
	    }

    fscanf(file, "%d %d %d %d %d %d %d", &size, &resident, &share, 
	&trs, &lrs, &drs, &dt); 
    fclose(file);

	return size;
};


//
// Class Codecook_Thread
//

Codecook_Thread::Codecook_Thread(char *_name, ThreadTypes _type) 
 : Thread(_type), name(strdup(_name)) {
};

Codecook_Thread::~Codecook_Thread() {
	free(name);
};

char *Codecook_Thread::GetName() const {
	return name;
};

void Codecook_Thread::Logn(const char * s, ... ) {

	va_list args;
	va_start(args, s);

	cstable->logger->LockMe();

		cstable->logger->LognNL("(%s) : ", name);
		cstable->logger->LogvnNL(s, args);

	cstable->logger->UnlockMe();

	va_end(args);
};

void Codecook_Thread::Logvn(const char * s, va_list l ) {

cstable->logger->LockMe();

	cstable->logger->LognNL("(%s) : ", name);
	cstable->logger->LogvnNL(s, l);

cstable->logger->UnlockMe();

}

void Codecook_Thread::Log(const char * s, ... ) {

cstable->logger->LockMe();

	va_list args;
	va_start(args, s);

		cstable->logger->LognNL("(%s) : ", name);
		cstable->logger->LogvNL(s, args);

	va_end(args);

cstable->logger->UnlockMe();

}

//
// Class WorkingThreads
//

WorkingThreads::WorkingThreads() : Room() {
	
	threadList = new TList<SThreadInfo>;

}

SThreadInfo *WorkingThreads::getThreadInfo(int pid, LockOrNot lockFlag){

SThreadInfo *tmpInfo;

	if( lockFlag == LOCK ) EnterReader();

	TIterator<SThreadInfo> *i = new TIterator<SThreadInfo>(threadList);

	while( ! i->endReached() ){

		tmpInfo=i->next(); 
		if( tmpInfo->pid == pid ){
			delete i;
			if( lockFlag == LOCK ) LeaveReader();
			return tmpInfo;
		}

	}
delete i;
if( lockFlag == LOCK ) LeaveReader();
return NULL;

}

int WorkingThreads::getThreadPid(char *name, LockOrNot lockFlag)
{
SThreadInfo *tmpInfo;

	if( lockFlag == LOCK ) EnterReader();

	TIterator<SThreadInfo> *i = new TIterator<SThreadInfo>(threadList);

	while( ! i->endReached() ){

		tmpInfo=i->next(); 
		char *thr_name = ((Codecook_Thread*) tmpInfo->thrPtr)->GetName();

		if (!strcmp(thr_name, name) ){
			delete i;
			if( lockFlag == LOCK ) LeaveReader();
			return tmpInfo->pid;
		}

	}
	delete i;

	if( lockFlag == LOCK ) LeaveReader();

	return -1;
}

SThreadInfo *WorkingThreads::getThreadInfo(Thread *pThrPtr, 
		LockOrNot lockFlag ){

SThreadInfo *tmpInfo;

	if( lockFlag == LOCK ) EnterReader();

	TIterator<SThreadInfo> *i = new TIterator<SThreadInfo>(threadList);

	while( ! i->endReached() ){

		tmpInfo=i->next(); 
		if( tmpInfo->thrPtr == pThrPtr ){
			delete i;
			if( lockFlag == LOCK ) LeaveReader();
			return tmpInfo;
		}

	}

delete i;
if( lockFlag == LOCK ) LeaveReader();
return NULL;

}

Thread *WorkingThreads::getThread(int pid, LockOrNot lockFlag){

SThreadInfo *tmpInfo;

	if (lockFlag == LOCK) EnterReader();

	TIterator<SThreadInfo> *i = new TIterator<SThreadInfo>(threadList);

	while( ! i->endReached() ){

		tmpInfo=i->next(); 
		if( tmpInfo->pid == pid ){
			delete i;
			if (lockFlag == LOCK) LeaveReader();
			return tmpInfo->thrPtr;
		}

	}
delete i;
if (lockFlag == LOCK) LeaveReader();
return NULL;
}

char *WorkingThreads::getThreadTree(ThreadTypes type_to_show) {

SThreadInfo *tmpInfo;
#define MY_LINE_LEN 80
	string list = "";

	EnterReader();

	TIterator<SThreadInfo> *i = new TIterator<SThreadInfo>(threadList);

	while( ! i->endReached() ){

		tmpInfo=i->next(); 
		Codecook_Thread* lCThr = (Codecook_Thread*)tmpInfo->thrPtr;

		char *lThreadName = lCThr->GetName();
		int   lThreadPid = tmpInfo->pid;
		char *lThreadType =
			GetThreadTypeName(lCThr->GetType());
		pthread_t lThreadId = tmpInfo->id;

		if (type_to_show != UNKNOWN)
			if (type_to_show != lCThr->GetType()) continue;

		list += XMLgen::Thread(lThreadName, lThreadId, lThreadPid, 
		    lThreadType);
	}
delete i;
LeaveReader();
	return copy_string(list);
}

void WorkingThreads::AddThread(Thread *pThrPtr, int pPid, pthread_t pId){

SThreadInfo *lThrInfo = new SThreadInfo(pThrPtr, pPid, pId);

	EnterWriter();

	threadList->add(lThrInfo);

	LeaveWriter();
}

void WorkingThreads::EradicateThread(Thread *pThrPtr){

EnterWriter(); 

	SThreadInfo *tmpInfo = getThreadInfo(pThrPtr, NO_LOCK);

	if( tmpInfo ){
		tmpInfo->thrPtr->Cancel(); // should work but.. oh yes
						// it's Linux..

		tmpInfo->thrPtr->Join(NULL); // let's wait for finishing thread...
		delete tmpInfo->thrPtr;
		threadList->del(tmpInfo);
	}

LeaveWriter();
}

void WorkingThreads::EradicateThread(int pPid){

EnterWriter(); 

SThreadInfo *tmpInfo = getThreadInfo(pPid, NO_LOCK);

	if( tmpInfo ){
		tmpInfo->thrPtr->Cancel(); // should work but.. 
		tmpInfo->thrPtr->Join(NULL); // let's wait for finishing thread...

		delete tmpInfo->thrPtr;
		threadList->del(tmpInfo);
	}

LeaveWriter();
}

void WorkingThreads::DelThread(int pPid){

EnterWriter(); 

	SThreadInfo *tmpInfo = getThreadInfo(pPid, NO_LOCK);

	if( tmpInfo ){
		threadList->del(tmpInfo);
	}

LeaveWriter();

}

void WorkingThreads::DelThread(Thread *pThrPtr){

EnterWriter(); 

SThreadInfo *tmpInfo = getThreadInfo(pThrPtr, NO_LOCK);


	if( tmpInfo ){
		threadList->del(tmpInfo);
	}

LeaveWriter();

}
