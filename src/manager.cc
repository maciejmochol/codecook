/* $Id: manager.cc,v 1.18 2000/11/05 21:20:55 code Exp $ */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#define _GNU_SOURCE
#include <string.h>
#include <unistd.h>

#include "cstable.h"
#include "thread.h"
#include "manager.h"
#include "parsing.h"
#include "cdtree.h"
#include "cddb.h"

#include "meaner.h"
#include "netmanager.h"
#include "alarmserv.h"
#include "logger.h"
#include "device_list.h"
#include "terminal.h"
#include "xmlgen.h"

class CAlarmList;
class UserList;

#include <signal.h>

// 
// class Manager
//

Manager::Manager() 
{
};

void Manager::Send_Request(Request *req)
{
	req_buf.Put(req);
}


void Manager::Start()
{

// Sygnaly

	struct sigaction lSA;

	memset((void*) &lSA, 0, sizeof(struct sigaction));

	lSA.sa_flags   = SA_RESTART;
	lSA.sa_handler = SIG_IGN;
	lSA.sa_restorer= NULL ;

//	sigaction(SIGTTOU, &lSA, NULL);

// Koniec

	cstable = new Codecook_System_Table();

	cstable->tree = new CD_Tree();


	// Nie zmieniac kolejnosci !!!

	cstable->manager = this;
	cstable->logger = new Logger();
	cstable->devices = new Device_List();
	cstable->threadList = new WorkingThreads();
	cstable->ptable = new Param_Tables();

	cstable->meaner = new Meaner();
	cstable->netmanager = new NetManager();
	cstable->alarmserv = new AlarmServ();
	cstable->dbms = Create_DBMS();

	// Manager must have a special signal handling..
	// ... ? FIX ME

		// Loading Tree from a file..
	cstable->tree->Load("./conf/tree.conf");

		// Adding ADMIN THREADS command
	cstable->protocol->AddCommand("ADMIN THREADS", new AdminListThreadsCommand(),"");

		// Executing script file..
	Terminal_RunScript *pRS = new Terminal_RunScript("./conf/rc.conf"); 
	pRS->Join(NULL);
	
		// Starting console terminal..
	new Terminal_Console();


	//char buff[300];
	Request *r;
	
	while(1) {
		req_buf.Get(&r);

		if( r->GetType() == THREAD_FAULT ){
			map_request(Req_Fault, r, _r);

			Thread *lThr = cstable->threadList
					->getThread(_r->GetPid()); 
			Codecook_Thread*lCThr = (Codecook_Thread*) lThr;

			Log("Deleting FAULT thread '%s' (pid: %d)",
				lCThr->GetName(), _r->GetPid());
			cstable->threadList->
				EradicateThread(_r->GetPid()); 
			Log( "Thread destroyed" " because of %s.", 
				strsignal(_r->GetSig()));
			delete _r;
		} else if( r->GetType() == THREAD_TERMINATE ){
			map_request(Req_Terminate, r, _r);

			Thread *lThr = cstable->threadList
					->getThread(_r->GetPid()); 
			Codecook_Thread*lCThr = (Codecook_Thread*) lThr;

			if (lCThr == NULL) {

				printf("Bad request !!!\n");
				printf("Seeked pid: %d\n", 
					_r->GetPid() );

			} else {
			cstable->logger->Log(
			"Deleting TERMINATED thread '%s' (pid : %d)",
			lCThr->GetName(), _r->GetPid());

			cstable->threadList->EradicateThread(
					_r->GetPid());
			cstable->logger->Log( 
				"Thread finished its job.");
				delete _r; 
			}
		}

			//sprintf(buff, "manager done.");
			//cstable->logger->Log(buff);
	}
};


void Manager::Logn(const char * s, ... ) {

cstable->logger->LockMe();

	va_list args;
	va_start(args, s);

		cstable->logger->LogNL("(Manager) : ");
		cstable->logger->LogvnNL(s, args);

	va_end(args);

cstable->logger->UnlockMe();
};

void Manager::Log(const char * s, ... ) {

cstable->logger->LockMe();

	va_list args;
	va_start(args, s);

		cstable->logger->LognNL("(Manager) : ");
		cstable->logger->LogvNL(s, args);
	
	va_end(args);

cstable->logger->UnlockMe();
}

//
// AdminListThreads
// CD_Tree command
//

char *AdminListThreadsCommand::execute(Command *command){
	
	string s = XMLgen::Command(command);

	s += XMLgen::Admin_threads_res(cstable->threadList->getThreadTree());
	return copy_string(s);
}
