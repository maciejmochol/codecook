#ifndef DEVICE_H
#define DEVICE_H

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "thread.h"
#include "command.h"
#include "cdtree.h"
#include "ptable.h"

class RegisteredTypeOfDevice;

// 
// class Device
//

class Device : public Codecook_Thread {
protected:
	Device_Param_Table *dpt;
	RegisteredTypeOfDevice *dev_type;

public:
	Device(char *, RegisteredTypeOfDevice*);

	virtual ~Device();
	virtual char *ExecCommand(char *);
	virtual void *Handler();

	void BeginAquisition();
	void EndAquisition();
	param_t MakeIntParam(string param, string unit="");
	param_t MakeDoubleParam(string param, string unit="");
	param_t MakeStringParam(string param);
	param_t GetHandle(string param);
	void SetParamChannel(string param, string channel, string value);
	void WriteParam(param_t handle, void * data);
};

//
// class CD_Node_Device
//

class CD_Node_Device : public CD_Node {
	Device* dev;
public:
	CD_Node_Device(Device* _dev);
	void Use() { };
};

// CD_PTableChannel 

class CD_Node_Param;

class CD_PTableChannel : public CD_Channel {
	CD_Node_Param *param_node;
public:
	CD_PTableChannel(CD_Node_Param &_param_node);
	
	virtual string get();
	virtual DataRecord GetDataRecord();
};


//
// class CD_Node_Param
//

class CD_Node_Param : public CD_Node {
	friend class Device;
	friend class CD_PTableChannel;
	Device* dev;
	DataTypes type;
	param_t handle;
public:
	CD_Node_Param(Device* _dev, param_t _handle, DataTypes _type);
	void Use() { };
};

#endif
