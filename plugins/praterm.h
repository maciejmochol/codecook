#ifndef __DRIVER_1_H__
#define __DRIVER_1_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <termios.h>


#include <device.h>
#include <command.h>
#include <datastruct.h>



//
//pierwszy driver dla sterownika malego by Klenio.
//
//protokol:
//^Q - poczatek komunikacji
//^BP1^C - 1 - numer sterownika mozna zamienic na dow. inny, dostaje parametry
//^B&1^C - 1 - analogicznie, PNP - zwraca opisy parametrow
//^BT11999/11/25 23:15^C - pierwsza 1 analog. reszta wiadomo.


//
//	to jest klasa do protokolu:
//	dodaje komende dodajaca driver z parametrami.
//


class NewDriverAddCommand : public ExecuteCommand {
public:
	NewDriverAddCommand();
	~NewDriverAddCommand();
	virtual char * execute(Command *);
};

//
//	to jest klasa do protokolu:
//	dodaje komendy ustawiajace parametry w driverze i sterowniku.
//


class NewDriverCommand : public ExecuteCommand {
public:
	NewDriverCommand();
	~NewDriverCommand();
	virtual char * execute(Command *);
};



class Praterm_Register : public RegisteredTypeOfDevice {
public:
	Praterm_Register(char *, void *);
	virtual ~Praterm_Register();
	Device *CreateDevice(char *name, Command *);
	virtual void *Handler();
};



class Praterm : public Device {
	char dev_name[100];
	int dev_num;	//numer ktory jest pamietany w
			//sterowniku - fizycznym urzadzeniu
	char *q_string;	//
	char *t_string;
	char *pnp_string;
	int q_len;
	int pnp_len;
	int t_len;
	int pnp_mode;
	
	int status;	//czy wszystko jest ok.
	int descr;
	int param_len;
	int *param_list;    // parametry ze sterownika
	int *param_handles; // handles z param table
	
	struct termios old_term_conf;	
	time_t lastGoodReport;
	time_t lastGoodDateSet;
	int reportInterval;
	int maxWait;
	
	int OpenLine();
	int askForReport();
	int askForPnPReport();
	DynamicBuffer *readReport();
	int setIOtimeout(int, int );
	int parseReport(DynamicBuffer *);
	int getNumberOfParams();
	int getReport();
	int testControlSum(char *);
	int getPnPReport();
	int ParsePnPReport(DynamicBuffer*);
	
	int setTime();
	int getConfig();
	int setConfig();
	int setMaxWait(int );
	int getMaxWait();
public:
	Praterm(char *, RegisteredTypeOfDevice*);
	virtual ~Praterm();
	virtual void *Handler();
	virtual char *ExecCommand(char *);
	int workingOk();
};

#endif
