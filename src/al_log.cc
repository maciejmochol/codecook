/* $Id: al_log.cc,v 1.4 2000/04/25 17:52:36 code Exp $ */
/*
 * logfile.cc - methods from log.h defined classes 
 * Vooyeck '99-07
 */

#define _GNU_SOURCE
#include <time.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "alarmserv.h"
#include "al_log.h"

/////////////////////////////////////////////////////////////////////////////
// Metody clasy CMemoryRecord
/////////////////////////////////////////////////////////////////////////////

CMemoryRecord::CMemoryRecord(char*pnazwa, long p, long o, int a) 
{
	name = strdup(pnazwa); 
	firstRecord = p; 
	ostatni = o; 
	aktywny = a;
}

/////////////////////////////////////////////////////////////////////////////
// Metody clasy CFileAlarm
/////////////////////////////////////////////////////////////////////////////

CFileAlarm::CFileAlarm(char *pname) 
{ 
	lista = new TList<SFileRecord> ;
	name = strdup(pname); 
	//aktywny = 0;
}

CFileAlarm::~CFileAlarm() 
{ 
	lista->destroy();  
	delete lista; 
	delete name; 
}

void CFileAlarm::print() 
{ 
	TIterator<SFileRecord> *i = new TIterator<SFileRecord>(lista);

	//long czas;
	SFileRecord *tmpRek;

		printf("* Statystyka alarmu %s *\n", name);
			while(!(i->endReached())) {
				tmpRek = i->next();		
				if (tmpRek->aktywny == 1) {
				printf("%s ->", time2str(tmpRek->kiedy) );
				} else {
				printf(" %s", time2str(tmpRek->kiedy) );
				if( tmpRek->aktywny == 2 )
					printf(" (Do turnOffenia serwera)\n");
				else
					printf("\n");
				}
			}
		if(tmpRek->aktywny  == 1)
			printf(" nie turnOffony\n");
	printf("\n");
}

/////////////////////////////////////////////////////////////////////////////
// Metody klasy CFileAlarmList
/////////////////////////////////////////////////////////////////////////////

CFileAlarmList::~CFileAlarmList()
{
	lista_alarmow->destroy();
	delete lista_alarmow;
}

void CFileAlarmList::print() 
{
	TIterator<CFileAlarm> *i = 
			new TIterator<CFileAlarm>(lista_alarmow);

		while(!(i->endReached()))
			i->next()->print();		
}

CFileAlarm *CFileAlarmList::getAlarmFromFile(char *nazwa) const
{
	TIterator<CFileAlarm> *i = 
			new TIterator<CFileAlarm>(lista_alarmow);

	CFileAlarm *tmpAl;

		while(!(i->endReached())){
			tmpAl = i->next();		
			if(!strcmp(nazwa, tmpAl->getName()))
				return tmpAl;
			}
	return NULL;
}

CFileAlarm *CFileAlarmList::add(CFileAlarm *pazp) 
{ 
	lista_alarmow->add(pazp);
	return pazp;
} 

CFileAlarm *CFileAlarmList::add(char *pname)
{
CFileAlarm *tmpAlrm = getAlarmFromFile(pname);

	if(tmpAlrm)
		return tmpAlrm;	

return add( new CFileAlarm(pname) );
}

void CFileAlarmList::add(char *pname, SFileRecord *rek)
{
CFileAlarm *tmpAlrm;

	tmpAlrm = add(pname); 
	tmpAlrm->add(rek);

//log_ost = kiedy;// modyfikujemy date ostatniego.. yea 
}

/////////////////////////////////////////////////////////////////////////////
// Metody klasy COnList
/////////////////////////////////////////////////////////////////////////////


