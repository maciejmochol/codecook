
#define __USE_GNU 1
#define _GNU_SOURCE 1

#include <unistd.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>

#include "cstable.h"
#include "device_list.h"
#include "logger.h"
#include "thread.h"
#include "protocol.h"
#include "manager.h"
#include "xmlgen.h"

#define MAX_STR 500

//
// Uwaga
// Najlepiej jak najmniej tu robic i dodawac
// Nie ma juz device listy tylko operacje 
// przeprowadzane sa bezposrednio na threadach
// ktore moga sie konczyc niezaleznie od tego kodu
// i jest to bugogenne (deadlocki, core dumpy, itp)
// -code
//

//
//ta funkcja jest uzywana do wybierania pluginow w katalogu.
//

int selPlugin(const struct dirent *dir) {
	if( strncmp(dir->d_name, "libCodecook_", 12 ) == 0) {
		int n = strlen(dir->d_name);
		
		if ( n >= 16 && strcmp(dir->d_name + n - 3, ".so") == 0 )
			return 1;
		else
			return 0;
	} else
		return 0;
};


//
//	RegisteredTypeOfDevice
//

RegisteredTypeOfDevice::RegisteredTypeOfDevice(char *n, void *h) {
	handle = h;
	max_count = 0;
	current_count = 0; 
	name = strdup(n);
	time(&working_since);
	comment = "no description";
};

RegisteredTypeOfDevice::~RegisteredTypeOfDevice() {
	//comment jest static (czy to jest dobre okreslenie??) 
	//wiec go nie usuwaj - bo bedzie segfault.
	if(name == NULL)
		free(name);
};

char *RegisteredTypeOfDevice::GetName() const {
	return name;
};

time_t *RegisteredTypeOfDevice::GetStartTime() {
	return &working_since;
};

//zwraca maksymalna liczbe tego typu urzadzen w systemie
int RegisteredTypeOfDevice::GetMaxCount() {
	return max_count; 
};

//ustawia maksymalna liczbe tego typu urzadzen w systemie
void RegisteredTypeOfDevice::SetMaxCount(int count) {
	max_count = count;
};

int RegisteredTypeOfDevice::GetCurCount() {
	return current_count;
};

void RegisteredTypeOfDevice::IncrCurCount() {
	current_count++;
};

void RegisteredTypeOfDevice::DecrCurCount() {
	current_count--;
};

char *RegisteredTypeOfDevice::GetComment() {
	return strdup(comment);
};

Device* RegisteredTypeOfDevice::NewDevice(Command *cmd)
{
	EnterWriter();

	int count = GetCurCount();
	int max = GetMaxCount();

	if (count + 1 > max && max != 0)  {
		LeaveWriter();
		return NULL;
	}

	DynamicBuffer new_name;

	char num[16];

	snprintf(num, sizeof(num), "%d", count + 1);

	new_name << name;
	new_name << "_";
	new_name << num;
	char *c = new_name.toString();

	Device *dev;
	try {
		dev = CreateDevice(c, cmd);
	}
	catch (exc_Codecook &exc) {
		cstable->logger->Log("NewDevice got exception from CreateDevice");
		cout << "NewDevice: " << exc;
		// no to awaria
		dev = NULL;
	}
	LeaveWriter();

	if (dev) dev->Start(); // dopiero teraz startujemy watek...

	delete c;

	return dev;
}


//
//	Device_List
//

