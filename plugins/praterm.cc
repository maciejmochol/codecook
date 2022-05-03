#include <unistd.h>

#include "../src/client.h"
#include "praterm.h"
#include "../src/cstable.h"
#include "../src/logger.h"
#include "../src/protocol.h"

#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>


#define MAX_BUF 255
//#define Q_STRING "\x11\x02Px\x03\n"

#define T1_STRING "\x11\x02T"
#define T2_STRING "\x03"

#define MAX_WAIT 5



RegisteredTypeOfDevice *register_init(char *n, void *h) {
	return new Praterm_Register(n, h);
	

}


//
//	NewDriverAddCommand
//

NewDriverAddCommand::NewDriverAddCommand() {
}

NewDriverAddCommand::~NewDriverAddCommand() {
}

char *NewDriverAddCommand::execute(Command *command) {

printf("jestem w execute\n");


//	Token *tok1 = (*command)[3];
	//trzeci argument jest nazwa urzadzenia np.: /dev/ttyS1
//	Token *tok2 = (*command)[4];
	//czwarty argument jest numerem urzadzenia np.: 1 jak w ^BP1^C
	
//	Praterm *drv = new Praterm((*command)[2]->string(), 0, tok1->string(), tok2->string);
	Praterm *drv;
// FIX ME
	int ret;
//	int ret = drv->workingOk() && cstable->devices->AddDevice(drv);
		
	//tutaj dlatego tak (strdup()) bo potem robie free() na tym co zwraca 
	//ta metoda a napis typu "jslkjs" jest static!!!
	if(ret == 0) {
		delete drv;
		return strdup("<RESULT>\n\tFAIL\n</RESULT>\n");
	} else
		return strdup("<RESULT>\n\tOK\n</RESULT>\n");

}

//
//	NewDriverCommand
//

NewDriverCommand::NewDriverCommand() {
}

NewDriverCommand::~NewDriverCommand() {
}

char *NewDriverCommand::execute(Command *command) {
printf("jestem w EXECUTE>>>>\n");
	char *d_name = (*command)[1]->string();
printf("AA\n");
	int d_num = atoi((*command)[2]->string());
printf("BB\n");
	if(d_num == 0) {
printf("CC\n");	
		if(((*command)[2]->string())[0] != '0')
			return strdup("<RESULT>\n\tFAIL\n</RESULT>\n");
printf("DD\n");	
	}
	char *comm = command->toStringBeginWith(3);
printf("doszedlem tu\n");
	return cstable->devices->DeviceExecCommand(d_name, comm);
}


//
//
//


Praterm_Register::Praterm_Register(char *n, void *h) :
		RegisteredTypeOfDevice(n,h) {
	SetMaxCount(0);
}

void *Praterm_Register::Handler() {

//	cstable->protocol->AddCommand("DEVICE NEW_DRIVER <??????> PRINT PARAM",
//	new NewDriverCommand, "ale nudy");

	cstable->protocol->AddCommand("DEVICE ADD NEW_DRIVER <??????> <??????>",
	new NewDriverAddCommand, "DEVICE ADD NEW_DRIVER <dev> <number>\n\t"
	"where <dev> is device to which driver should be connected (like /dev/ttyS0)"
	"\n<number> is a number stored in the device's memory, device is identified "
	"by this number\n");

	cstable->protocol->AddCommand("DEVICE NEW_DRIVER <??????> SET TIME",
	new NewDriverCommand, "ale nudy");

	cstable->protocol->AddCommand("DEVICE NEW_DRIVER <??????> SET MAX TIMEOUT <??????>",
	new NewDriverCommand, "ale nudy");

	cstable->protocol->AddCommand("DEVICE NEW_DRIVER <??????> SHOW STATUS",
	new NewDriverCommand, "ale nudy");

	return NULL;
}

Praterm_Register::~Praterm_Register() {
}

Device *Praterm_Register::CreateDevice(char * name, Command *command) {
	return new Praterm(name, this);
}



//
//
//

