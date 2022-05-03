#include <unistd.h>

#include <device.h>
#include <logger.h>
#include <cstable.h>
#include "system_info.h"


#include <time.h>

//
// Register function
//

#ifdef __cplusplus
// Nie ma bata - ten plik sie nie przekompiluje bez __cplusplus
extern "C"
{
#endif

extern RegisteredTypeOfDevice *register_init(char *n, void *h) {
	return new System_Informator_Register(n,h);
}

#ifdef __cplusplus
}
#endif

//
//	System_Informator_Register
//

System_Informator_Register::System_Informator_Register(char *n, void *h) : 
		RegisteredTypeOfDevice(n, h) {

	comment = strdup("Pierwszy driver napisany przez Vooyecka, przerobiony"
		    " przez Joja i Codematica. Zbiera dane o systemie komputerowym.");

	SetMaxCount(2);
};

System_Informator_Register::~System_Informator_Register() {
printf("jestem w destruktorze\n");
};

Device *System_Informator_Register::CreateDevice(char *dev_name, Command *c) {
	return new System_Informator(dev_name, this);
};

//
//	System_Informator
//


System_Informator::System_Informator(char *name, RegisteredTypeOfDevice *dev_type) 
	: Device(name, dev_type)
{
	mem_free = swap_free = -1;
}

System_Informator::~System_Informator()
{
}

void *System_Informator::Handler()
{
	cstable->logger->Log("Device SYSINFO started...");

	int ostype = MakeStringParam("OSTYPE");
	SetParamChannel("OSTYPE", "NAME", "Operating system type");

	int osrelease = MakeStringParam("OSRELEASE");
	SetParamChannel("OSRELEASE", "NAME", "Operating system release");

	int free_mem_phis = MakeIntParam("FREEMEM_PHYSICAL");
	SetParamChannel("FREEMEM_PHYSICAL", "NAME", "Amount of free system memory");

	int free_mem_swap = MakeIntParam("FREEMEM_SWAP");
	SetParamChannel("FREEMEM_SWAP", "NAME", "Amount of free swap memory");

	int probes = MakeDoubleParam("probes");
//	SetParamChannel("FREEMEM_SWAP", "NAME", "Amount of free swap memory");


	#define DUMMIES 10000

	int dummy[DUMMIES];
	int j;
	char dum_name[10];

/*
	for (j = 0; j < DUMMIES; j++) {
		sprintf(dum_name, "DUMMY%d", j);
		dummy[j] = MakeIntParam(dum_name);
	}
*/

	char cbuffer[20];

	BeginAquisition();

	GetOsType(cbuffer);
	WriteParam(ostype, cbuffer);

	GetOsRelease(cbuffer);
	WriteParam(osrelease, cbuffer);

	EndAquisition();

	int ibuffer[2];

	int probe = 0;
	time_t begin_time, curr_time, dtime;
	double prob = 0.0;

	begin_time = time(NULL);

	while(1) {

		probe++;
		curr_time = time(NULL);
		dtime = curr_time - begin_time;
		if (dtime > 0)
			prob = (double) probe	/ (double) dtime;	

		//char buf[100];
		//sprintf(buf, "T: %d P: %d\n", dtime, probe);
		//Log(buf);

		BeginAquisition();
		GetFreeMem(ibuffer);
		if( ibuffer[0] != mem_free ){
			WriteParam(free_mem_phis, &(ibuffer[0]));
		}

		if( ibuffer[1] != swap_free ){
			WriteParam(free_mem_swap, &(ibuffer[1]));
		}

		WriteParam(probes, &prob);
		EndAquisition();

		mem_free = ibuffer[0]; swap_free= ibuffer[1];

		pthread_testcancel();
		sleep(20);
	}

return NULL;
}


///////////////////
// Do poprawy : uzywa sysctl, niektoore kernele tego nie maja..

void System_Informator::GetOsType(char *buffer)
{
char filenam[] = "/proc/sys/kernel/ostype";
FILE *file = fopen(filenam, "r");

    if(!file) {
	Log("Cannot open file '%s'", filenam);
	strcpy(buffer, "UNRECOGNIZED");
	return;
    }

    fscanf(file, "%s", buffer); 
    fclose(file);
}	

void System_Informator::GetOsRelease(char *buffer)
{
char filenam[] = "/proc/sys/kernel/osrelease";
FILE *file = fopen(filenam, "r");

    if(!file) {
	Log("Cannot open file '%s'", filenam);
	strcpy(buffer, "UNRECOGNIZED");
	return;
    }

    fscanf(file, "%s", buffer); 
    fclose(file);
}

void System_Informator::GetFreeMem(int buffer[])
{
FILE *file = fopen("/proc/meminfo", "r");

    if(!file) {
	cstable->logger->Log("Cannot open file");
	buffer[0] = 0; buffer[1] = 0;
	return;
    }

    int c, cnt=0, mf=0, sf=0; 
    char string1[] = "MemFree:", string2[] = "SwapFree:";

    while( (c=fgetc(file)) != EOF ){
		if(string1[cnt] == c){
			cnt++;	
			if( c == ':' ){
				fscanf(file, "%d", &mf);	
				break;
			}
		} else
			cnt = 0;
	}

    cnt = 0;
    while( (c=fgetc(file)) != EOF ){
		if(string2[cnt] == c){
			cnt++;	
			if( c == ':' ){
				fscanf(file, "%d", &sf);	
				break;
			}
		} else
			cnt = 0;
	}

     fclose(file);

     buffer[0] = mf;
     buffer[1] = sf;
}
