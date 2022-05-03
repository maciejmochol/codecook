#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "device.h"
#include "cstable.h"
#include "logger.h"
#include "device_list.h"


// Device

Device::Device(char *name, RegisteredTypeOfDevice *_dev_type) 
	: Codecook_Thread(name, DEVICE), dev_type(_dev_type)
{ 

	if (dev_type) dev_type->IncrCurCount();

	string device_name = "drivers:";
	device_name += GetName();

	// Param_Tables

	dpt = NULL;
	cstable->ptable->Create_Table(GetName());

	// CD_Tree...

	cstable->tree->InsertNode(device_name, new CD_Node_Device(this) );
}

Device::~Device() { 
	if (dpt) EndAquisition();
	cstable->ptable->Delete_Table(GetName());
	if (dev_type) dev_type->DecrCurCount();
	cout << "Device deleted" << endl;
}

void Device::BeginAquisition()
{
	dpt = cstable->ptable->Lock_Table_To_Aquisition(GetName());
}

void Device::EndAquisition()
{
	if (dpt) cstable->ptable->Unlock_Table_From_Aquisition(GetName());
	dpt = NULL;
}

void *Device::Handler()
{
	return NULL;
}

char *Device::ExecCommand(char *c) {
	return NULL;
};

param_t Device::MakeIntParam(string param, string unit)
{
	param_t h;

	dpt = cstable->ptable->Lock_Table_To_Write(GetName());
	h = dpt->AddInt();
	cstable->ptable->Unlock_Table_From_Write(GetName());
	dpt = NULL;

	string param_name = "drivers:" + string(GetName()) + ":" + param;

	CD_Node *lCDN;	
	cstable->tree->InsertNode(param_name, lCDN = 
			new CD_Node_Param(this, h, INT_VALUE));
	lCDN->AddChannel("UNIT", unit);

	return h;
}

param_t Device::MakeDoubleParam(string param, string unit)
{
	param_t h;

	dpt = cstable->ptable->Lock_Table_To_Write(GetName());
	h = dpt->AddDouble();
	cstable->ptable->Unlock_Table_From_Write(GetName());
	dpt = NULL;

	string param_name = "drivers:" + string(GetName()) + ":" + param;

	CD_Node *lCDN;	
	cstable->tree->InsertNode(param_name, lCDN = 
			new CD_Node_Param(this, h, DOUBLE_VALUE));
	lCDN->AddChannel("UNIT", unit);

	return h;
}

param_t Device::MakeStringParam(string param)
{
	param_t h;

	dpt = cstable->ptable->Lock_Table_To_Write(GetName());
	h = dpt->AddString();
	cstable->ptable->Unlock_Table_From_Write(GetName());
	dpt = NULL;

	string param_name = "drivers:" + string(GetName()) + ":" + param;

	cstable->tree->InsertNode(param_name, new CD_Node_Param(this, h, STRING_VALUE));

	return h;
}

param_t Device::GetHandle(string param)
{
	string param_name = "drivers:" + string(GetName()) + ":" + param;

	CD_Node* ref = cstable->tree->GetNode(param_name);

	cout << ref << " " << param_name << endl;

	if (!ref) return -1;
	else if (ref->GetChannel("TYPE") != "PARAM") return -1;
	else return ((CD_Node_Param*) ref)->handle;
}

void Device::SetParamChannel(string param, string channel, string value)
{
	string param_name = "drivers:" + string(GetName()) + ":" + param;

	CD_Node* ref = cstable->tree->GetNode(param_name);

	if (!ref) return;
	ref->AddChannel(channel, value);
}

void Device::WriteParam(param_t handle, void * data)
{
	if (dpt) dpt->Set(handle, data);
}


// CD_Node_Device

CD_Node_Device::CD_Node_Device(Device* _dev) : CD_Node()
{
	dev = _dev;
	AddChannel("TYPE", "DEVICE");
	AddChannel("DEVICE_NAME", dev->GetName());
};

// CD_PTableChannel

CD_PTableChannel::CD_PTableChannel(CD_Node_Param &_param_node)
{
	param_node = &_param_node;
	value_type = param_node->type;
}

string CD_PTableChannel::get()
{
	Device_Param_Table *dpt;
	Param_Rec param(value_type);

	dpt = cstable->ptable->Lock_Table_To_Aquisition(param_node->dev->GetName());
	if (!dpt) return "UNKNOWN";
	dpt->Get(param_node->handle, param);
	cstable->ptable->Unlock_Table_From_Aquisition(param_node->dev->GetName());
	return param.toString();
}

DataRecord CD_PTableChannel::GetDataRecord()
{
	Device_Param_Table *dpt;
	Param_Rec param(value_type);

	dpt = cstable->ptable->Lock_Table_To_Aquisition(param_node->dev->GetName());
	if (!dpt) return DataRecord();
	dpt->Get(param_node->handle, param);
	cstable->ptable->Unlock_Table_From_Aquisition(param_node->dev->GetName());

	switch (param.data_type) {
		case INT_VALUE: return DataRecord(DEFINITE, (int) param.data);

		case DOUBLE_VALUE: 	if (!param.data) return DataRecord();
					return DataRecord(DEFINITE, *((double*) param.data));

		case STRING_VALUE: 	if (!param.data) return DataRecord();
					return DataRecord(DEFINITE, string((char*) param.data));

		default:	return DataRecord();
	}
}

// CD_Node_Param

CD_Node_Param::CD_Node_Param(Device* _dev, param_t _handle, DataTypes _type) : CD_Node()
{
	dev = _dev;
	handle = _handle;
	type = _type;

	AddChannel("TYPE", "PARAM");
	AddChannel("DEVICE_NAME", dev->GetName());
	AddChannel("VALUE", new CD_PTableChannel(*this));
};