Device_List::Device_List() {
	reg_list = new TListRoom<RegisteredTypeOfDevice>; 

	cstable->protocol->AddCommand("DEVICE LIST WORKING <??????>", new DeviceListWorkingCommand(),
	"DEVICE LIST WORKING <level>\n\twhere <level> is a number indicating"
	" how many information about devices you want"
	" 0 means only names, the bigger number, the more information you will get");

	cstable->protocol->AddCommand("DEVICE LIST WORKING", new DeviceListWorkingCommand(), "dupa opis 1");
	cstable->protocol->AddCommand("DEVICE LIST REGISTERED <??????>", new DeviceListRegisteredCommand(), "dupa opis 1");
	cstable->protocol->AddCommand("DEVICE LIST REGISTERED", new DeviceListRegisteredCommand(), "dupa opis 1");
	cstable->protocol->AddCommand("DEVICE ADD <??????>", new DeviceAddCommand(), "pupa opis 2");
	cstable->protocol->AddCommand("DEVICE DELETE <??????>", new DeviceDeleteCommand(), "pupa opis 2");
	cstable->protocol->AddCommand("DEVICE DELETE <??????> <??????>", new DeviceDeleteCommand(), "pupa opis 2");
	cstable->protocol->AddCommand("DEVICE REGISTER <??????>", new DeviceRegisterCommand(), "pupa opis 2");
	cstable->protocol->AddCommand("DEVICE REGISTER ALL <??????>", new DeviceRegisterAllCommand(), "pupa opis 2");
	cstable->protocol->AddCommand("DEVICE UNREGISTER <??????>", new DeviceUnregisterCommand(), "pupa opis 2");
	cstable->protocol->AddCommand("DEVICE UNREGISTER ALL", new DeviceUnregisterAllCommand(), "pupa opis 2");
	cstable->protocol->AddCommand("DEVICE REREGISTER ALL", new DeviceReRegisterAllCommand(), "pupa opis 2");
	cstable->protocol->AddCommand("DEVICE REREGISTER <??????>", new DeviceReRegisterCommand(), "pupa opis 2");

}


int Device_List::AddDevice(char *name, Command *command) {

	//sprawdz czy jest zarejestrowany driver tego device
	RegisteredTypeOfDevice *reg_dev;
	
	reg_list->EnterWriter(); //writer a nie reader bo zaraz bycmoze bede cos 
				 //dodawal do tej listy 
	
	reg_dev = privateFindRegistered(name);
	if(reg_dev == NULL) {
		//wlasciwie to nie wiem jak postapic?
		//ale powiedzmy ze ladujemy drivery dynamicznie
		if( (reg_dev = privateRegisterDevice(name) ) == NULL) {
			reg_list->LeaveWriter();
			return 0;
		}
	}
	
	Device *tmpDevice = NULL;
	try {
		tmpDevice = reg_dev->NewDevice(command);
	}
	catch (exc_Codecook &exc) {
		cout << "AddDevice: dostalem exception z NewDevice" << endl;
		cout << exc;
		// w takim razie awaria
		tmpDevice = NULL;
	}
	reg_list->LeaveWriter();

	if(tmpDevice) return 1;

	return 0;
}



int Device_List::DelDevice(char *name) {

	Device *tmpDevice = NULL;

	cstable->threadList->EnterWriter();
	if( !(tmpDevice = privateGetDevice(name, NO_LOCK)) ) {
		cstable->threadList->LeaveWriter();
		return 0;
	}

	Request *req = new Req_Terminate(cstable->threadList->getThreadPid(name, NO_LOCK));
	
	cstable->threadList->LeaveWriter();

	cstable->manager->Send_Request(req);

	return 1;
};

int Device_List::GetParams(char *buffer, int size_of_buffer) {
//co to robi (mam robic)???
	return -1;
};

int Device_List::GetParams(char *device_name, char *buffer, int size_of_buffer) {
//co to robi (mam robic)???
	return -1;
}

char *Device_List::ShowWorkingDevices() {
	return cstable->threadList->getThreadTree(DEVICE);
}

