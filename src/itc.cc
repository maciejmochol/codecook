#include <stdlib.h>
#include <string.h>

#include "itc.h"

//
// Class Request
//

// Wszystko w interface'ach...

//
// Class Request_Buffer
//

Request_Buffer::Request_Buffer()
{
	pthread_cond_init(&notempty, NULL);
	pthread_cond_init(&notfull, NULL);
	readpos = writepos = 0;
}

Request_Buffer::~Request_Buffer()
{
}

void Request_Buffer::Put(Request *req)
{
	LockMe();
	
	while ((writepos + 1) % REQUEST_BUFFER_SIZE == readpos) {
	  pthread_cond_wait(&notfull, GetClassLock());
	}

	requests[writepos] = req;
	writepos++;
	if (writepos >= REQUEST_BUFFER_SIZE) writepos = 0;
	pthread_cond_signal(&notempty);

	UnlockMe();
}

void Request_Buffer::Get(Request **req)
{
	LockMe();

	*req = NULL;
	
	while (writepos == readpos) {
	  pthread_cond_wait(&notempty, GetClassLock());
	}
	*req = requests[readpos];
	readpos++;
	if (readpos >= REQUEST_BUFFER_SIZE) readpos = 0;
	pthread_cond_signal(&notfull);

	UnlockMe();
}

int Request_Buffer::IsEmpty()
{
	int i;

	LockMe();
	i = writepos == readpos;
	UnlockMe();
	return i;
}