COnList::COnList(char*np) 
{ 
		fileName = (char*)malloc( strlen(np) + 10 );
		strcpy( fileName, "./log/" );
		strcat( fileName, np);	
	
		lista = new TList<CMemoryRecord>; 
		wolny = 1;
		log_p = log_ost = time(NULL);
		ready = false;

		plik = fopen( fileName, "r+" );
		if( plik ){
			if( loadList() ); // ustawia 'wolny'
				ready = true;

			if(ready){
				SFileRecord *tmpSRek = readFromRealFile(1);
				log_p = tmpSRek->kiedy;
				delete tmpSRek;

				tmpSRek = readFromRealFile(wolny - 
						RECORD_LENGTH + 1);
				log_ost = tmpSRek->kiedy;
				delete tmpSRek;
			}
		}
		else{
			plik = fopen( fileName, "w+");

			if(plik) 
				ready = true;
		}

}

COnList::~COnList() 
{ 
	if(ready){
		fclose(plik); 
		saveList();
	}

	delete fileName;

	lista->destroy(); 
	delete lista; 
}

void COnList::add(CMemoryRecord *i) 
{ 
	if(!getMemoryRecord(i->getName()) ){
		lista->add(i); 
		}
	else {
		printf("%s juz istnieje\n",
		i->getName());
		delete i;
	}
}

void COnList::logsync(int jak) 
{
	saveList();
	if(jak == CLOSE )
		fclose(plik);
	else{
		fflush(plik);
	}	
}


CMemoryRecord *COnList::getMemoryRecord(int numer) const
{
TIterator<CMemoryRecord> *i = new TIterator<CMemoryRecord>(lista);

CMemoryRecord *tmpAs;
int licznik=1;

	while(!(i->endReached())) {
		tmpAs = i->next();
		if(licznik == numer){
			delete i;
			return tmpAs;
			}
		licznik++;
	}
delete i;
return NULL;
}

CMemoryRecord *COnList::getMemoryRecord(char *pname) const
{
	TIterator<CMemoryRecord> *i = new TIterator<CMemoryRecord>(lista);

	CMemoryRecord *tmpAs;

	while(!(i->endReached())) {
		tmpAs = i->next();
		if(!strcmp(tmpAs->getName(), pname) ){
			delete i;
			return tmpAs;
		}
	}	
delete i;
return NULL;
}

SFileRecord *COnList::readFromRealFile() const
{
SFileRecord *lAs = new SFileRecord;
char c;

	fscanf(plik, "%ld %ld %ld %d %c", &(lAs->kiedy), 
		&(lAs->poprz), &(lAs->next), &(lAs->aktywny) , &c);

return lAs;
}

SFileRecord *COnList::readFromRealFile(long offset) const
{	
SFileRecord *lAs = new SFileRecord;
char c;

	fseek(plik, offset, SEEK_SET);
	fscanf(plik, "%ld %ld %ld %d %c", &(lAs->kiedy), 
		&(lAs->poprz), &(lAs->next), &(lAs->aktywny), &c);

return lAs;
}

/* Podajemy offset, a on zwraca tez offset ale ustawilony na 'kiedy' */
long COnList::fit(long offset)
{
char bufor[15] = "1";	

	fseek(plik, offset, SEEK_SET);
	while(bufor[0] != ',')	
		fscanf(plik, "%s", bufor);

return ftell(plik) - RECORD_LENGTH + 1; // jeden znak za przecinkiem
}

long COnList::findInFile(int kryt, long wartosc)
{
SFileRecord *pAs;

if( kryt == LESS ){

	while(ftell(plik) < wolny - RECORD_LENGTH + 1){
		pAs = readFromRealFile();
		if( pAs->kiedy >= wartosc ){
			delete pAs;
			return ftell(plik) - RECORD_LENGTH;		
		}
		delete pAs;
	}	
}
else if( kryt == MORE ){
	long offset = fit(wolny - RECORD_LENGTH + 1);
	while( ftell(plik) > 1 ){
		pAs = readFromRealFile(offset);
		if( pAs->kiedy <= wartosc ){
			delete pAs;
			return ftell(plik);
			}
		offset-=RECORD_LENGTH;
		delete pAs;
		}
	}	
return 0L;
}