char *Device_List::ShowRegisteredDevices(int level) {
//PP
	string list = "";
	TIterator<RegisteredTypeOfDevice> *i = 
			new TIterator<RegisteredTypeOfDevice>(reg_list);
	RegisteredTypeOfDevice *tmpDevice;
	
	reg_list->EnterReader();
	
	while(!( i->endReached() )){
		tmpDevice = i->next();
		string com, st;
		int mc, cc;
		com += tmpDevice->GetComment();
		cc += tmpDevice->GetCurCount();
		mc += tmpDevice->GetMaxCount();
		st += ctime(tmpDevice->GetStartTime());
		if(level > 3) {
			list += XMLgen::RegisteredDevice(tmpDevice->GetName(),
			    &com, &cc, &mc, &st);
		}
		else if (level > 2) {
			list += XMLgen::RegisteredDevice(tmpDevice->GetName(),
			    &com, &cc, &mc);
		}
		else if (level > 1) {
			list += XMLgen::RegisteredDevice(tmpDevice->GetName(),
			    &com, &cc);
		}
		else if (level > 0) {
			list += XMLgen::RegisteredDevice(tmpDevice->GetName(),
			    &com);
		}
		else 
			list += XMLgen::RegisteredDevice(tmpDevice->GetName());
	}
	
	reg_list->LeaveReader();
	
	delete i;
	return copy_string(list);
}


RegisteredTypeOfDevice * Device_List::RegisterDevice(char *newname, char *directory = NULL) {
	char lib_name[MAX_STR];
	//sprawdz czy juz takiego nie mamy, jesli nie to
	//sprobuj znalezc plugin z takim device'm
	
	reg_list->EnterWriter();
	
	RegisteredTypeOfDevice *found_dev = privateFindRegistered(newname);
	if( found_dev != NULL ) {
		reg_list->LeaveWriter();
		return found_dev;
	} else {
		if( directory == NULL) {
			if( snprintf(lib_name, MAX_STR, "libCodecook_%s.so", newname) == -1) {
				reg_list->LeaveWriter();
				cstable->logger->Log("Internal error:\ttoo long library name: libCodecook_%s.so while registering device %s\n", newname, newname);
				return NULL;
			}
		} else {
			if( snprintf(lib_name, MAX_STR, "%s/libCodecook_%s.so", directory, newname) == -1) {
				reg_list->LeaveWriter();
				cstable->logger->Log("Internal error:\ttoo long library name: %s/libCodecook_%s.so while registering device %s\n", directory, newname, newname);
				return NULL;
			}
		}


			// Trying to open		
		void *handle = dlopen(lib_name, RTLD_LAZY);
		if(!handle) {
			char *lDN = get_current_dir_name();	
			char lFP[strlen(lDN) + strlen(lib_name) + 10];
	
			strcpy(lFP, lDN);
			strcat(lFP, "/");
			strcat(lFP, lib_name);

			handle = dlopen(lFP, RTLD_LAZY);
			
			free(lDN);
		}


		if(!handle) {
			reg_list->LeaveWriter();
			cstable->logger->Log("Error while loading a library (plugin) %s:\n %s\nHave you set the proper LD_LIBRARY_PATH evironment variable?\n", lib_name, dlerror());
			return NULL;
		}

		const char *error;
		printf("PUPA: %s\n", newname);
		RegisteredTypeOfDevice *(*__register_init)(char *, void *) =
				(RegisteredTypeOfDevice *(*)(char *, void *))
				dlsym(handle, "register_init");
		error = dlerror();
		if( error != NULL) {
			reg_list->LeaveWriter();
			cstable->logger->Log("Error while registering new device: %s\n", error);
			dlclose(handle);
			return NULL;
		}
		
		found_dev = (*__register_init)(newname, handle);
		reg_list->add(found_dev);
		
		reg_list->LeaveWriter();
		
		return found_dev;
	}
}

	
int Device_List::UnregisterDevice(char *newname) {
	//sprawdz czy sa dzialajace device tego typu jesli nie to usun plugin
	//wpp. nie rob nic
	RegisteredTypeOfDevice *tmpDevice;
	void *handle;

	reg_list->EnterWriter();
		if( !(tmpDevice = privateFindRegistered(newname)) ) {
			cstable->logger->Log("Cannot unregister device (class not found)");
		} else {
			if (tmpDevice->GetCurCount() == 0) {
				handle = tmpDevice->GetLibHandle();
				reg_list->rmElt(tmpDevice); 
				dlclose(handle); // dopiero tutaj, bo w
						 // konstruktorze nadrzednym nie mozna, 
						 // bo beda wywolane pochodne konstruktory
				reg_list->LeaveWriter();
		
				//tutaj cos loguj
				return 1;
			}
			cstable->logger->Log("Cannot unregister device (instances are working)");
		}
	reg_list->LeaveWriter();
	return 0;
};

