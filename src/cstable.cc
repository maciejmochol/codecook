/* $Id: cstable.cc,v 1.8 2000/01/31 06:15:31 code Exp $ */

#include "cstable.h"
#include "device_list.h"
#include "protocol.h"
#include "thread.h"
#include "cdtree.h"
#include "ptable.h"

//
// Class Codecook_System_Table
//

Codecook_System_Table::Codecook_System_Table() {
	meaner = NULL;
	manager = NULL;
	netmanager = NULL;
	alarmserv = NULL;
	tree = NULL; // inicjalizacja w manager, potrzebuje protokolu
	devices = NULL;
	threadList = NULL;
	ptable = NULL;
	protocol = new Protocol();
	dbms = NULL;
};

Codecook_System_Table *cstable;

void ThrowCorrectException(exc_Codecook &exc)
{
	switch (exc.code) {

		case 1: throw exc_CannotAlloc(exc);

		case 11: throw exc_TSortedArray(exc);

		case 12: throw exc_TSortedArray_Empty(exc);
		case 13: throw exc_TSortedArray_NotFound(exc);
		case 14: throw exc_TSortedArray_AlreadyExists(exc);
		case 15: throw exc_TSortedArray_OutOfBounds(exc);

		case 21: throw exc_TIndex(exc);
		case 22: throw exc_TIndex_AlreadyExists(exc);
		case 23: throw exc_TIndex_NotFound(exc);

		case 31: throw exc_CD_Tree(exc);
		case 32: throw exc_CD_Tree_NodeNotFound(exc);
		case 33: throw exc_CD_Tree_NonEmptyNodeExists(exc);
		case 34: throw exc_CD_Tree_CannotInsertAlias(exc);
		case 35: throw exc_CD_Tree_ChannelReadOnly(exc);
		case 36: throw exc_CD_Tree_Alias(exc);
		case 37: throw exc_CD_Tree_UnresolvedAlias(exc);
		case 38: throw exc_CD_Tree_Alias_TooManyLinks(exc);

		case 41: throw exc_Device_Param_Table(exc);
		case 42: throw exc_Device_Param_Table_BadData(exc);
		case 43: throw exc_Device_Param_Table_BadHandle(exc);
		case 44: throw exc_Device_Param_Table_ShouldBeReader(exc);
		case 45: throw exc_Device_Param_Table_ShouldBeWriter(exc);

		case 51: throw exc_Param_Tables(exc);
		case 52: throw exc_Param_Tables_TableNotFound(exc);
		case 53: throw exc_Param_Tables_TableExists(exc);
	
		default: throw exc_Codecook(exc);
	}
};
