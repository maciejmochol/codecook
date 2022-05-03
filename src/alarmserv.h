/* $Id: alarmserv.h,v 1.3 2000/04/26 18:53:32 code Exp $ */

#ifndef __CONTROLER_H__
#define __CONTROLER_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdlib.h>
#include "ptable.h"
#include "cstable.h"
#include "thread.h"
#include "cdtree.h"
#include "parser_mathforms.h"
#include "al_log.h"

typedef struct _DataRecord RetVal;

enum MathOperation { ADD, SUB, MULT, DIV, POW };
enum LogicOperation { AND, OR };
enum CondOperation { EQUAL, DIFFERENT, BIGGER, EBIGGER, SMALLER, ESMALLER };

//
// CValue
//

class CValue
{
public:
	CValue() { };
	virtual DataRecord calculate()  = 0 ;
};

//
// CCD_Address
//

class CCD_Address : public CValue
{
	CD_Tree *tree_ptr;
	CD_Address *address;
	
public:
	CCD_Address(char *pc, CD_Tree *ptree = cstable->tree) { 
		address = new CD_Address(pc); tree_ptr = ptree;
	};
	virtual ~CCD_Address() { if (address) delete address; }

	virtual DataRecord calculate();
};

//
// CDouble
//

class CDouble : public CValue 
{
	double d;
public:
	CDouble( double _d ) { d = _d; };
	virtual ~CDouble() { };
	DataRecord calculate() { 
		return DataRecord(DEFINITE, d); 
	};
};

//
// CString
//

class CString : public CValue 
{
	string s;
public:
	CString( char *_s ) { s = string(_s); };
	virtual ~CString() { };
	DataRecord calculate() { 
		return DataRecord(DEFINITE, s); 
	};
};

//
// CFunction
//

class CFunction : public CValue 
{
	CValue *arg;
	double (*f)(double);
public:
	CFunction( char* funname, CValue* _arg );
	virtual ~CFunction() { if (arg) delete arg; };
	DataRecord calculate();
};

//
// CNegation
//

class CNegation : public CValue 
{
	CValue *arg;
public:
	CNegation( CValue* _arg ) { arg = _arg; };
	virtual ~CNegation() { if (arg) delete arg; };
	DataRecord calculate();
};

//
// CArithmeticExpression
//

class CArithmeticExpression : public CValue 
{
	CValue *left, *right;
	int operation;
public:
	CArithmeticExpression( CValue* _left, CValue* _right, int _operation ) {
		left = _left; right = _right; operation = _operation; };
	virtual ~CArithmeticExpression() { 
		if (left) delete left; if (right) delete right; };

	DataRecord calculate();
	DataRecord Power();
	DataRecord Addiction();
	DataRecord Substraction();
	DataRecord Multiplication();
	DataRecord Division();
};

//
// CLogicalExpression
//

class CLogicalExpression : public CValue 
{
	CValue *left, *right;
	int operation;
public:
	CLogicalExpression( CValue* _left, CValue* _right, int _operation ) {
		 left = _left; right = _right; operation = _operation; };
	virtual ~CLogicalExpression() {
		if (left) delete left; if (right) delete right; };
	DataRecord calculate();
};

//
// CConditionalExpression
//

class CConditionalExpression : public CValue 
{
	CValue *left, *right;
	char operation;
public:
	CConditionalExpression( CValue* _left, CValue* _right, char _operation ) {
		left = _left; right = _right; operation = _operation; };
	virtual ~CConditionalExpression() { 
		if (left) delete left; if (right) delete right; };
	DataRecord calculate();
};


// WHAT TO DO WITH THESE FUNCTIONS ???

char *time2str(long);
long str2time(char*);
// bool parse_string(char*string_to_parse, int kind_of_string);


//###############################################################

class CD_Node_Definable : public CD_Node {

	CValue *val_tree;
public:

	CD_Node_Definable();
	~CD_Node_Definable();

	void Use() { };
	virtual DataRecord GetValue();
	void ReParse();

	string GetChannel(string channel, int &found);
	string GetChannel(string channel) { 
		int found; 
		return GetChannel(channel, found); 
	}
	void AddChannel(string name, CD_Channel *channel);
	void AddChannel(string name, string value);
	void AddChannel(string name, int value);
	void SetChannel(string name, string value);
	void SetChannel(string name, int value);	
};

class CD_Node_Alarm : public CD_Node_Definable {
public:
	CD_Node_Alarm();
	~CD_Node_Alarm();
	void Use() { };
	bool isOn();
};

class CD_Node_Formula : public CD_Node_Definable {
public:
	CD_Node_Formula();
	~CD_Node_Formula();
	void Use() { };
};

class CD_Node_Constant : public CD_Node_Definable {
public:
	CD_Node_Constant();
	~CD_Node_Constant();
	void Use() { };
};

//
// CD_Tree commands
//

// 
// class AlarmServ
//

class AlarmServ : public Codecook_Thread 
{
public:
	AlarmServ();
	virtual void * Handler();
};

#endif
