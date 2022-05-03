/* ptable.c */

/* Copyright <C> 1999 by Codematic Solutions */

/* param table */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdlib.h>
#include <string.h>

#include "datastruct.h"
#include "ptable.h"
#include "ccerror.h"

#include "cstable.h"
#include "logger.h"

// Param_Rec

_Param_Rec::_Param_Rec(DataTypes type) : Monitor()
{
	data_len = 0;
	data = NULL;
	data_type = type;
}

void _Param_Rec::TimeStamp()
{
	timestamp = time(NULL);
}

void _Param_Rec::Set(void *todata)
{
	LockMe();
	switch (data_type) {
		case INT_VALUE: 
				// nie alokuje pamieci, ustawia wskaznik na
				// wartosc spod adresu todata
				data =  (void*) (* (int *) todata); 
				data_len = sizeof(int);
				break;
		case DOUBLE_VALUE: 
				if (!data)
				  data = (void*) malloc(sizeof(double));
				memcpy(data, todata, sizeof(double));	
				data_len = sizeof(double);
				break;
		case STRING_VALUE: 
				if (data) free(data);
				data = strdup((char*) todata);
				data_len = strlen((char*) todata);
				break;
	}
	TimeStamp();
	UnlockMe();
}

//
// Uwaga
// trzeba przemyslec korzystanie z paramow, a wlasciwie to napisac
// konstruktor copy (patrz Get())
//

string _Param_Rec::toString()
{
	string s;
	char buf[100];

//	LockMe();
		switch (data_type) {
			case INT_VALUE: 	sprintf(buf, "%d", (int) data);
						s = string(buf); break;

			case DOUBLE_VALUE: 	if (!data) { s = "UNKNOWN"; break; }
						sprintf(buf, "%f", *((double*) data));
						s = string(buf); break;

			case STRING_VALUE: 	if (!data) { s = "UNKNOWN"; break; }
						s = string((char*) data); break;
		}
//	UnlockMe();

	return s;
}

string _Param_Rec::Length()
{
	string s;

//	LockMe();
		char buf[100];
		sprintf(buf, "%d", data_len);
		s = string(buf);
//	UnlockMe();
	
	return s;
}

// Device_Param_Table

Device_Param_Table::Device_Param_Table() 
{
	counter = allocated = 0;
	Params = NULL;
}

void Device_Param_Table::realloc_array(int tosize)
{

	int tmp = 1;

	while (tmp < tosize) {
		tmp *= 2;
	};

	if (tmp == allocated) return;

	allocated = tmp;
	Params = (Param_Rec**) realloc(Params, allocated * sizeof(Param_Rec*));
}


Device_Param_Table::~Device_Param_Table()
{
	int i;
	if (counter == 0) return;
	for (i = 0; i < counter; i++)
		delete Params[i];
	delete Params;
}

param_t Device_Param_Table::Add(DataTypes type)
{
	if (!IsWriter())
		throw exc_Device_Param_Table_ShouldBeWriter("Device_Param_Table::Add");

	Param_Rec *param;
	param_t retval;
	
	param = new Param_Rec(type);

	realloc_array(counter+1);
	counter++;
	Params[counter - 1] = param;
	retval = counter - 1;
	return retval;
}

param_t Device_Param_Table::AddInt()
{
	return Add(INT_VALUE);
}

param_t Device_Param_Table::AddDouble()
{
	return Add(DOUBLE_VALUE);
}

param_t Device_Param_Table::AddString()
{
	return Add(STRING_VALUE);
}

void Device_Param_Table::BeginWR()
{
	EnterReader();
}

void Device_Param_Table::EndWR()
{
	LeaveReader();
}

void Device_Param_Table::Get(param_t handle, Param_Rec &param)
{
	if (!IsReader())
		throw exc_Device_Param_Table_ShouldBeReader("Device_Param_Table::Get");
	if (handle >= 0 && handle < counter) { 
		Params[handle]->LockMe();
		param = *(Params[handle]); 
		Params[handle]->UnlockMe();
	}
	else throw exc_Device_Param_Table_BadHandle("Device_Param_Table::Get");
}

