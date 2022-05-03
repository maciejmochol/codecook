/* $Id: parser_mathforms.cc,v 1.4 2000/04/25 17:52:36 code Exp $ */


#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <strstream>
#include <math.h>

#include "parser_mathforms.h"

extern char* time2str(long time);

//
// Math Functions
//

double f_abs(double param){
	if(param > 0)
		return param;
	else
		return -param;
}	

struct string_func_table{
	char *name;
	double (*fun)(double);
}
	math_fun[] = {
		{ "sin", sin   },
		{ "cos", cos   },
		{ "tg", tan    },
		{ "tan",tan    },
		{ "abs", f_abs },
		{ "exp", exp   },
		{ "ln" , log   },
		{ "log", log10 },
		{ "lg", log10  },
		{ "sqrt", sqrt },
		{ NULL, NULL   }
	};

void * string2fun( char * string )
{
int i;
	for( i=0; math_fun[i].name != NULL; i++)
		if( !strcmp( math_fun[i].name, string ) )
			return (void*) math_fun[i].fun;

return NULL;
}

//
// CCD_Address
//


DataRecord CCD_Address::calculate() 
{
DataRecord lRV;

	if (!address || !tree_ptr) return DataRecord();

	if (address->channel == "") address->channel = "VALUE";	

	lRV = tree_ptr->GetChannelDataRecord(*address);

	return lRV;
}

//
// CFunction
//

CFunction::CFunction( char* funname, CValue* _arg ) { 
	arg = _arg; 
	f = (double (*)(double)) string2fun(funname); 
};

DataRecord CFunction::calculate() { 

	if (!arg || !f) return DataRecord();
	DataRecord dr = arg->calculate(); 
	if (dr.val_type == STRING_VALUE) return DataRecord();
	if (dr.val_type == INT_VALUE) return DataRecord(DEFINITE, f(dr.value_i));
	return DataRecord(DEFINITE, f(dr.value));
};

//
// CNegation
//

DataRecord CNegation::calculate() { 

	if (!arg) return DataRecord();
	DataRecord dr = arg->calculate(); 
	if (dr.val_type == STRING_VALUE) return DataRecord();
	if (dr.val_type == INT_VALUE) return DataRecord(DEFINITE, - dr.value_i);
	return DataRecord(DEFINITE, -dr.value);
};

//
// CArithmeticExpression
//

DataRecord CArithmeticExpression::calculate() { 

	if (!left || !right) return DataRecord();

	switch (operation) {
		case ADD : return Addiction();
		case SUB : return Substraction();
		case MULT : return Multiplication();
		case DIV : return Division();
		case POW : return Power();
	}
	assert(0); // Strange operation
	return DataRecord();
};

DataRecord CArithmeticExpression::Addiction() { 
	DataRecord l, r;
	double dl, dr;

	l = left->calculate();
	r = right->calculate();

	if (l.val_type == STRING_VALUE || r.val_type == STRING_VALUE) {
		// string mode
		if (l.val_type != r.val_type) return DataRecord();
		string sl = string(l.c);
		string sr = string(r.c);
		return DataRecord(DEFINITE, sl + sr);
	}

	if (l.val_type == INT_VALUE) dl = l.value_i; else dl = l.value;
	if (r.val_type == INT_VALUE) dr = r.value_i; else dr = r.value;
	return DataRecord(DEFINITE, dl + dr);
};

DataRecord CArithmeticExpression::Substraction() { 
	DataRecord l, r;
	double dl, dr;

	l = left->calculate();
	r = right->calculate();

	if (l.val_type == STRING_VALUE || r.val_type == STRING_VALUE) 
		return DataRecord();

	if (l.val_type == INT_VALUE) dl = l.value_i; else dl = l.value;
	if (r.val_type == INT_VALUE) dr = r.value_i; else dr = r.value;
	return DataRecord(DEFINITE, dl - dr);
};

DataRecord CArithmeticExpression::Multiplication() { 
	DataRecord l, r;
	double dl, dr;

	l = left->calculate();
	r = right->calculate();

	if (l.val_type == STRING_VALUE || r.val_type == STRING_VALUE) 
		return DataRecord();

	if (l.val_type == INT_VALUE) dl = l.value_i; else dl = l.value;
	if (r.val_type == INT_VALUE) dr = r.value_i; else dr = r.value;
	return DataRecord(DEFINITE, dl * dr);
};