Praterm::Praterm(char *name, RegisteredTypeOfDevice *dev_type) : Device(name, dev_type) 
{
	status = 1;
	q_string = t_string = pnp_string = NULL;
	param_list = param_handles = NULL;
	maxWait = MAX_WAIT;

	char num[4];

	dev_num = 2;
	sprintf(num, "%d", dev_num);	
	sprintf(dev_name, "/dev/ttyS%d", 1-1);

	q_len = strlen("\x11\x02P") + strlen(num) + strlen("\x03") + 1;
	q_string = (char *) malloc(q_len);
	snprintf(q_string, q_len, "\x11\x02P%d\x03", dev_num);
	
	pnp_len = strlen("\x11\x02&") + strlen(num) + strlen("\x03") + 1;
	pnp_string = (char *) malloc(pnp_len);
	snprintf(pnp_string, pnp_len, "\x11\x02&%d\x03", dev_num);
	
	t_len = strlen(T1_STRING) + strlen(num) + 17 + strlen(T2_STRING);
	t_string = (char *) malloc(t_len);
	
 	pnp_mode = 0;	
};

Praterm::~Praterm() {
	if(descr != -1)
		tcsetattr(descr, TCSANOW, &old_term_conf);
	close(descr);	//olej bledy
	if(q_string != NULL)
		free(q_string);
	if(pnp_string != NULL)
		free(pnp_string);
	if(t_string != NULL)
		free(t_string);
	if(param_list != NULL)
		free(param_list);
	if(param_handles != NULL)
		free(param_handles);
};

int Praterm::OpenLine()
{
	errno = 0;

	cstable->logger->Log("Driver:[%s,%d]: Trying to open device %s",name,
		dev_type->GetCurCount(), dev_name);
	descr = -1;
	int probes = 1;
	while(probes > 0 && descr == -1) {
		descr = open(dev_name, O_RDWR | O_NOCTTY);
		probes--;
	};
	if(descr == -1) {
		cstable->logger->Log("Driver:[%s,%d]: Opening device: %s FAILED!; ERROR:%s",name, 
			dev_type->GetCurCount(), dev_name, sys_errlist[errno]);
		status = 0;
		return 0;
	} else 
		cstable->logger->Log("Driver:[%s,%d]: Openning device: %s SUCCEDED!",name, 
			dev_type->GetCurCount(), dev_name);
	
	//ustaw odpowiednio port komunikacyjny
	
	struct termios term_conf;
	if(tcgetattr(descr, &old_term_conf) != 0) {
		cstable->logger->Log("Driver:[%s,%d]: Configuring device: %s FAILED!; ERROR:%s",name, 
				dev_type->GetCurCount(), dev_name, sys_errlist[errno]);	
		status = 0;
		return 0;
	} else {
		bzero(&term_conf, sizeof(term_conf));
		term_conf.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
		term_conf.c_iflag = IGNPAR | ICRNL;
		term_conf.c_oflag = 0;
		term_conf.c_lflag = 0;
		term_conf.c_cc[VTIME] = maxWait * 10;
		term_conf.c_cc[VMIN] = 0;
		if(tcsetattr(descr, TCSANOW, &term_conf) != 0) {
			cstable->logger->Log("Driver:[%s,%d]: Configuring device: %s FAILED!; ERROR:%s",name, 
				dev_type->GetCurCount(), dev_name, sys_errlist[errno]);	
			status = 0;
			return 0;
		} else
			cstable->logger->Log("Driver:[%s,%d]: Configuring device: %s SUCCEDED!",name, 
				dev_type->GetCurCount(), dev_name);	
	}

	return 1;
}

int Praterm::getNumberOfParams() {
	DynamicBuffer *buf;
	int ret;
	ret = askForReport();
	if(ret) {
		buf = readReport();
		if(buf != NULL) {
			ret = buf->numberOfLines() - 5 - 1; //rapid, subid, funid, suma kontrolna, data.
			cstable->logger->Log("Number of parameters: %d\n",ret);
			delete buf;
		}
	}
	return ret;
};

void DoClearDivChar(char **token, char div_char)
{
 if (!token || !*token) return;
 char *c = *token;

 while (*c == ' ' || *c == '\n' || *c == div_char) c++; /* czysc poczatkowe space */
 *token = c;
}

void GetNextToken(char **token, char *buf, int size, char div_char)
{
 char *c, *cend;
 int i;
 if (!token || !(*token) || !buf || !size) return;
 c = *token;

 DoClearDivChar(&c, div_char); /* czysc poczatkowe space */
 *token = c;
  
 if (*c == 0) { *token = NULL; return; };
 bzero(buf, size);
 while (*c != div_char && *c != '\n' && *c != 0) c++;
 i = (c - *token);
 size--;
 if (i > size) i = size;
 strncpy(buf, *token, i);
 cend = buf + strlen(buf);
 while (cend > buf && (*cend == 0 || *cend == ' ' || *cend == '\n')) cend--; /* czysc koncowe space */
 if (cend+1 < buf+size) *(cend+1) = 0;
 *token = c;
}

