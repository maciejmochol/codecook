/* ptable.h */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef __PTABLE_H__
#define __PTABLE_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <pthread.h>
#include <time.h>

#include "ccerror.h"
#include "blocking.h"
#include "datastruct.h"

// Class Device_Param_Table

enum DataTypes { INT_VALUE, DOUBLE_VALUE, STRING_VALUE } ; 
typedef int param_t;

struct _Param_Rec : public Monitor
{
	DataTypes data_type;
	int data_len;
	void *data;
	time_t timestamp;
	
	_Param_Rec(DataTypes type);
	 
	void TimeStamp();
	void Set(void *todata);		
	string toString();
	string Length();
};
typedef struct _Param_Rec Param_Rec;

class Device_Param_Table : public Room {
	friend class Param_Tables;
	Param_Rec* *Params;
	int counter, allocated;

	void realloc_array(int tosize);
	void BeginWR();
	void EndWR();

public:
	Device_Param_Table();
	~Device_Param_Table();

	void 	Get(param_t handle, Param_Rec &param);
	void 	Set(param_t handle, void *data);
	int 	GetCounter();

	param_t Add(DataTypes type);
	param_t AddInt();
	param_t AddDouble();
	param_t AddString();
};

// Class Param_Tables

class Param_Tables : public Room {
	TIndex<Device_Param_Table*> tables;
public:
	Param_Tables();
	~Param_Tables();
	
	void Create_Table(string device_name);
	void Delete_Table(string device_name);
	bool Exist_Table(string device_name);
	Device_Param_Table* Get_Table(string device_name);
	Device_Param_Table* Lock_Table_To_Aquisition(string device_name);
	void Unlock_Table_From_Aquisition(string device_name);
	Device_Param_Table* Lock_Table_To_Write(string device_name);
	void Unlock_Table_From_Write(string device_name);
};

#endif