long COnList::findInFileRapidly(int kryt, long wartosc, 
	long gdzie_jestes, long krok)
{
	/* Wyszukiwanie poloowkowe */

if( gdzie_jestes == 0) gdzie_jestes = wolny/2;
if( krok == 0) krok = wolny/2;
	if( krok < RECORD_LENGTH )
		krok = RECORD_LENGTH;

gdzie_jestes = fit(gdzie_jestes);	

if ( gdzie_jestes == 1 ){
	if ( kryt == LESS ) // mniejszego juz nie bedzie
		return gdzie_jestes;
	}
else if(gdzie_jestes >= wolny - RECORD_LENGTH + 1){
	if ( kryt == MORE ) // wiekszego nie ma na swiecie
		return gdzie_jestes;
	}
else if( kryt == MORE && readFromRealFile()->kiedy >= wartosc )
	return gdzie_jestes;
else if( kryt == LESS && readFromRealFile()->kiedy <= wartosc )
	return gdzie_jestes;
else
	return findInFileRapidly(kryt, wartosc, kryt == MORE ?
		gdzie_jestes + krok/2 : gdzie_jestes - krok/2, krok/2);
return 0L;
}

/*
SFileRecord *COnList::znajdz_ostatni(CMemoryRecord *as)
{
fseek(plik, as->getFirst(), SEEK_SET);

SFileRecord lAs;
long next;
while(next = (lAs = readFromRealFile())->next){
	fseek(plik, next, SEEK_SET);
	delete lAs;
	}
// teraz mamy juz ostatni
lAs->next  = next;
return lAs;
}
*/

void COnList::changeFileLog(long gdzie, int co, long wartosc)
{
	fseek(plik, gdzie, SEEK_SET);
	if( co == NEXT ) {
		fseek(plik, 26, SEEK_CUR);
		fprintf(plik, "%12ld ", wartosc);
	}
}

void COnList::addFileLog(long kiedy, long poprz, long next,
	int stan)
{
	fseek(plik, 0L, SEEK_END);

	fprintf(plik, " %12ld %12ld %12ld %1d ,", kiedy, poprz, 
		next, stan );
}


void COnList::logShutdown() 
{
long data = time(NULL);

TIterator<CMemoryRecord> *i = new TIterator<CMemoryRecord>(lista);
CMemoryRecord *tmpAs;

while(!(i->endReached())) {
	tmpAs = i->next();
	if( tmpAs->isOn() ){
		long o_wsk = tmpAs->getLast();
		changeFileLog(o_wsk, NEXT, wolny);
		tmpAs->setLast(wolny);
		addFileLog(data, o_wsk, 0, 2);
		wolny+=RECORD_LENGTH;
	}
}

log_ost = data;
logsync();
}


void COnList::logFromList(TList<CMemoryRecord> *nasza_lista) 
{
if( !isReady() )
	return; 

TIterator<CMemoryRecord> *i = new TIterator<CMemoryRecord>(nasza_lista);

int licznik=1;
CMemoryRecord *tmpAs;
long data = time(NULL);

while(!(i->endReached())) {
	tmpAs = i->next();

	if( tmpAs->getFirst() != 0 ){
		long o_wsk = tmpAs->getLast();
		changeFileLog(o_wsk, NEXT, wolny);
		tmpAs->setLast(wolny);
		addFileLog(data, o_wsk, 0, tmpAs->isOn());
		wolny+=RECORD_LENGTH;
	}
	else{
		addFileLog(data, -licznik, 0, tmpAs->isOn());
		tmpAs->setFirst(wolny);
		tmpAs->setLast(wolny);
		wolny+=RECORD_LENGTH;
	}
	licznik++;
}

log_ost = data;
logsync();
}