int Praterm::ParsePnPReport(DynamicBuffer *buf)
{
	printf("ParsePnP !\n");
	if(buf == NULL)
		return 0;
	//sprawdz czy liczba linii ktore przyszly zgadza sie z param_len
	if(buf->numberOfLines() != param_len + 3) {
		return 0;
	}

	printf("Raport PNP\n");
	
	//pierwsza linijka to "PNP INFO" - olej
	free(buf->nextLine());

	char *str;
	char name[20];
	char description[100];
	char min[10];
	char max[10];
	char komma[3];
	char unit[20];

	for (int i = 0; i < param_len; i++) {
		str = buf->nextLine();
		char *c;
		c = str;
		GetNextToken(&c, name, sizeof(name), '*');
		GetNextToken(&c, description, sizeof(description), '*');
		GetNextToken(&c, min, sizeof(min), '*');
		GetNextToken(&c, max, sizeof(max), '*');
		GetNextToken(&c, komma, sizeof(komma), '*');
		GetNextToken(&c, unit, sizeof(unit), '*');

		printf("PARAM%d: %s %s %s %s %s %s\n", i, 
			name, description, min, max, komma, unit);
		param_handles[i] = MakeIntParam(name);
		SetParamChannel(name, "NAME", description);
		SetParamChannel(name, "MIN", min);
		SetParamChannel(name, "MAX", max);
		SetParamChannel(name, "UNIT", unit);
		free(str);
	};

	// a teraz kolej na opis programu i date
	str = buf->nextLine();
	free(str);

	// ostatnia modyfikacje olej

	pnp_mode = 1;

	return 1;
}

int Praterm::getPnPReport() {
	DynamicBuffer *buf;
	int ret;
	cstable->logger->Log("Getting PnP report...");
	ret = askForPnPReport();
	if(ret) {
		buf = readReport();
		if(buf != NULL) {
			ParsePnPReport(buf);
			// FIXME - dodac sume kontrolna
			// i powtarzanie gdy zla
			delete buf;
		}
	}
	return ret;
};


int Praterm::setIOtimeout(int min, int time) {
	struct termios term_conf;
	if(tcgetattr(descr, &term_conf) != 0) {
		cstable->logger->Log("Driver:[%s,%d]: Configuring device: %s FAILED!; ERROR:%s",name, 
				1, dev_name, sys_errlist[errno]);	
		return 0;
	}
	term_conf.c_cc[VTIME] = time;
	term_conf.c_cc[VMIN] = min;
	if(tcsetattr(descr, TCSANOW, &term_conf) != 0) {
		cstable->logger->Log("Driver:[%s,%d]: Configuring device: %s FAILED!; ERROR:%s",name, 
				1, dev_name, sys_errlist[errno]);	
		return 0;
	}
	cstable->logger->Log("Driver:[%s,%d]: Configuring device: %s SUCCEDED!",name, 
			1, dev_name);	
	return 1;
};

int Praterm::askForReport() {
	int ret = write(descr, q_string, q_len);
	if(ret < 0)
		cstable->logger->Log("Driver:[%s,%d]:"
				" Error while writing to: %s, %s",name, 
				1, dev_name, sys_errlist[errno]);
	return ret == q_len;
};

int Praterm::askForPnPReport() {
	int ret = write(descr, pnp_string, pnp_len);
	if(ret < 0)
		cstable->logger->Log("Driver:[%s,%d]:"
				" Error while writing to: %s, %s",name, 
				1, dev_name, sys_errlist[errno]);
	return ret == pnp_len;
};

