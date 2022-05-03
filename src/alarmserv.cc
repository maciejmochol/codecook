/* $Id: alarmserv.cc,v 1.4 2000/04/26 18:53:32 code Exp $ */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "alarmserv.h"
#include <unistd.h>
#include <stdio.h>

// class CD_Node_Definable

CD_Node_Definable::CD_Node_Definable()
{
	val_tree = NULL;
	// AddChannel("TYPE", "DEFINABLE"); // 
}

CD_Node_Definable::~CD_Node_Definable() { delete val_tree; }

// Poprawic locki i unlocki..
// NODE chyba bedzie dziedziczone po Room (powinno byc!)
// No no no no no !!!!! Bo sie spieprzy - code
// Po room jest cdtree, nie odwolujemy sie bezposrednio
// do CD_Node

DataRecord CD_Node_Definable::GetValue() 
{
	if (val_tree)
		return val_tree->calculate();
	else 
		return DataRecord();
}

string CD_Node_Definable::GetChannel(string channel, int &found)
{
	if (channel == "VALUE") {
		found = 1;

		DataRecord lRV = GetValue();
		char buf[200], buf1[200];

		if( lRV.type == INDEFINITE ) 
			return "INDEFINITE";

		if (lRV.val_type == STRING_VALUE ) {
			return string(lRV.c);			

		} if( lRV.val_type == DOUBLE_VALUE ) {
		
			sprintf(buf1, "%%.%sf", "2");
			sprintf(buf, buf1, lRV.value);

		} else if( lRV.val_type == INT_VALUE ) {

			sprintf(buf, "%d", lRV.value_i);
		}

	
		return string(buf);
	}

	return CD_Node::GetChannel(channel, found);
}

void CD_Node_Definable::AddChannel(string name, CD_Channel *channel)
{
	CD_Node::AddChannel(name, channel);
	ReParse();
}

void CD_Node_Definable::AddChannel(string name, string value)
{
	CD_Node::AddChannel(name, value);
	ReParse();
}

void CD_Node_Definable::AddChannel(string name, int value)
{
	CD_Node::AddChannel(name, value);
	ReParse();
}

void CD_Node_Definable::SetChannel(string name, string value)
{
	CD_Node::SetChannel(name, value);
	ReParse();
}

void CD_Node_Definable::SetChannel(string name, int value)
{
	CD_Node::SetChannel(name, value);
	ReParse();
}


// A moze powinno zwracac wartosc???
void CD_Node_Definable::ReParse()
{
	string lS = GetChannel("FORMULA");	

	if( lS == "" )
		return;

	Math_Parser lMP(lS); // Poprawic konstruktory !!!
	val_tree = (CValue*) lMP.ParseString(lS);
}

CD_Node_Alarm::CD_Node_Alarm(){
	AddChannel("TYPE", "ALARM");
}

CD_Node_Alarm::~CD_Node_Alarm(){}

CD_Node_Formula::CD_Node_Formula(){
	AddChannel("TYPE", "FORMULA");
}

CD_Node_Formula::~CD_Node_Formula(){}

CD_Node_Constant::CD_Node_Constant(){
	AddChannel("TYPE", "CONSTANT");
}

CD_Node_Constant::~CD_Node_Constant(){}


// 
// class AlarmServ
//

AlarmServ::AlarmServ() : Codecook_Thread("AlarmServ", ALARMSERV) {
	Start();
};


void * AlarmServ::Handler()
{
	Log("AlarmServer starting...");

	while(1) { sleep(1); };

	return NULL;
};

