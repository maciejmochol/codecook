#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// 'x' - nazwa, 'y' - warunek, 'z' - liczba //
char *tab[] = { 
"ALARM HELP",
"ALARM HELP {x}",
"ALARM SHOW NAMES",
"ALARM SHOW DESCR",
"ALARM SHOW ON",
"ALARM SHOW OFF",
"ALARM SHOW FILE",	
"ALARM SHOW FILENAME",
"ALARM SHOW ALL",
"ALARM SHOW DEBUG",
"ALARM SAVE",
"ALARM SAVE {x}",
"ALARM ADD {x}",
"ALARM ADD {x} {y}",
"ALARM ADD {z} {y}",
"ALARM DEL {z}", 
"ALARM DEL {x}",
"ALARM DEL {z}:{z}",
"ALARM DEL {x}:{z}",
"ALARM LOG",
"ALARM LOG SAVE",
"ALARM LOG SAVE {x}",
"ALARM LOG SETFILE {x}",
"ALARM LOG SHOW {x}",
"ALARM LOG SHOWALL",
"ALARM LOG SHOWFILE"
};

int choose(int min, int max); // wybiera losowo wartosc <min, max>
char *generate(char seed); // generuje jakis shit na podstawie seed
char *genstring(int len); // generuje jakis string dlugosci len
char *genstrfrom(int len, int min, int max);    // generuje string, ale z 
						// przedzialu ASCII <min, max>

main(int argc, char **argv)
{
FILE *tel_file;
char buf[50], buf2[50];
int a=0, i;

	if( argc != 3 ){
		printf(">Usage: %s <hostname> <port>\n", *argv);
		exit(1);
	}	
		
	sprintf(buf, "telnet %s %s", argv[1], argv[2]);
	fflush(stdout);
	tel_file = popen(buf, "w");

	if(!tel_file){
		printf(">Error!\n");
		exit(2);
	}

	setbuf(tel_file, NULL);

	fprintf(tel_file, "USER LOGIN placek placek\n"); 
	fflush(tel_file);
	sleep(1);

	for(i=0; i<100; i++){
		char command[80], *c, *cold, *commandp;
		int no = choose(0, 24);	
	
		cold = c = tab[no];	
		commandp = command;

		strcpy(command, "");
		while(1) 
		{
		  if( c = strchr(cold, '{') ){
			strncat(commandp, cold, c - cold);
			commandp+=c-cold;
			*commandp = 0;

			if( choose(1, 5) == 1 )
				; // robimy swiadomy blad - z szansa 1/5
			else{
				strcat(commandp, generate(*(c+1)));
				commandp+=strlen(commandp); 
			}	
			cold = strchr(cold, '}'); cold++;			
		  }
		  else{
			strcat(commandp, cold);	
			break;
		  }	
			
		}	
		strcat(command, "\n");
		printf(">Wygenerowane: %s", command); fflush(stdout);

		fprintf(tel_file, command); 
		fflush(tel_file);
		sleep(1);
	}

	pclose(tel_file);
return 0;
}

int choose(int min, int max)
{
static int seed=0;
if(seed == 0)
	seed = getpid();
if( min < 0 || max < 0 || min > max )
	return -1;

srand(seed);
seed = rand();
return min+(int) ( seed/(RAND_MAX+1.0)*(max-min));
}

char *generate(char seed)
{
static char string[20];
// 'x' - nazwa, 'y' - warunek, 'z' - liczba //

	if( seed == 'x' )
		strcpy(string, genstring( choose(1, 15) ));
	else if( seed == 'y' ){
		char *str;
		strcpy(string, "{");
		strcat(string, genstring( choose(1, 15) ) );
		strcat(string, " ");

		strcat(string, str = genstrfrom( 1, 60, 63 ) );
		if(*str == '=')
			strcat(string, "=");

		strcat(string, " ");
		strcat(string, genstring( choose(1, 15) ) );
		strcat(string, "}");
	}
	else if( seed == 'z' )
		strcpy(string, genstrfrom(choose(1,2), 48, 58));

return string;
}

char *genstring(int len)
{
static char string[20];
int i;
	
	for(i=0; i<len; i++){
		if( i == 19 ) break;
		string[i] = choose(66, 110);
		string[i+1] = 0;
	}

return string;
}

char *genstrfrom(int len, int min, int max)
{
static char string[20];
int i;
	
	for(i=0; i<len; i++){
		if( i == 19 ) break;
		string[i] = choose(min, max);
		string[i+1] = 0;
	}

return string;
}
