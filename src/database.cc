//
// Baza danych systemu codecook
//

/* $Id: database.cc,v 1.3 2000/11/05 21:20:55 code Exp $ */

#include "database.h"

// Day

DB_Day::DB_Day(Day_Type dayt, Data_Type datat) { 

	day_t = dayt;
	data_t = datat;
	empty = 1;

	switch (data_t) {
		case BYTE: 
			item_size = 1;
			break;
		case WORD: 
			item_size = 2;
			break;
		case LONG: 
			item_size = 4;
			break;
		case MEMO: 
			item_size = 1; // MEMO ? to niech bedzie 1
			break;
	};

	switch (day_t) {
		case _1s:
			items = 24 * 60 * 60;
			break;
		case _10s:
			items = 24 * 60 * 6;
			break;
		case _1min:
			items = 24 * 60;
			break;
		case _10min:
			items = 24 * 6;
			break;
		case _1h:
			items = 24;
			break;
	};

	data_size = items * item_size;


};

DB_Day::DB_Day(Day_Type dayt, int memo_size) { 

	day_t = dayt;
	data_t = MEMO;
	empty = 1;

	item_size = memo_size;

	switch (day_t) {
		case _1s:
			items = 24 * 60 * 60;
			break;
		case _10s:
			items = 24 * 60 * 6;
			break;
		case _1min:
			items = 24 * 60;
			break;
		case _10min:
			items = 24 * 6;
			break;
		case _1h:
			items = 24;
			break;
	};

	data_size = items * item_size;


};

DB_Day::~DB_Day() {
};

int DB_Day::IsEmpty() { 
	return empty;
};


// DBMS

DB_DBMS::DB_DBMS()
{
}

DB_DBMS::~DB_DBMS()
{
}

string DB_DBMS::GetTableName(CD_Address &addr, int year)
{
	return "";
}
