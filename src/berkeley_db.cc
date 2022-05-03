//
// Baza danych systemu codecook
//

/* $Id: berkeley_db.cc,v 1.2 2000/02/02 00:33:06 code Exp $ */

#include "berkeley_db.h"
#include "cstable.h"
#include "logger.h"

Berkeley_DBMS::Berkeley_DBMS() 
{
	cstable->logger->Log("Starting database system (Berkeley DB 3.x), $Revision: 1.2 $");
}

Berkeley_DBMS::~Berkeley_DBMS() 
{
}

DB_Table* Berkeley_DBMS::Create_Table()
{
	return NULL;
}

DB_Table* Berkeley_DBMS::Get_Table(string table_name)
{
	return NULL;
}

void Berkeley_DBMS::Free_Table(DB_Table *table)
{
}