DynamicBuffer *Praterm::readReport() {
	int num, pos;
	char buffer[MAX_BUF];
	fd_set readfs;
	struct timeval t_max;
	DynamicBuffer *dynBuf = new DynamicBuffer();
	bzero(&readfs, sizeof(readfs));
	pos = 0;
	do {
		FD_SET(descr, &readfs);
		t_max.tv_usec = 0;
		t_max.tv_sec = maxWait;
		num = 0;
		switch(select(descr +1, &readfs, NULL, NULL, &t_max)) {
			case 0:
				if(pos == 0) {
					cstable->logger->Log("Driver:[%s,%d]: Error while reading from: %s - timeout waiting for first character", name, 
								1, dev_name);
					delete dynBuf;
					return NULL;
				}
				break;
			case -1:
				cstable->logger->Log("Driver:[%s,%d]: Error while reading from: %s ",name, 
					1, dev_name);
				delete dynBuf;
				return NULL;
			default:
				bzero(buffer, MAX_BUF * sizeof(char));
				if((num = read(descr,buffer,MAX_BUF-1)) == -1) {
					cstable->logger->Log("Driver:[%s,%d]: Error while reading from: %s ",name, 
							1, dev_name);
					delete dynBuf;
					return NULL;
				} else {
					pos = num;
					buffer[num] = 0;
					(*dynBuf) << buffer;
				}
		}
	} while(num > 0);
	return dynBuf;	
};
int Praterm::testControlSum(char * str) {
//chwilowo tak:
	return 1;
	//ostatnia linijka to suma kontrolna:
	int i;
	int len = strlen(str);
	for( i = len; i > 1 && str[i - 1] != '\n'; i--);
	printf("str:%s:\n", str + len - i);
	unsigned int sumCR = (unsigned int) atoi(str + len - i);
	printf("sumCR:%d:\n", sumCR);
	if(sumCR == 0)  //raczej niemozliwe - a atoi moglo zwrocic 0
		return 0;
	unsigned int sum = 0;
	printf("FFF:%d:%d\n", '\r','\n');	
	for( i = 0; i <= len; i++) {
		
		sum += (unsigned int) str[i];
		printf("sum:%d:%d:%c\n", sum, str[i], str[i]);
	//	if(str[i] == '\n')
	//		sum += 13;
	}
	printf("sum:%d:sumCR:%d:\n", sum, sumCR);	
	return (sum == sumCR);
};

int Praterm::parseReport(DynamicBuffer *buf) {
	if(buf == NULL)
		return 0;
	//sprawdz czy liczba linii ktore przyszly zgadza sie z param_len
	if(buf->numberOfLines() != param_len + 5 + 1) {
		return 0;
	}
	//sprawdz sume kontrolna.
	char *str = buf->toString();
	int ret = testControlSum(str);
	free(str);
	if(!ret) {
		printf("Bad control sum !\n");
		return 0;
	}
	
	//pierwsza linijka to RapId - olej
	free(buf->nextLine());
	//druga linijka to SubId - olej
	free(buf->nextLine());
	//trzecia linijka to FunId - olej
	free(buf->nextLine());
	//czwarta linijka to data i czas - olej
	free(buf->nextLine());
	
	//dalej zakladam ze znam liczbe parametrow ktore do mnie przychodza

	BeginAquisition(); // rozpocznij akwizycje

	int l;
	for(int i = 0; i < param_len; i++) {
		str = buf->nextLine();
		l = atoi(str);
		free(str);
		if(param_list[i] != l) {
			param_list[i] = l;

			// Wstaw do ParamTable			
			WriteParam(param_handles[i], &l);
		}
	}

	EndAquisition(); // skoncz akwizycje
	return 1;
};

int Praterm::getReport() {
	DynamicBuffer *buf;
	
	int ret = askForReport();
	if(ret) {
		buf = readReport();
		ret = parseReport(buf);
		if(buf != NULL)
			delete buf;
	}
	return ret;
}

void *Praterm::Handler() {

	if (!OpenLine()) return NULL;

	//ustaw czas
	
	if(!setTime()) {
		status = 0;
		return NULL;
	};
	sleep(1); // poczekaj chwile

	//pobierz liczbe parametrow
	if((param_len = getNumberOfParams()) <= 0) {
		cstable->logger->Log("Driver:[%s,%d]: Wrong number of parameters in report",
			name, dev_type->GetCurCount());	
			status = 0;
			return NULL;
	}

	param_list = (int *) malloc(param_len * sizeof(int));
	param_handles = (int *) malloc(param_len * sizeof(int));

	//pobierz raport PnP jesli sie da
	getPnPReport();

	if (!pnp_mode) {
		// Nie udalo sie wczytac raportu PnP
		// FIXME
		// napisac odczyt kanalow dla sterownika nie PnP
		// w/g dokumentacji

		char name_buf[10];
		for (int i = 0; i < param_len; i++) {
			snprintf(name_buf, sizeof(name_buf), "P%d", i);
			param_handles[i] = MakeIntParam(name_buf);
		}
	}

	while(1) {
		sleep(reportInterval);

		status = getReport();

		if (!status) {
			cstable->logger->Log("Bad report received !");
		}
	}

	return NULL;
}

