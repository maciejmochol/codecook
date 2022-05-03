#ifndef __DRIVER_1_H__
#define __DRIVER_1_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "device.h"
#include "execute_c.h"



//
//pierwszy driver dla sterownika malego by Klenio.
//
//protokol:
//^Q - poczatek komunikacji
//^BP1^C - 1 - numer sterownika mozna zamienic na dow. inny, dostaje parametry
//^B&1^C - 1 - analogicznie, PNP - zwraca opisy parametrow
//^BT11999/11/25 23:15^C - pierwsza 1 analog. reszta wiadomo.




class NewDriverAddCommand : public ExecuteCommand {
public:
	NewDriverAddCommand();
	~NewDriverAddCommand();
	virtual char * execute(Command *);
};





class Driver_1 : public Device {
	char *dev_name;
public:
	Driver_1(char *, int, char * );
	virtual ~Driver_1();
	virtual void *Handler();
};

#endif
