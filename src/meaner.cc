/* $Id: meaner.cc,v 1.5 1999/12/11 20:12:51 code Exp $ */


/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "meaner.h"
#include "ptable.h"
#include "cstable.h"
#include <unistd.h>
#include <stdio.h>

// 
// class Meaner
//

Meaner::Meaner() : Codecook_Thread("Meaner", MEANER) {
	Start();
};	



void *Meaner::Handler()
{
Request *r;

	Log("Meaner starting...");

	while(1) {
	//	if (req_buf.IsEmpty()) {
	//		Log("Request buffer of %s is empty", name); 
	//	}
		sleep(1);
		while ( !req_buf.IsEmpty()) {
			Log("%s gets request...", name);
			req_buf.Get(&r);

			delete r; // adresat niszczy requesta!
			Log("%s done.", name);
		};
	}
	return NULL;
};