DataRecord CArithmeticExpression::Division() { 
	DataRecord l, r;
	double dl, dr;

	l = left->calculate();
	r = right->calculate();

	if (l.val_type == STRING_VALUE || r.val_type == STRING_VALUE) 
		return DataRecord();

	if (l.val_type == INT_VALUE) dl = l.value_i; else dl = l.value;
	if (r.val_type == INT_VALUE) dr = r.value_i; else dr = r.value;
	if (dr == 0.0) return DataRecord();
	return DataRecord(DEFINITE, dl / dr);
};

DataRecord CArithmeticExpression::Power() { 
	DataRecord l, r;
	double dl, dr;

	l = left->calculate();
	r = right->calculate();

	if (l.val_type == STRING_VALUE || r.val_type == STRING_VALUE) 
		return DataRecord();

	if (l.val_type == INT_VALUE) dl = l.value_i; else dl = l.value;
	if (r.val_type == INT_VALUE) dr = r.value_i; else dr = r.value;
	if (dr == 0.0) return DataRecord();
	return DataRecord(DEFINITE, pow(dl, dr));
};

//
// CLogicalExpression
//

DataRecord CLogicalExpression::calculate() { 

	if (!left || !right) return DataRecord();

	DataRecord l, r;
	int il, ir;
	int result;

	l = left->calculate();
	r = right->calculate();

	if (l.val_type == STRING_VALUE || r.val_type == STRING_VALUE) 
		return DataRecord();

	if (l.val_type == DOUBLE_VALUE) il = l.value >= 1.0; else il = l.value_i;
	if (r.val_type == DOUBLE_VALUE) ir = r.value >= 1.0; else ir = r.value_i;

	switch (operation) {
		case AND:	result = (il && ir); break;
		case OR:	result = (il || ir); break;
		default: return DataRecord();
	}
	return DataRecord(DEFINITE, result);
};

//
// CConditionalExpression
//


DataRecord CConditionalExpression::calculate() { 

	if (!left || !right) return DataRecord();

	DataRecord l, r;
	double dl, dr;
	int result;

	l = left->calculate();
	r = right->calculate();

	if (l.val_type == STRING_VALUE || r.val_type == STRING_VALUE) {
		// string mode
		if (l.val_type != r.val_type) return DataRecord();
		string sl = string(l.c);
		string sr = string(r.c);
		switch (operation) {
			case EQUAL:	result = (sl == sr); break;
			case DIFFERENT:	result = (sl != sr); break;
			case BIGGER:	result = (sl > sr); break;
			case EBIGGER:	result = (sl >= sr); break;
			case SMALLER:	result = (sl < sr); break;
			case ESMALLER:	result = (sl <= sr); break;
			default: return DataRecord();
		}
		return DataRecord(DEFINITE, result);
	}

	if (l.val_type == INT_VALUE) dl = l.value_i; else dl = l.value;
	if (r.val_type == INT_VALUE) dr = r.value_i; else dr = r.value;

	switch (operation) {
		case EQUAL:	result = (dl == dr); break;
		case DIFFERENT:	result = (dl != dr); break;
		case BIGGER:	result = (dl > dr); break;
		case EBIGGER:	result = (dl >= dr); break;
		case SMALLER:	result = (dl < dr); break;
		case ESMALLER:	result = (dl <= dr); break;
		default: return DataRecord();
	}
	return DataRecord(DEFINITE, result);
};


// Mathforms parser

Math_Parser::Math_Parser(string s)
{
	lString = s;
}

Math_Parser::Math_Parser(char *pc)
{
	lString = string(pc);
}

void *Math_Parser::Parse(istream &in, ostream &out)
{
	
		//CConditionList *lpConditions = new CConditionList();
		//(CConditionList*)lpConditions, 
		
		mathforms_ParserStruct lPS(
			NULL, new mathforms_LexerClass(&in, &out), 
			(CD_Tree*)cstable->tree);

#ifdef YYDEBUG
	mathforms_debug  =  1;
#endif

		int ret = mathforms_parse((void*)&lPS);

		if(ret){
			delete lPS.list;
			error_line = lPS.lexer->GetLineNumber();
			delete lPS.lexer;
			return NULL;
		} else {
			delete lPS.lexer;
			return (void*) lPS.list;
		}		

}