RegisteredTypeOfDevice * Device_List::FindRegistered(char *newname) {
	TIterator<RegisteredTypeOfDevice> *iter;
	RegisteredTypeOfDevice *reg_dev;

	iter = new TIterator<RegisteredTypeOfDevice>(reg_list);

	reg_list->EnterReader();
	
	while(!iter->endReached()) {
		reg_dev = iter->next();
		if(strcmp(newname, reg_dev->GetName()) == 0) {
			reg_list->LeaveReader();
			delete iter;
			return reg_dev;
		}
	}

	reg_list->LeaveReader();
	
	delete iter;
	return NULL;
}




int Device_List::ReRegisterDevice(char *newname) {
	if( UnregisterDevice(newname) == 1 )
		return (RegisterDevice(newname) != NULL);
	else
		return 0;
};

int Device_List::RegisterAllDevices(char *directory) {
	//tutaj directory jest katalogiem w ktorym nalezy szukac wszystkich
	//pluginow
	//UWAGA zakladam ze pluginy maja takie nazwy:
	//libCodecook_NAZWA.so gdzie NAZWA nalezy zamienic na nazwe driveru
	//patrz selPlugin na pocztku tego pliku
	
	char *name;
	struct dirent **namelist;
	int pluginCount, n;
	pluginCount = scandir(directory, &namelist, selPlugin, 0);
	if( pluginCount < 0 ) {
		//loguj cos!!!
		return 0;
	} else {
		while( pluginCount-- ) {
			if((name = strdup(namelist[pluginCount]->d_name)) == NULL)
				cstable->logger->Log("za malo pamieci");
			n = strlen(name);
			name[n - 3] = '\0';
			RegisterDevice(name + 12 , directory);
			free(name);
		}
		//loguj cos
		return 1;
	}
};

int Device_List::UnregisterAllDevices() {
	//jesli mozna wyrzucic wszystkie device to to zrob
	//wpp. nic nie rob
	
	//UWAGA: tu moze dojsc do zakleszczenia jesli
	//gdzies jeszcze mamy dostep do obu list jednoczesnie w jakiejs 
	//metodzie - teraz jest ok! ale jak bedziesz dodawal metody
	//zwroc na to uwage. 
	
//	dev_list->EnterReader();
	reg_list->EnterWriter();
	
// FIX
//	if( dev_list->getLength() != 0 ) {
	if (0) {
		//logguj cos
		reg_list->LeaveWriter();
//		dev_list->LeaveReader();
		return 0;
	} else {
		TIterator<RegisteredTypeOfDevice> *iter;
		iter = new TIterator<RegisteredTypeOfDevice>(reg_list);
		while( reg_list->rmHead());
		
		reg_list->LeaveWriter();
//		dev_list->LeaveReader();
		
		//loguj cos
		return 1;
	}
};
int Device_List::ReRegisterAllDevices(char *directory) {
	//nie wiem czy to bedzie kiedykolwiek uzywane???
	//powinno byc tak:
	//nie ma juz/jeszcze zadnego dzialajacego drivera
	//wtedy usuwamy wszystkie zarejestrowane pluginy i  
	//rejestrujemy wszystkie z katalogu directory


//czy tu nie powinienem uzyc private bez sem'ow

	
	if( UnregisterAllDevices() == 1 )
		return RegisterAllDevices(directory);
	else
		return 0;
};

char *Device_List::DeviceExecCommand(char *d_name, char *d_comm) {
//cos tu loguj
	Device *tmpDevice;
	char *ret = NULL;
	cstable->threadList->EnterWriter();
	tmpDevice = privateGetDevice(d_name);
	if(tmpDevice != NULL)
		ret = tmpDevice->ExecCommand(d_comm);
	cstable->threadList->LeaveWriter();

	return ret;
};


//
//	prywatne metody
//	(roznia sie tym od swoich publicznych odpowiednikow, ze nie uzywaja
//	semaforow do blokowania dostepu)

