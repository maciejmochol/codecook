#ifndef __DEVICE_LIST_H__
#define __DEVICE_LIST_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdlib.h>
#include <time.h>

#include "device.h"
#include "datastruct.h"
//#include "cstable.h"
#include "thread.h"
#include "command.h"

//
// class RegisteredTypeOfDevice
//

class RegisteredTypeOfDevice : Room {
friend class Device;

protected:
	void *handle;
	time_t working_since;
	char *comment;

private:
	char *name;
	int max_count;
	int current_count;

	void IncrCurCount();
	void DecrCurCount();

public:
	RegisteredTypeOfDevice(char *, void *);
	virtual ~RegisteredTypeOfDevice();
	char *GetName() const;

	//zwraca maksymalna liczbe tego typu urzadzen w systemie
	int GetMaxCount();
	//ustawia maksymalna liczbe tego typu urzadzen w systemie
	void SetMaxCount(int );

	int GetCurCount();
	void* GetLibHandle() { return handle; };

	time_t *GetStartTime();
	char *GetComment();

	Device *NewDevice(Command *);
	virtual Device *CreateDevice(char * _name, Command *) = 0;
	//uwaga powinno byc pure virtual

	char *DeviceExecCommand(char *, int, char *);
};

//
// class DeviceList
//

class Device_List
{
	TListRoom<RegisteredTypeOfDevice> *reg_list;
	RegisteredTypeOfDevice*	privateFindRegistered(char *);
	RegisteredTypeOfDevice * privateRegisterDevice(char *, char *directory = NULL);
	Device *privateGetDevice(char *, LockOrNot lockFlag = LOCK);
public:
	Device_List();

	int  	AddDevice(char *name, Command *);
	int  	DelDevice(char *);
	RegisteredTypeOfDevice * RegisterDevice(char *, char *directory = NULL);
	int	UnregisterDevice(char *);
	RegisteredTypeOfDevice*	FindRegistered(char *);  //???
	int	ReRegisterDevice(char *);
	int	RegisterAllDevices(char *);
	int	UnregisterAllDevices();
	int	ReRegisterAllDevices(char *);
	char *ShowWorkingDevices();
	char *ShowRegisteredDevices(int );
	char *DeviceExecCommand(char *, char *);
	
	int GetParams(char *buffer, int size_of_buffer);
	int GetParams(char* device_name, char *buffer, int size_of_buffer);
};

//
// Protokol
//

class DeviceListWorkingCommand : public ExecuteCommand {
public:
	DeviceListWorkingCommand() { };
	~DeviceListWorkingCommand() { };
	virtual char * execute(Command *);
};

class DeviceListRegisteredCommand : public ExecuteCommand {
public:
	DeviceListRegisteredCommand() { };
	~DeviceListRegisteredCommand() { };
	virtual char * execute(Command *);
};

class DeviceAddCommand : public ExecuteCommand {
public:
	DeviceAddCommand() { };
	~DeviceAddCommand() { };
	virtual char * execute(Command *);
};

class DeviceDeleteCommand : public ExecuteCommand {
public:
	DeviceDeleteCommand() { };
	~DeviceDeleteCommand() { };
	virtual char * execute(Command *);
};

class DeviceRegisterCommand : public ExecuteCommand {
public:
	DeviceRegisterCommand() { };
	~DeviceRegisterCommand() { };
	virtual char * execute(Command *);
};

class DeviceReRegisterCommand : public ExecuteCommand {
public:
	DeviceReRegisterCommand() { };
	~DeviceReRegisterCommand() { };
	virtual char * execute(Command *);
};

class DeviceUnregisterCommand : public ExecuteCommand {
public:
	DeviceUnregisterCommand() { };
	~DeviceUnregisterCommand() { };
	virtual char * execute(Command *);
};

class DeviceRegisterAllCommand : public ExecuteCommand {
public:
	DeviceRegisterAllCommand() { };
	~DeviceRegisterAllCommand() { };
	virtual char * execute(Command *);
};

class DeviceUnregisterAllCommand : public ExecuteCommand {
public:
	DeviceUnregisterAllCommand() { };
	~DeviceUnregisterAllCommand() { };
	virtual char * execute(Command *);
};

class DeviceReRegisterAllCommand : public ExecuteCommand {
public:
	DeviceReRegisterAllCommand() { };
	~DeviceReRegisterAllCommand() { };
	virtual char * execute(Command *);
};

#endif