int Praterm::getMaxWait() {
	return maxWait;
};

int Praterm::setMaxWait(int m) {
	struct termios term_conf;
	if(tcgetattr(descr, &term_conf) != 0) {
		cstable->logger->Log("Driver:[%s,%d]: Configuring device: %s FAILED!; ERROR:%s",name, 
				1, dev_name, sys_errlist[errno]);	
		return 0;
	} else {
		term_conf.c_cc[VTIME] = m * 10;
		if(tcsetattr(descr, TCSANOW, &term_conf) != 0) {
			cstable->logger->Log("Driver:[%s,%d]: Configuring device: %s FAILED!; ERROR:%s",name, 
					1, dev_name, sys_errlist[errno]);	
			return 0;
		} else {
			cstable->logger->Log("Driver:[%s,%d]: Configuring device: %s SUCCEDED!",name, 
					1, dev_name);
			maxWait = m;
		}
	}
	return 1;
};

int Praterm::setTime() {
	time_t t = time(NULL);
	struct tm * tim = localtime(&t);

	char time_buf[100];
	snprintf(time_buf, sizeof(time_buf), "\x11\x02T%d%04d/%02d/%02d %02d:%02d\x03", 
		dev_num,
		1900 + tim->tm_year,tim->tm_mday, tim->tm_mon,
		tim->tm_hour, tim->tm_min);

	int ret = write(descr, time_buf, strlen(time_buf));
	printf("time: %s \n", time_buf);

	if(ret < 0)
		cstable->logger->Log("Driver:[%s,%d]:"
				" Error while writing to: %s, %s",name, 
				1, dev_name, sys_errlist[errno]);

	return (ret >= 0);
};

int Praterm::workingOk() {
	return status;
}

char *Praterm::ExecCommand(char *str) {
printf("jestem w Exec COMMAND\n");
	int ret = 0;
	char *retval = NULL;
	if(strncmp(str,"SET TIME", 8) == 0) {
	printf("jestem w SET TIME\n");
		LockMe();
		ret = setTime();
		UnlockMe();
	}
	if(strncmp(str,"SET MAX TIMEOUT ", 16) == 0) {
printf("jestem w SET MAX WAIT\n");
		int val = atoi(str + 16);
		if( val <= 0 )
			ret = 0;
		else {
			LockMe();
			ret = setMaxWait(val);
			UnlockMe();
		}
	}
	if(strncmp(str,"SHOW STATUS", 11) == 0) {
	printf("jestem w STATUS\n");
		ret = 1;
		LockMe();
		if(workingOk())
			retval = strdup("<WORKING>\n\tGOOD\n</WORKING>\n");
		else
			retval = strdup("<WORKING>\n\tBAD\n</WORKING>\n");
		UnlockMe();
	}
	if(strncmp(str,"SHOW PARAMS", 11) == 0) {
	printf("jestem w show params\n");
		ret = 1;
		LockMe();
		if(workingOk())
			retval = strdup("<WORKING>\n\tGOOD\n</WORKING>\n");
		else
			retval = strdup("<WORKING>\n\tBAD\n</WORKING>\n");
		UnlockMe();
	}

	char *r;
	if(retval != NULL) {
		int a = strlen(retval);
		int b = (ret == 1) ? 24 : 26;
		r = (char *) malloc(a+b);
		strncpy(r, retval, a);
		if(ret)
			strncpy(r + a, "<RESULT>\n\tOK\n</RESULT>\n", b);
		else
			strncpy(r + a, "<RESULT>\n\tFAIL\n</RESULT>\n", b);
		free(retval);
	} else
		if(ret)
			r = strdup("<RESULT>\n\tOK\n</RESULT>\n");
		else
			r = strdup("<RESULT>\n\tFAIL\n</RESULT>\n");
	printf("zwroce:%s:\n",r);
	return r;
}