//
// Klasy, struktury, funkcje sluzace do blokowania
//

#ifndef __BLOCKING_H__
#define __BLOCKING_H__

#include <pthread.h>

#ifndef _REENTRANT
#define _REENTRANT
#endif

/*

UWAGA ! Co z dekonstruowaniem ?

Klasy zabezpieczane za pomoca Monitor lub Room 
zakladaja ze nie sa likwidowane - dekonstruowane.
Watek likwidujacy klase powinien dopilnowac zeby w tym czasie
zaden inny nie mial dostepu do klasy, oraz zeby po jej zlikwidowaniu
wszystkie watki o tym wiedzialy. Dlatego wskazniki na klasy wspoldzielone
powinny byc trzymane w jednym, centralnym miejscu - watki nie powinny
trzymac kopii wskaznikow klas wspoldzielonych.

Powinno sie zapewnic, aby chwila od odczytu wskaznika do wywolania metody 
byla atomowa. Niestety nie wiem jak to zrobic.

Sadze ze konstruowac i likwidowac klasy wspoldzielone powinien jeden watek
- czyli manager.

*/

class Monitor {
	pthread_mutex_t class_lock;
public:
	Monitor();
	~Monitor();	
	void LockMe();
	void UnlockMe();	
	pthread_mutex_t* GetClassLock();
};

class Room {
	int readers_in;
	int readers_waiting;
	int writers_in;
	int writers_waiting;
	pthread_mutex_t global_lock;
	pthread_cond_t readers;
	pthread_cond_t writers;
public:
	Room();
	virtual ~Room();
	void EnterReader();
	void LeaveReader();
	void EnterWriter();
	void LeaveWriter();
	bool IsReader();
	bool IsWriter();
};

#endif