bool COnList::loadList()
{
char *list_name = (char*)malloc(strlen(fileName) + 15);

	strcpy( list_name, fileName);
	strcat( list_name, ".lista" );

FILE *list_file = fopen( list_name, "r" );

	if(!list_file){
		//printf("Nie moge otworzyc pliku %s!\n", list_name);
		delete lista;
		lista = NULL;

		return false;
	}	
int ilosc;
fscanf(list_file, "%d %ld %ld %ld\n", &ilosc, &wolny, &log_p, &log_ost );

char tmp_name[MAX_ALARM_NAME];
long tmp_p, tmp_ost;
int tmp_aktywny;

	while(ilosc--){
		fscanf(list_file, "%s %ld %ld %d", tmp_name, &tmp_p, 
			&tmp_ost, &tmp_aktywny);
		add( new CMemoryRecord(strdup(tmp_name), tmp_p, tmp_ost,
			0 ) );
	}	

return true;
}

void COnList::print_liste()
{
	TIterator<CMemoryRecord> *i = new TIterator<CMemoryRecord>(lista);
		
	while(!(i->endReached()))
		printf("%s\n", i->next()->getName() );
}

void COnList::saveList()
{
char *list_name = (char*)malloc(strlen(fileName) + 15);

	strcpy( list_name, fileName );
	strcat( list_name, ".lista" );

FILE *list_file = fopen( list_name, "w" );

	if(!list_file){
		printf("Nie moge otworzyc pliku %s!\n", list_name);
		return;
	}	

	fprintf(list_file, "%d %ld %ld %ld\n", 
		getNoOfElements(), wolny, log_p, log_ost );
	
	TIterator<CMemoryRecord> *i = new TIterator<CMemoryRecord>(lista);
	CMemoryRecord *tmpAs;
		
	while(!(i->endReached())) {
		tmpAs = i->next();
		fprintf(list_file, "%s %ld %ld %d\n", tmpAs->getName(),
			tmpAs->getFirst(), tmpAs->getLast(), 
			tmpAs->isOn() );
		}

	fclose(list_file);
}


CFileAlarm* COnList::readFromFile(char *alarmName, long from, 
	long to)
{
CFileAlarm *azp = new CFileAlarm(alarmName);
CMemoryRecord *tmpAs = getMemoryRecord(alarmName);

	if( (!tmpAs) || (from>to) ) { 
		delete azp;
		return NULL;
	}	

SFileRecord *tmpAs2 ;

	tmpAs2 = NULL;
	do {
		if(tmpAs2)
			tmpAs2 = readFromRealFile(tmpAs2->next);
		else tmpAs2 = readFromRealFile(tmpAs->getFirst()); 

			if( from==-1 && to==-1)
				azp->add(tmpAs2);
			else if( from==-1 ){
				if(tmpAs2->kiedy <= to) 
					azp->add(tmpAs2);
			}
			else if( to==-1 ){
				if(tmpAs2->kiedy >= from) 
					azp->add(tmpAs2);
			}
			else if( tmpAs2->kiedy >= from && tmpAs2->kiedy <= to )
				azp->add(tmpAs2);
	}	
	while(tmpAs2->next);

return azp;
}

// ta funkcja: ZMIENIC!

CFileAlarmList* COnList::readFromFile(long od_, long do_)
{
SFileRecord *tmpAsI = readFromRealFile(1);
SFileRecord *tmpAsII = readFromRealFile(wolny-RECORD_LENGTH);
long offset1=0, offset2=0;
if( od_ > do_ ){
	printf("Data 1 jest pozniejsza niz data 2!\n");
	return NULL;
	}
if( tmpAsI->kiedy > od_ ){
	printf("Logi zaczynaja sie dopiero: %s\n", time2str(tmpAsI->kiedy));
	offset1=1;
	}
if( tmpAsII->kiedy < od_ ){
	printf("Ostatni log jest z: %s\n", time2str(tmpAsII->kiedy));
	offset2 = wolny-RECORD_LENGTH;
	}
if( ! offset1 )
	offset1 = findInFile(LESS, od_);
if( ! offset2 )
	offset2 = findInFile(MORE, do_);


CFileAlarmList *azpl = new CFileAlarmList();
SFileRecord *ptrAs;

	while(offset1 <= offset2){
		ptrAs = readFromRealFile(offset1);
		azpl->add( getAlarmNameFromMemory(ptrAs), ptrAs);
		offset1+=RECORD_LENGTH;
		delete ptrAs;
	}

return azpl;
}

