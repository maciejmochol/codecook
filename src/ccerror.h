/* ccerror.h */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _CCERROR_H_
#define _CCERROR_H_

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <iostream>
#include <string>

// Main exceptions
//////////////////

class exc_Codecook { 
public:
	string s, throw_from;
	int code;

	exc_Codecook() : s(""), throw_from(""), code(0) { }; 
	exc_Codecook(string _s) : s(_s), throw_from(""), code(0) { }; 
	void print(ostream &o) const { 
		if (throw_from != "")
			o << s << " (from " << throw_from << ") " << endl; 
		else
			o << s << endl; 
	};
	friend ostream& operator << (ostream &o, const exc_Codecook &exc) { 
		exc.print(o);
		return o;
	};
};

#define cc_exception(_code, name, base, text) \
	class name : public base { \
	public: \
		name() { code = _code; s = text; } \
		name(string _s) { code = _code; s = text; throw_from = _s; } \
		name(exc_Codecook& exc) { code = exc.code; s = exc.s; throw_from = exc.throw_from; }\
		}

cc_exception(1, exc_CannotAlloc, exc_Codecook, "Cannot alloc memory");

// Exceptions from TSortedArray
///////////////////////////////

cc_exception(11, exc_TSortedArray, exc_Codecook, "TSortedArray error");

cc_exception(12, exc_TSortedArray_Empty, exc_TSortedArray, "Empty array");
	// Pusta tablica
	
cc_exception(13, exc_TSortedArray_NotFound, exc_TSortedArray, "Element not found");
	// Nie znaleziono
	
cc_exception(14, exc_TSortedArray_AlreadyExists, exc_TSortedArray, "Element already exists");
	// Wkladany element juz istnieje

cc_exception(15, exc_TSortedArray_OutOfBounds, exc_TSortedArray, "Index out of bounds");
	// indeks poza zakresem


// Exceptions from TIndex
/////////////////////////

cc_exception(21, exc_TIndex, exc_Codecook, "TIndex error");

cc_exception(22, exc_TIndex_AlreadyExists, exc_TIndex, "Element already exists");
	// Wkladany element juz istnieje

cc_exception(23, exc_TIndex_NotFound, exc_TIndex, "Element not found");
	// Nie znaleziono

// Exceptions from CD_Tree
//////////////////////////

cc_exception(31, exc_CD_Tree, exc_Codecook, "CD_Tree error");

cc_exception(32, exc_CD_Tree_NodeNotFound, exc_CD_Tree, "Node not found"); 
	// Nie znaleziono node

cc_exception(33, exc_CD_Tree_NonEmptyNodeExists, exc_CD_Tree, "Non-empty node exists"); 
	// Istnieje node typu innego niz EMPTY

cc_exception(34, exc_CD_Tree_CannotInsertAlias, exc_CD_Tree, "Empty node cannot be replaced by alias"); 
	// Node typu EMPTY nie moze byc zastapiona przez alias

cc_exception(35, exc_CD_Tree_ChannelReadOnly, exc_CD_Tree, "Channel read-only"); 
	// Kanal tylko do odczytu

cc_exception(36, exc_CD_Tree_ChannelNotFound, exc_CD_Tree, "Channel not found"); 
	// Nie znaleziono kanalu

cc_exception(37, exc_CD_Tree_Alias, exc_CD_Tree, "Alias error"); 
	// B³±d dowiazania

cc_exception(38, exc_CD_Tree_UnresolvedAlias, exc_CD_Tree_Alias, "Unresolved alias"); 
	// Nierozwiazywalne dowiazanie

cc_exception(39, exc_CD_Tree_Alias_TooManyLinks, exc_CD_Tree_UnresolvedAlias, "Too many links or cycle in tree"); 
	// Za duzo dowiazan lub cykl w drzewie

// Exceptions from Device_Param_Table
/////////////////////////////////////

cc_exception(41, exc_Device_Param_Table, exc_Codecook, "Device Param Table error");

cc_exception(42, exc_Device_Param_Table_BadData, exc_Device_Param_Table, "Bad data");
	// Bledne dane

cc_exception(43, exc_Device_Param_Table_BadHandle, exc_Device_Param_Table, "Bad handle");
	// Bledny uchwyt

cc_exception(44, exc_Device_Param_Table_ShouldBeReader, exc_Device_Param_Table, "Bad blocking: should be reader");
	// Bledne blokowanie - powinien byc reader

cc_exception(45, exc_Device_Param_Table_ShouldBeWriter, exc_Device_Param_Table, "Bad blocking: should be writer");
	// Bledne blokowanie - powinien byc reader


// Exceptions from Param_Tables
///////////////////////////////

cc_exception(51, exc_Param_Tables, exc_Codecook, "Param Tables error");

cc_exception(52, exc_Param_Tables_TableNotFound, exc_Param_Tables, "Table not found"); 
	// Tablicy nie znaleziono

cc_exception(53, exc_Param_Tables_TableExists, exc_Param_Tables, "Table exists");
	// Tablica o podanej nazwie istnieje


extern void ThrowCorrectException(exc_Codecook &exc);
	// zdefiniowane w cstable.cc

#endif
