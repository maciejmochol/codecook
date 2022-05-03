#ifndef __SYSTEM_INFO_H__
#define __SYSTEM_INFO_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <device.h>
#include <device_list.h>

class System_Informator_Register : public RegisteredTypeOfDevice {
public:
	System_Informator_Register(char *, void *);
	~System_Informator_Register();
	Device *CreateDevice(char *dev_name, Command *);
};


class System_Informator : public Device  
{
	// Kazdy Device ma wlasny bufor:

	// W tym przypadku - wersja i system sa niezmienne 
	int mem_free, swap_free; // a te rzeczy sie zmieniaja..	
	void GetFreeMem(int []);	
	void GetOsType(char *);
	void GetOsRelease(char *);

public:
	System_Informator(char *, RegisteredTypeOfDevice*);

	~System_Informator();
	void *Handler();
	
};

#endif
