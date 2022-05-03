/* $Id: al_log.h,v 1.3 2000/04/25 17:52:36 code Exp $ */
/* 
 * log.h - Creating and managing of logfiles
 * Vooyeck '99-07
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include "alarmserv.h"
#include "datastruct.h"

#define RECORD_LENGTH 43 // SPACE, (999.999.999.999, SPACE)*3, COLON 
#define MAX_ALARM_NAME 30
#define DEFALULT_LOG_FILENAME "alarm.log"

enum { PREV, NEXT, MORE, LESS, LOGSYNC, CLOSE, TURN_ON, TURN_OFF };

class CMemoryRecord {
private:
	char *name;
	long  firstRecord;
	long  ostatni;
	int   aktywny;
public:
	CMemoryRecord(char*pnazwa, long p=0, long o=0, int a=0); 
	~CMemoryRecord() { delete name; }

	char *getName()  const { return name; }
	long  getFirst() const { return firstRecord; }
	long  getLast()  const { return ostatni; }
	void  setFirst(long w) { firstRecord = w; }
	void  setLast(long o) { ostatni = o; }

	void  turnOn(){ aktywny = 1; }
	void  turnOff(){ aktywny = 0; }
	int   isOn() const { return aktywny; }
};

struct SFileRecord {
	long kiedy;
	long poprz;
	long next;
	int aktywny; // 0 lub 1
}; 

class CFileAlarm{
	char *name;
	TList<SFileRecord> *lista; // nieparzyste daty to rozpoczecie
				// parzyste - zakonczenie alarmu
public:
	CFileAlarm() { lista = new TList<SFileRecord> ; }
	CFileAlarm(char*name);
	~CFileAlarm();
			
	void  add(SFileRecord *rek) { lista->add( rek ); }
	char *getName() const { return name; }
	void  print();
};


class CFileAlarmList{
	TList<CFileAlarm> *lista_alarmow; 
public:
	CFileAlarmList() { lista_alarmow = new TList<CFileAlarm>; }
	~CFileAlarmList();

	void        print();
	CFileAlarm *getAlarmFromFile(char *name) const;
	CFileAlarm *add(CFileAlarm *pazp);
	CFileAlarm *add(char *name);
	void        add(char *name, SFileRecord *rek);
};

class COnList{
	TList<CMemoryRecord> *lista;

	char *fileName;
	FILE *plik;
	bool  ready;

		/* Te rzeczy sa w pliku */
	long  wolny;// wolne miejsce w pliku 
	long  log_p, log_ost; // data pierwszego i ostatniego loga
	int   aktywny; // Stan alarmu

	void  saveList(); // te z pamieci
	bool  loadList();

	long  fit(long offset);
	long  findInFileRapidly(int crit, long val, long start=0, long step=0);
	long  findInFile(int criteria, long value);

	SFileRecord *readFromRealFile() const;	
	SFileRecord *readFromRealFile(long offset) const;	

	void  changeFileLog(long offset, int co, long wartosc);
	void  addFileLog(long kiedy, long poprz, long next, int stan);
	void  changeOnOffState(char *alarmName, int stan);

	char *getAlarmNameFromMemory(SFileRecord*) const;
	char *getAlarmNameFromMemory(int i) const;

	//SFileRecord *znajdz_ostatni(CMemoryRecord *as);	// NU
public:
	COnList(char*np);
	~COnList();

	void  add(CMemoryRecord *i);
	void  turnOn(char  *alarm_name=NULL); // NULL uaktywnia wszystkie!
	void  turnOff(char *alarm_name=NULL); // -||-

	bool  isReady() const { return ready; }
	char *getFileName() const { return fileName; }
	int   getNoOfElements() const { return lista->getLength(); }
	long  getFirstRecordTime() const { return log_p; }
	long  getLastRecordTime() const { return log_ost; }

	void  print_liste();

	void  logFromList(TList<CMemoryRecord> *memoryList); 
	void  logShutdown(); // turnOffamy wszystkie przy zabijaniu serwera
	void  log();  
	void  logsync(int jak=LOGSYNC);

	CFileAlarm     *readFromFile(char *alarmName, long fr=-1, long to=-1); 
	CFileAlarmList *readFromFile(long od_, long do_);
	CFileAlarmList *readFromFile();
	CMemoryRecord  *getMemoryRecord(char*name)  const;
	CMemoryRecord  *getMemoryRecord(int numerek)const;
};

#endif
