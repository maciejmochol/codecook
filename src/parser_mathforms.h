/* $Id: parser_mathforms.h,v 1.4 2000/04/25 17:52:36 code Exp $ */


#ifndef PARSER_MATHFORMS_H
#define PARSER_MATHFORMS_H

//#include "grammar_mathforms.tab.h"

#include <string>
#include <iostream>
#include "datastruct.h"
#include "parsing.h"
#include "alarmserv.h"
#include "cstable.h"


extern int mathforms_debug;
//#define YYDEBUG 1


#ifndef DONT_INCLUDE_FLEXLEXER_H
	#undef yyFlexLexer
	#define yyFlexLexer mathforms_FlexLexer
	#include "FlexLexer.h"
#endif
	
	class CAlarm;
	class CConditionList;
	class CConditions;
	class CConditions;
	class CExpressions;
	class CExpression;
	class CAlarmList;
	class CValue;
	class CD_Tree; 

enum { CCONDITION, CEXPRESSION };

class Math_Parser : public Parser {

// Tu nie powinno byc zwracane drzewo, lecz juz gotowa wartosc..	//
// czyli RetVal.. w ktorym miejscu powinno sie znajdowac wywolanie	//
// z cache'u? Chyba gdzies na wyzszym poziomie.				//

public:	
	Math_Parser(string s);
	Math_Parser(char *);

	// void *Parse(); // (void*) means (CValue*) here!
	void *Parse(istream &in, ostream &out);

	//CValue *ParseString(string);
	//CValue *ParseString(char *);

};



class mathforms_LexerClass; 
//class CConditionList;
int mathforms_parse(void*);
int mathforms_lex(YYSTYPE*, void*);
int mathforms_error(char*);

struct mathforms_ParserStruct {
	CValue *list; // w rzeczywistosci CConditionsList lub CExpressionList
	mathforms_LexerClass *lexer;
	CD_Tree *fCD_Tree;
	int type;

	void SetType(int ptype) { type = ptype; }	
	int GetType() { return type ; }	

	mathforms_ParserStruct(CValue *pVal=NULL, mathforms_LexerClass *
		pLexer = NULL, CD_Tree *pCD_Tree = cstable->tree) 
	{
		list=pVal; lexer=pLexer; 
		fCD_Tree = pCD_Tree; 
	}
};

class mathforms_LexerClass : public mathforms_FlexLexer {

	YYSTYPE *yylval; 
	int line_number;

public:
	mathforms_LexerClass(istream* pYYIn, ostream* pYYOut) : 
		mathforms_FlexLexer(pYYIn, pYYOut) { 
			yylval = new YYSTYPE; 
			line_number=0;
		}
	~mathforms_LexerClass()  { delete yylval; }

	YYSTYPE *GetYYlval() { return yylval; }
	virtual int yylex();
	
	void popState(){ yy_pop_state(); }
	void pushState(int state){ yy_push_state(state); }
	int  GetLineNumber() { return line_number; }
};

#endif
