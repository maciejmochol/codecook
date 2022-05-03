%{


/* $Id: grammar_mathforms.y,v 1.6 2000/04/25 17:52:36 code Exp $ */

#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "parser_mathforms.h"

#define YYPARSE_PARAM PStruct
#define YYLEX_PARAM PStruct

%}

%token mathforms_WYBOR
%token mathforms_WYRAZENIE
%token mathforms_NUM
%token mathforms_SEMICOLON
%token mathforms_CHAR

%token mathforms_IDENTIFIER
%token mathforms_FULLPATH
%token mathforms_TEXT

%token mathforms_POW
%token mathforms_FUN
%token mathforms_NEG

%token mathforms_RELACJA

%token mathforms_OR
%token mathforms_AND
%token mathforms_DBL_QUOT

%nonassoc mathforms_L_BRACE
%nonassoc mathforms_R_BRACE

%left mathforms_AND
%left mathforms_OR
%left mathforms_RELACJA
%left '+' '-'
%left '*' '/'
%left mathforms_NEG
%right '^'

%type <value>  	math_expression
%type <value>  	expression

%type <str> mathforms_IDENTIFIER
%type <str> mathforms_FULLPATH
%type <str> mathforms_FUN
%type <str> mathforms_TEXT
%type <d> mathforms_NUM
%type <c> mathforms_RELACJA
%type <c> "*"
%type <c> "/"
%type <c> "+"
%type <c> "-"
%type <c> "^"

%pure_parser

/* Grammar follows */
%%

mathform:
	expression {  
		((mathforms_ParserStruct*)PStruct)->list = $1;
	}
;

expression:
	math_expression { $$ = $1;  }
	| expression mathforms_AND expression { $$ = new CLogicalExpression($1, $3, AND); }
	| expression mathforms_OR expression { $$ = new CLogicalExpression($1, $3, OR); }
	| expression mathforms_RELACJA expression { $$ = new CConditionalExpression($1, $3, $2); }
	| expression '+' expression { $$ = new CArithmeticExpression($1, $3, ADD); }
	| expression '-' expression { $$ = new CArithmeticExpression($1, $3, SUB); }
	| expression '*' expression { $$ = new CArithmeticExpression($1, $3, MULT); }
	| expression '/' expression { $$ = new CArithmeticExpression($1, $3, DIV); }
	| '-' expression %prec mathforms_NEG { $$ = new CNegation($2); }
	| expression '^' expression { $$ = new CArithmeticExpression($1, $3, POW); }
	| mathforms_L_BRACE expression mathforms_R_BRACE { $$ = $2; }
;

math_expression:
	mathforms_NUM { $$ = new CDouble($1); }
	| mathforms_TEXT { $$ = new CString($1); free($1); }
	| mathforms_FULLPATH { $$ = new CCD_Address($1); free($1); }
	| mathforms_FUN mathforms_L_BRACE expression mathforms_R_BRACE {
		 $$ = new CFunction($1, $3); free($1); }
;

%%

/* End of grammar */

int mathforms_error(char*p){
	p=NULL; return -1;
}
