/* ccerror.h */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef __MEANER_H__
#define __MEANER_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "thread.h"

class Request;

//class Meaner;

// 
// class Meaner
//

class Meaner : public Codecook_Thread {
public:
	Meaner(); 

	virtual void * Handler();
};

#endif
