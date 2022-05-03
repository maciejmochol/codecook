/* $Id: netmanager.h,v 1.2 1999/12/06 18:33:56 code Exp $ */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _NETMANAGER_H_
#define _NETMANAGER_H_

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "thread.h"

// 
// class NetManager
//

class NetManager : public Codecook_Thread 
{
public:
	NetManager();
	virtual void * Handler();
};


#endif
