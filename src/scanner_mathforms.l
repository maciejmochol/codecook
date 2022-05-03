
%{
/* $Id: scanner_mathforms.l,v 1.6 2000/04/25 17:52:36 code Exp $ */
/* 
 * lang.l - flex input file. This file is made to work with bison. 
 * Contains definitions of tokens for Alarm Description Language. 
 * Vooyeck '99-06
 */

#define DONT_INCLUDE_FLEXLEXER_H
#include "parser_mathforms.h"
#include <string.h>

#define YY_NO_UNPUT // cztoby uniknac Warningoow (-Wall)
// TEXTCHAR		[a-zA-Z0-9_(),. ]

int mathforms_lex(YYSTYPE* yylval, void*pPS) {

	int ret = ((mathforms_ParserStruct*)pPS)->lexer->yylex();
	*yylval = (YYSTYPE)*((mathforms_ParserStruct*)pPS)->lexer->GetYYlval();	

return ret;	
}

%}

%s DEF_QUOTED_ID
%option NOYYWRAP
%option c++
%option stack
%option yyclass="mathforms_LexerClass"
%option prefix="mathforms_"

LETTER			[a-zA-Z_]
LLETTER			[a-z_]
HLETTER			[A-Z_]
PATHCHAR		[a-zA-Z0-9_]
CRYPTEDCHAR		[a-zA-Z0-9./]
CYPHER			[0-9]
DIFFCHAR		[!@#$%^&*()_+{}:"~<>,./;[]-=\\|]
CHAR			[a-z0-9!@#$%^&()_{}:"~<>,.\[\]=-\\|+*/\\;]
TEXTCHAR		[a-zA-Z0-9!@#$%^&() _{}:~<>,.\[\]=-\\|+*/\\;]
SP			[ \t]
%%

#.*\n			line_number++;  /* komentarz */
{CYPHER}*"."*{CYPHER}+		{ 
				yylval->d = atof(yytext);
				return mathforms_NUM; 
			}
"sin"|"cos"|"abs"|"tan"|"tg"|"exp"|"ln"|"log"|"lg"|"sqrt" {
			yylval->string = strdup(yytext);
			return mathforms_FUN; 

			}
{LETTER}{PATHCHAR}*(:{PATHCHAR}+)*{SP}*(","{SP}*{HLETTER}+)*  {
			yylval->string = strdup(yytext); 
			return mathforms_FULLPATH; 
			}
"\'"{TEXTCHAR}*"\'"	{
			yylval->string = strdup(yytext + 1); 
			yylval->string[strlen(yylval->string) - 1] = 0;
			return mathforms_TEXT;
			}

"+"			return '+';
"-"			return '-';
"*"			return '*';
"/"			return '/';
"^"			return '^';

";"			return mathforms_SEMICOLON;

"<>"|"!="|"=="|"<"|">"|">="|"<="	{ 

		if (!strcmp("!=", yytext)) yylval->i = DIFFERENT;
		else if (!strcmp("<>", yytext)) yylval->i = DIFFERENT;
		else if (!strcmp("==", yytext)) yylval->i = EQUAL;
		else if (!strcmp(">", yytext)) yylval->i = BIGGER;
		else if (!strcmp("<", yytext)) yylval->i = SMALLER;
		else if (!strcmp(">=", yytext)) yylval->i = EBIGGER;
		else if (!strcmp("<=", yytext)) yylval->i = ESMALLER;

	return mathforms_RELACJA;
};

"||"			 return mathforms_OR;
"&&"			 return mathforms_AND;
".."			 return mathforms_WYBOR;
"("			 return mathforms_L_BRACE;
")"			 return mathforms_R_BRACE;
"\""			 return mathforms_DBL_QUOT;
"="			 return '=';
"%"			 return '%';
[ \t\r]			;
[\n]			line_number++;
.			return mathforms_CHAR;			
 
%%
