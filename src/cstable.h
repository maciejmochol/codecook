#ifndef __CSTABLE_H__
#define __CSTABLE_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "blocking.h" // Monitor definition required for cstable

class Protocol;
class Meaner;
class NetManager;
class Manager;
class AlarmServ;
class Param_Tables;
class Logger;
class Device_List;
class Monitor;

class CAlarmList;
class UserList;
class WorkingThreads;

class CD_Tree;
class DB_DBMS;

//
// Class Codecook_System_Table
//

class Codecook_System_Table : public Monitor {
public:
	Meaner* meaner;
	NetManager* netmanager;
	Manager* manager;
	AlarmServ* alarmserv; 
	Param_Tables *ptable; 
	Logger* logger; 
	Device_List *devices; 
	CAlarmList *alarmList;
	UserList   *userList;
	Protocol *protocol;
	WorkingThreads *threadList;
	CD_Tree *tree;
	DB_DBMS *dbms;
	
	Codecook_System_Table();
};

extern Codecook_System_Table *cstable;

#endif
