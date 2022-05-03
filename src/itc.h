//
// Inter-Thread Communication
// Mechanizmy do komunikacji miedzywatkowej, w przyszlosci
// oprocz requestow moze cos asynchronicznego na sygnalach
//

#ifndef __ITC_H__
#define __ITC_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif


#include "codecook.h"
#include "blocking.h"
#include "ptable.h"

class Device;

// Class Request

typedef enum { EMPTY, THREAD_FAULT, THREAD_TERMINATE } Request_Type;

#define map_request(type, base, mapped) type *mapped = (type*) base
	// Makro "mapuje" klase Request na podklase type
	// dane type: podklasa, base: nazwa zm. request,  mapped: nazwa nowej zm.
	// np. map_request(Req_Terminate, r, _r)

class Request {

protected:
	Request_Type type;

public:
	Request() { type = EMPTY; };
	~Request() { };
	
	Request_Type GetType() { return type; }
};

// Class Request_Buffer

#define REQUEST_BUFFER_SIZE 1000

class Request_Buffer : public Monitor {

	Request *requests[REQUEST_BUFFER_SIZE];
	pthread_cond_t notempty, notfull;	
	int readpos, writepos;
		
public:	

	Request_Buffer();
	~Request_Buffer();
	
	void Put(Request *req);
	void Get(Request **req);
	int IsEmpty();
};



#endif
