%{
/* $Id: scanner_cdtree.l,v 1.4 2000/04/25 17:52:36 code Exp $ */
/* 
 * CD_Tree && Math scanner, Vooyeck '99-12
 */


#define DONT_INCLUDE_FLEXLEXER_H
#include "parser_cdtree.h"
#include <string.h>
#include <iostream>


/////////////////////// HOW TO DEBUG FLEX++ CODE ?? /////////////////////////

//void set_debug(int);
//extern int yy_flex_debug ;

int line_number = 1;
#define YY_NO_UNPUT // cztoby uniknac Warningoow (-Wall)

int cdtree_lex(YYSTYPE* yylval, void*pPS) {

//	set_debug(1);
// 	yy_flex_debug  = 1;

	int ret = ((cdtree_ParserStruct*)pPS)->lexer->yylex();
	*yylval = (YYSTYPE)*((cdtree_ParserStruct*)pPS)->lexer->GetYYlval();	

return ret;	
}

// TEXTCHAR		[a-zA-Z0-9_(),. ]
%}

%option NOYYWRAP
%option c++
%option stack 
%option yyclass="cdtree_LexerClass"
%option prefix="cdtree_"
%option debug

LETTER			[a-zA-Z_]
LLETTER			[a-z_]
HLETTER			[A-Z_]
PATHCHAR		[a-zA-Z0-9_]
CRYPTEDCHAR		[a-zA-Z0-9./]
INETADDRCHAR		[a-zA-Z0-9.]
CYPHER			[0-9]
DIFFCHAR		[!@#$%^&*()_+{}:"~<>,./;[]-=\\|]
CHAR			[a-z0-9!@#$%^&()_{}:"~<>,.\[\]=-\\|+*/\\;]
TEXTCHAR		[a-zA-Z0-9'!@#$%^&() _{}:~<>,.\[\]=-\\|+*/\\;]
SP			[ \t]
NOCOMMENT		[^"]
%x INSIDE_NODE INSIDE_STRING
%%

<INITIAL,INSIDE_NODE>#.*\n	line_number++;  /* komentarz */
<INITIAL>({LETTER}{INETADDRCHAR}*"/")*{LETTER}{PATHCHAR}*(:{PATHCHAR}+)*{SP}*(","{SP}*{PATHCHAR}+)*  {
			((YYSTYPE*)yylval)->mstring = strdup(yytext); 
//			cout << "i'm outside node!\n"; 
			return cdtree_ADDRESS;
			}
<INSIDE_NODE>{LETTER}{PATHCHAR}* 	{
			((YYSTYPE*)yylval)->mstring = strdup(yytext); 
//			cout << "i'm inside node!\n"; 
			return cdtree_IDENTIFIER;
			}
<INITIAL,INSIDE_NODE>";"		return cdtree_SEMICOLON;
<INITIAL,INSIDE_NODE>"{"		{ 
					BEGIN(INSIDE_NODE); 
					return cdtree_L_TBRACKET;
					}
<INITIAL,INSIDE_NODE>"}"		{
					BEGIN(0); 
					return cdtree_R_TBRACKET;
					}
<INITIAL,INSIDE_NODE>"="		return '=';
<INITIAL,INSIDE_NODE>"\""	{ pushState(INSIDE_STRING); }
<INSIDE_STRING>{TEXTCHAR}*\"	{ 	popState();
					yytext[strlen(yytext)-1] = 0;
					((YYSTYPE*)yylval)->mstring = 
						strdup(yytext); 
					return cdtree_STRING;
				}
<INITIAL,INSIDE_NODE>[ \t\r]		;
<INITIAL,INSIDE_NODE>[\n]		line_number++;
<INITIAL,INSIDE_NODE>.			cout << yytext << endl; return -1;			
%%
