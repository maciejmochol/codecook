//
// Baza danych systemu Codecook
//
// $Id: database.h,v 1.4 2000/11/05 21:20:55 code Exp $

#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <pthread.h>

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "cdtree.h"

class DB_Day;
class DB_Table;
class DB_DBMS;

enum Day_Type { _1s, _10s, _1min, _10min, _1h };

// Ilo¶æ elementów w tablicy dnia:
//
// 1s - ka¿da sekunda dnia (24*3600 elementów)
// 10s - co 10 sekund (24*60*6 elementów)
// 1min - co minuta (24*60 elementów)
// 10min - co 10 minut (24*6 elementów)
// 1h - co godzina (24 elementy)

enum Data_Type { BYTE, WORD, LONG, MEMO };

// D³ugo¶æ danych:
//
// BYTE - 1 bajt
// WORD - 2 bajty
// LONG - 4 bajty
// MEMO - obszar danych o sta³ej d³ugo¶ci

/*

UWAGA

Cache dni powinien byæ realizowany z dok³adno¶ci± do dnia dzisiejszego
Dzieñ bie¿±cy nigdy nie jest zapisywany do cache'u

W przypadku aktualizacji dnia innego ni¿ bie¿±cy cache 
równie¿ powinien byæ aktualizowany (mo¿e nawet czyszczony)

Na razie olej cache (dla wersji Berkeley DB powinien wystarczyc
wewnetrzny cache)

*/

// DB_Day
// klasa odpowiadaj±ca pobieranemu dniowi z bazy
// realizuje funkcje ¶rednich poddniowych

//
// Zarz±dzaniem dniami zajmuje siê klasa DB_Table
// tworzeniem nowego dnia zajmuje siê Get_Day()
// zwalnianiem - Free_Day()
//

class DB_Day {
	int empty;
	Data_Type data_t;
	Day_Type  day_t;

	int item_size; // rozmiar pojedynczej danej
	int items; // ilosc danych w dniu	
	int data_size; // data_size = item_size * items

	int month, day, year;

public:	
	DB_Day(Day_Type dayt, Data_Type datat);
	DB_Day(Day_Type dayt, int memo_size); // dla MEMO
	~DB_Day();

	int IsEmpty();

	// ¶rednie poddniowe	
	// ...
};

// DB_Table
// klasa odpowiadaj±ca tabeli dni
// realizuje funkcje ¶rednich naddniowych 
// Okre¶lone implementacje np. SQL albo DB powinny byæ pochodnymi tej klasy
//
// klasa ta powinna realizowaæ równie¿ cache dni

class DB_Table {
public:
	int year; 		// rok który opisuje tabela
	long version;		// wersja bazy
	string table_name;	// nazwa tabeli
	CD_Address addr;	// ¶cie¿ka
	Day_Type day_t;
	Data_Type data_t;
	int data_size;
	
	DB_Table();
	virtual ~DB_Table();
	
// Pobierz dzieñ z tabeli
// je¶li nie ma go w bazie to stwórz pusty z flag± "empty"
	virtual DB_Day* Get_Day(int month, int day) = 0;

// Zapisz dzieñ do tabeli, jak nie ma to stwórz
	virtual DB_Day* Write_Day(int month, int day) = 0;
	
// Zwolnij dzieñ z tabeli
	virtual void Free_Day(DB_Day *day) = 0;	
	
// ? ¶rednie naddniowe

};

// DB_DBMS
// abstrakcyjna klasa definiuj±ca us³ugi
// Systemu Zarz±dzania Baz± Danych
// Okre¶lone implementacje np. SQL albo DB powinny byæ pochodnymi tej klasy
//
// klasa powinna realizowaæ cache tabel

class DB_DBMS {
public:

	DB_DBMS();
	virtual ~DB_DBMS();

// Pobierz nazwê tabeli
	virtual string GetTableName(CD_Address &addr, int year);

// Stwórz tabelê
	virtual DB_Table* Create_Table() = 0;

// Pobierz tabelê i ustaw jej konfiguracjê, zwróæ NULL je¶li nie ma takiej tabeli
	virtual DB_Table* Get_Table(string table_name) = 0;

// Zwolnij tabelê
	virtual void Free_Table(DB_Table *table) = 0;
};

#endif