// a teraz readFromFile wszystko..
CFileAlarmList* COnList::readFromFile()
{
CFileAlarmList *azpl = new CFileAlarmList();
CFileAlarm *tmpAzpl;
CMemoryRecord *tmpAs;

	TIterator<CMemoryRecord> *i = 
		new TIterator<CMemoryRecord>(lista);
	
	while(!(i->endReached())) {
		tmpAs = i->next();
		tmpAzpl = readFromFile(tmpAs->getName());
		azpl->add(tmpAzpl);
	}

return azpl;
}

char* COnList::getAlarmNameFromMemory(SFileRecord *pAs) const
{
SFileRecord *tmpAs2 = pAs;
long poprz = pAs->poprz;

	while(poprz > 0) {
		tmpAs2 = readFromRealFile(poprz);
		poprz = tmpAs2->poprz;
		delete tmpAs2;
	}	
return getAlarmNameFromMemory( -poprz );
}

char* COnList::getAlarmNameFromMemory(int numer) const
{
CMemoryRecord *tmpAs = getMemoryRecord(numer);

	if(tmpAs)
		return tmpAs->getName();
	else
		return NULL;	
}

void COnList::turnOn(char *alarmName){
	changeOnOffState(alarmName, TURN_ON);
}
void COnList::turnOff(char *alarmName){
	changeOnOffState(alarmName, TURN_OFF);
}

// changeOnOffState: JESLI GO NIE MA, TO JEST DODAWANY!!
void COnList::changeOnOffState(char *alarmName, int stan)
{
CMemoryRecord *tmpAs;

	if( alarmName == NULL ){
		// dla wszystkich..
		TIterator<CMemoryRecord> *i = 
			new TIterator<CMemoryRecord>(lista);
		
		while(!(i->endReached())) {
			tmpAs = i->next();
			if(stan == TURN_ON)
				tmpAs->turnOn();
			else
				tmpAs->turnOff();
		} 
	delete i;	
	}
	else {
		tmpAs = getMemoryRecord(alarmName);
		if( !tmpAs )
			lista->add(tmpAs = 
				new CMemoryRecord(alarmName) );

			if(stan == TURN_ON)
				tmpAs->turnOn();
			else
				tmpAs->turnOff();
	} 

}

///////////////////////////////////////////////////////////////////////////

char *time2str(time_t czas){
time_t t1;
struct tm *t2;
static char ltime[30];

	if( czas == (time_t) -1) 
		t1 = time(NULL);
	else
		t1 = czas;

	t2 = localtime( &t1 ); 
	strftime(ltime, 25, "%Y-%m-%d %H:%M:%S", t2);
 
return ltime;
}

time_t str2time(char *string)
{
time_t actual = time(NULL);
struct tm t2;
struct tm *t3 = localtime(&actual);

	bzero(&t2, sizeof t2);	

	if( strptime(string, "%Y-%m-%d %H:%M", &t2) == NULL)
	   if( strptime(string, "%m-%d %H:%M", &t2) == NULL)
	      if( strptime(string, "%H:%M", &t2)    == NULL)
	         if( strptime(string, "%Y-%m-%d", &t2) == NULL)
	            if( strptime(string, "%m-%d", &t2)  == NULL){
			return (time_t) -1;
		    } else {
			t2.tm_year = t3->tm_year;
		      }	
		 else {}
	      else {
			t2.tm_year = t3->tm_year;
			t2.tm_mon = t3->tm_mon;
			t2.tm_mday = t3->tm_mday;
	      }	
	   else
			t2.tm_year = t3->tm_year;

return mktime(&t2);
}