void Device_Param_Table::Set(param_t handle, void *data)
{
	Param_Rec *p = NULL;

	if (!IsReader())
		throw exc_Device_Param_Table_ShouldBeReader("Device_Param_Table::Set");
	if (!data)
		throw exc_Device_Param_Table_BadData("Device_Param_Table::Set");
	if (handle < 0 || handle >= counter)
		throw exc_Device_Param_Table_BadHandle("Device_Param_Table::Set");

	p = Params[handle];
	
	p->Set(data);
}

int Device_Param_Table::GetCounter()
{
	if (!IsReader())
		throw exc_Device_Param_Table_ShouldBeReader("Device_Param_Table::GetCounter");
	return counter;
}

// Param_Tables

Param_Tables::Param_Tables()
{
}

Param_Tables::~Param_Tables()
{
	TIndexIterator<Device_Param_Table*> it(tables);
	Device_Param_Table **dpt;

	do {
		dpt = it.next();
		if (dpt && *dpt) delete *dpt;
	} while (dpt);	
}


void Param_Tables::Create_Table(string device_name) 
{
	EnterWriter();
	Device_Param_Table *newtab = new Device_Param_Table();
	try {
	tables.insert(device_name, newtab);	 
	}
	catch (exc_TIndex_AlreadyExists) {
		delete newtab;
		LeaveWriter();
		throw exc_Param_Tables_TableExists("Param_Tables::Create_Table");
	}
	LeaveWriter();
}

void Param_Tables::Delete_Table(string device_name) 
{ 
	EnterWriter();
	Device_Param_Table **tab = tables.get(device_name);
	if (tab && *tab) {
		Device_Param_Table *pt = *tab;
		pt->EnterWriter();
		tables.del(device_name);
		delete pt;
	}
	else {
		LeaveWriter();
		throw exc_Param_Tables_TableNotFound("Param_Tables::Delete_Table");
	}
	LeaveWriter();
}

bool Param_Tables::Exist_Table(string device_name) 
{ 
	bool retval;

	EnterReader();
	retval = (tables.find(device_name) > -1);
	LeaveReader();

	return retval;
}

Device_Param_Table* Param_Tables::Get_Table(string device_name) 
{ 
	Device_Param_Table **tab;

	EnterReader();
	tab = tables.get(device_name);
	LeaveReader();
	if (tab)
		return *tab;
	else return NULL;
}

Device_Param_Table* Param_Tables::Lock_Table_To_Aquisition(string device_name) 
{ 
	EnterReader();
	Device_Param_Table *tab = Get_Table(device_name);
	
	if (tab) {
		tab->BeginWR();
	}
	else {
		LeaveReader();
		throw exc_Param_Tables_TableNotFound("Param_Tables::Lock_Table_To_Aquisition");
	}
	LeaveReader();
	return tab;
}

void Param_Tables::Unlock_Table_From_Aquisition(string device_name) 
{ 
	EnterReader();
	Device_Param_Table *tab = Get_Table(device_name);
	
	if (tab) {
		tab->EndWR();
	}
	else {
		LeaveReader();
		throw exc_Param_Tables_TableNotFound("Param_Tables::Unlock_Table_From_Aquisition");
	}
	LeaveReader();
}

Device_Param_Table* Param_Tables::Lock_Table_To_Write(string device_name) 
{ 
	EnterReader();
	Device_Param_Table *tab = Get_Table(device_name);
	
	if (tab) {
		tab->EnterWriter();
	}
	else {
		LeaveReader();
		throw exc_Param_Tables_TableNotFound("Param_Tables::Lock_Table_To_Write");
	}
	LeaveReader();
	return tab;
}

void Param_Tables::Unlock_Table_From_Write(string device_name) 
{ 
	EnterReader();
	Device_Param_Table *tab = Get_Table(device_name);
	
	if (tab) {
		tab->LeaveWriter();
	}
	else {
		LeaveReader();
		throw exc_Param_Tables_TableNotFound("Param_Tables::Unlock_Table_From_Write");
	}
	LeaveReader();
}