RegisteredTypeOfDevice * Device_List::privateFindRegistered(char *newname) {
	TIterator<RegisteredTypeOfDevice> *iter;
	RegisteredTypeOfDevice *reg_dev;

	iter = new TIterator<RegisteredTypeOfDevice>(reg_list);

	while(!iter->endReached()) {
		reg_dev = iter->next();
		if(strcmp(newname, reg_dev->GetName()) == 0) {
			delete iter;
			return reg_dev;
		}
	}

	delete iter;
	return NULL;
}



RegisteredTypeOfDevice * Device_List::privateRegisterDevice(char *newname, char *directory = NULL) {
	char lib_name[MAX_STR];
	//sprawdz czy juz takiego nie mamy, jesli nie to
	//sprobuj znalezc plugin z takim device'm

	RegisteredTypeOfDevice *found_dev = privateFindRegistered(newname);
	if( found_dev != NULL )
		return found_dev;
	else {
		if( directory == NULL) {
			if( snprintf(lib_name, MAX_STR, "libCodecook_%s.so", newname) == -1) {
				cstable->logger->Log("Internal error:\ttoo long library name: libCodecook_%s.so while registering device %s\n", newname, newname);
				return NULL;
			}
		} else {
			if( snprintf(lib_name, MAX_STR, "%s/libCodecook_%s.so", directory, newname) == -1) {
				cstable->logger->Log("Internal error:\ttoo long library name: %s/libCodecook_%s.so while registering device %s\n", directory, newname, newname);
				return NULL;
			}
		}
		
		void *handle = dlopen(lib_name, RTLD_LAZY);
		if(!handle) {
			char *lDN = get_current_dir_name();	
			char lFP[strlen(lDN) + strlen(lib_name) + 10];
	
			strcpy(lFP, lDN);
			strcat(lFP, "/");
			strcat(lFP, lib_name);

			handle = dlopen(lFP, RTLD_LAZY);
			
			free(lDN);
		}
		if(!handle) {
		
			cstable->logger->Log("Error while loading a library"
			" (plugin) %s:\n %s\n"
			"Have you set the proper LD_LIBRARY_PATH evironment"
			" variable?\n", lib_name, dlerror());
			return NULL;
		}

		const char *error;
		RegisteredTypeOfDevice *(*__register_init)(char *, void *) =
				(RegisteredTypeOfDevice *(*)(char *, void *))
				dlsym(handle, "register_init");
		error = dlerror();
		if( error != NULL) {
			reg_list->LeaveWriter();
			cstable->logger->Log("Error while registering new device: %s\n", error);
			dlclose(handle);
			return NULL;
		}
		
		found_dev = (*__register_init)(newname, handle);

		reg_list->add(found_dev);

		return found_dev;
	}
}

Device *Device_List::privateGetDevice(char *name, LockOrNot lockFlag = LOCK) {

	Device *dev = NULL;

	if (lockFlag == LOCK) cstable->threadList->EnterWriter();
	int pid = cstable->threadList->getThreadPid(name, NO_LOCK);
	if (pid >= 0) {
		Thread *th = cstable->threadList->getThread(pid, NO_LOCK);
		if (th->GetType() == DEVICE) dev = (Device*) th;
	}
	if (lockFlag == LOCK) cstable->threadList->LeaveWriter();
	return dev;
}


// 
// Komendy protokolu...
//

//
//	DeviceListWorkingCommand
//

char *DeviceListWorkingCommand::execute(Command *command) {
//PP
	string s = "";
	
	s += XMLgen::Command(command);
	s += XMLgen::Device_list_working_res(cstable->devices->ShowWorkingDevices());
	return copy_string(s);
};

//
//	DeviceAddCommand
//

char *DeviceAddCommand::execute(Command *command) {
	Token *tok = (*command)[2];
//PP
	int ret = cstable->devices->AddDevice(tok->string(), command);
	string s = XMLgen::Command(command);
	
	if(ret == 0)
		s += XMLgen::Error("Couldn't add device");
	else
		s += XMLgen::TaggedResult("DEVICE_ADD_RES", "OK", "Device added");
	return copy_string(s);
};

