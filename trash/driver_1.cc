#include <unistd.h>

#include "client.h"
#include "driver_1.h"
#include "cstable.h"
#include "logger.h"
#include "protocol.h"

#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


Device * init_device(int number) {
	return NULL;
	//return new Driver_1("NOWY_DRIVER", number);
}

int max_count_of_device() {
	return 0; //nieskonczonosc
}

char *descryption() {
	return NULL;
}

void register_init(RegisteredTypeOfDevice *koko) {
//	cstable->protocol->AddCommand("DEVICE NOWY_DRIVER <??????> PRINT PARAM",
//	new NewDriverCommand, "ale nudy");

	cstable->protocol->AddCommand("DEVICE ADD NOWY_DRIVER <??????>",
	new NewDriverAddCommand, "ale nudy");

}


//
//	NewDriverCommand
//

NewDriverAddCommand::NewDriverAddCommand() {
}

NewDriverAddCommand::~NewDriverAddCommand() {
}

char *NewDriverAddCommand::execute(Command *command) {
	Token *tok = (*command)[3];
	//trzeci argument jest nazwa urzadzenia np.: /dev/ttyS1
	
	//badzmy nadgorliwi cotam...
	int descr;
	if(-1 == (descr = open(tok->string(), O_RDWR)))
		return strdup("<RESULT>\n\tFAIL\n</RESULT>\n");
	close(descr);
	
	Driver_1 *drv = new Driver_1((*command)[1]->string(), 0, tok->string());
	
	int ret = cstable->devices->AddDevice(drv);

	//tutaj dlatego tak (strdup()) bo potem robie free() na tym co zwraca 
	//ta metoda a napis typu "jslkjs" jest static!!!
	if(ret == 0)
		return strdup("<RESULT>\n\tFAIL\n</RESULT>\n");
	else
		return strdup("<RESULT>\n\tOK\n</RESULT>\n");

}



//
//
//

Driver_1::Driver_1(char *name, int number, char *dn) : Device(name, number) {
	dev_name = strdup(dn);
	//sprobuj otworzyc device
	int descr;
	if((descr = open(dev_name, O_RDWR|O_NOCTTY|O_SYNC)) == -1) {
		//no to nie powinno sie zdarzyc ale... co tu zrobic???
	}
	struct termios *term_conf;
	if(tcgetattr(descr, term_conf) != 0) {
		//tu loguj jakis blad, moze sprobuj jeszcze raz??
	};
	
};

Driver_1::~Driver_1() {

};

void *Driver_1::Handler() {
	while(1) {
		
		cstable->logger->Log("dzialam: %s", name);
		sleep(2);
	}
	return NULL;
}
