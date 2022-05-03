//
// Baza danych systemu Codecook
//
// $Id: cddb.h,v 1.1 2000/01/31 06:17:48 code Exp $

//
// Udostêpnia ró¿ne implementacje bazy
//
// Tutaj sa mechanizmy do latwego wyboru metody bazy danych
// Domyslnie uzywa bazy Berkeley

#ifndef __CDDB_H__
#define __CDDB_H__

#include <pthread.h>

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "berkeley_db.h"
#include "sql.h"

// FIXME - SelectMethod(), etc.

DB_DBMS* Create_DBMS();

#endif