//
//	DeviceDeleteCommand
//

char *DeviceDeleteCommand::execute(Command *command) {
	Token *tok1 = (*command)[2];

	int ret = cstable->devices->DelDevice(tok1->string());

	string s = XMLgen::Command(command);
	
	if(ret == 0)
		s += XMLgen::Error("Cannot delete device");
	else
		s += XMLgen::TaggedResult("DEVICE_DELETE_RES", "OK", "Kill request sent");
	return copy_string(s);
};


//
//	DeviceRegisterCommand
//

char *DeviceRegisterCommand::execute(Command *command) {
	Token *tok = (*command)[2];
	RegisteredTypeOfDevice *ret;
	ret = cstable->devices->RegisterDevice(tok->string());
	
	string s = XMLgen::Command(command);
	
	if(ret == NULL)
		s += XMLgen::Error("Cannot register device");
	else
		s += XMLgen::TaggedResult("DEVICE_REGISTER_RES", "OK", "Device registered");
	return copy_string(s);
};

//
//	DeviceReRegisterCommand
//

char *DeviceReRegisterCommand::execute(Command *command) {
	Token *tok = (*command)[2];

	int ret = cstable->devices->ReRegisterDevice(tok->string());
	
	string s = XMLgen::Command(command);
	
	if(ret == 0)
		s += XMLgen::Error("Cannot re-register device");
	else
		s += XMLgen::TaggedResult("DEVICE_REREGISTER_RES", "OK", "Device re-registered");
	return copy_string(s);
};

//
//	DeviceRegisterAllCommand
//

char *DeviceRegisterAllCommand::execute(Command *command) {
	Token *tok = (*command)[3];
	int ret = cstable->devices->RegisterAllDevices(tok->string());
	
	string s = XMLgen::Command(command);
	
	if(ret == 0)
		s += XMLgen::Error("Cannot perform REGISTER ALL command");
	else
		s += XMLgen::TaggedResult("DEVICE_REGISTER_ALL_RES", "OK", "Registered all available devices");
	return copy_string(s);

};

//
//	DeviceReRegisterAllCommand
//

char *DeviceReRegisterAllCommand::execute(Command *command) {
	Token *tok = (*command)[2];

	int ret = cstable->devices->ReRegisterAllDevices(tok->string());
	
	string s = XMLgen::Command(command);
	
	if(ret == 0)
		s += XMLgen::Error("Cannot re-register all available devices");
	else
		s += XMLgen::TaggedResult("DEVICE_REREGISTER_ALL_RES", "OK", "All available devices re-registered");
	return copy_string(s);

};


//
//	DeviceListRegisteredCommand
//

char *DeviceListRegisteredCommand::execute(Command *command) {
	Token *tok = (*command)[3];
	int val;
	string s = XMLgen::Command(command);
	if(tok != NULL)
		val = atoi(tok->string());
	else
		val = 0;
	
	s += XMLgen::Device_list_registered_res(cstable->devices->ShowRegisteredDevices(val));
	return copy_string(s);
};

//
//	DeviceUnregisterCommand
//

char *DeviceUnregisterCommand::execute(Command *command) {
	Token *tok = (*command)[2];
	int ret = cstable->devices->UnregisterDevice(tok->string());
	
	string s = XMLgen::Command(command);
	
	if(ret == 0)
		s += XMLgen::Error("Cannot unregister device");
	else
		s += XMLgen::TaggedResult("DEVICE_UNREGISTER_RES", "OK", "Device unregistered");
	return copy_string(s);
};


//
//	DeviceUnregisterAllCommand
//

char *DeviceUnregisterAllCommand::execute(Command *command) {
	int ret = cstable->devices->UnregisterAllDevices();
	
	string s = XMLgen::Command(command);
	
	if(ret == 0)
		s += XMLgen::Error("Cannot unregister all devices");
	else
		s += XMLgen::TaggedResult("DEVICE_UNREGISTER_ALL_RES", "OK", "All devices unregistered");
	return copy_string(s);
};

