//
// Baza danych systemu Codecook
// Implementacja dla Berkeley DB
//
// $Id: berkeley_db.h,v 1.2 2000/02/02 00:33:06 code Exp $

#ifndef __BERKELEY_DB_H__
#define __BERKELEY_DB_H__

#include <pthread.h>

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "cdtree.h"
#include "database.h"

class Berkeley_Table : DB_Table {
public:
	
	DB_Day* Get_Day(int month, int day);
	DB_Day* Write_Day(int month, int day);
	void Free_Day(DB_Day *day);	
};

class Berkeley_DBMS : public DB_DBMS {
public:
	Berkeley_DBMS();
	~Berkeley_DBMS();

// Pobierz nazwê tabeli
//	string GetTableName(CD_Address &addr, int year);


// Stwórz tabelê
	DB_Table* Create_Table();

// Pobierz tabelê i ustaw jej konfiguracjê
	DB_Table* Get_Table(string table_name);

// Zwolnij tabelê
	void Free_Table(DB_Table *table);
};

#endif
