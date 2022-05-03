//
// Baza danych systemu codecook
//

/* $Id: cddb.cc,v 1.1 2000/01/31 06:17:48 code Exp $ */

// Create_DBMS
// creates DB_DBMS for particular method
// by default uses Berkeley DB method

#include "cddb.h"

DB_DBMS* Create_DBMS()
{
	return new Berkeley_DBMS();
}
