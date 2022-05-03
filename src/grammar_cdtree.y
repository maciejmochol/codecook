%{
/* $Id: grammar_cdtree.y,v 1.4 2000/04/15 08:21:56 vooyeck Exp $ */
/* 
 * CD_Tree parser, Vooyeck '99-12
 */


#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include "cdtree.h"

#include "parser_cdtree.h"

#define YYPARSE_PARAM PStruct
#define YYLEX_PARAM PStruct
#define YYDEBUG 1 // when debugging
%}

%token cdtree_ADDRESS
%token cdtree_IDENTIFIER
%token cdtree_STRING
%token cdtree_SEMICOLON
%token cdtree_R_TBRACKET
%token cdtree_L_TBRACKET

%nonassoc cdtree_R_TBRACKET
%nonassoc cdtree_L_TBRACKET

%type <mstring>	cdtree_ADDRESS
%type <mstring> cdtree_IDENTIFIER
%type <mstring> cdtree_STRING
%type <i>	cdtree_SEMICOLON
%type <var> 	variable	
%type <varList>	node_variables

%pure_parser

/* Grammar follows */

%%

program:  /* empty */ { cout << "Tree is EMPTY!\n"; }
        | tree 
;

tree: 	node {}	
	| tree node { }
;

node:
	cdtree_ADDRESS cdtree_L_TBRACKET node_variables cdtree_R_TBRACKET {
		CD_Address *lAddress = new CD_Address($1);

		$3->CreateNode(((cdtree_ParserStruct*)PStruct)->tree, lAddress);

		delete lAddress;
		delete $3; // get rid of temporary variable class
		}
	| cdtree_ADDRESS cdtree_L_TBRACKET cdtree_R_TBRACKET {
		CD_Address *lAddress = new CD_Address($1);

		((cdtree_ParserStruct*)PStruct)->tree->InsertNode(*lAddress, new CD_Node_Empty());
		delete lAddress;
		}
;

node_variables: 
	variable { $$ = new VariableList(); $$->Add($1); }
	| node_variables variable { $$->Add($2); }
;

variable:
	cdtree_IDENTIFIER '=' cdtree_IDENTIFIER cdtree_SEMICOLON { 
				$$ = new Variable($1, $3); 
			} 
	| cdtree_IDENTIFIER '=' cdtree_STRING cdtree_SEMICOLON { 
				$$ = new Variable($1, $3); 
			}
;

/* End of grammar */
%%
/*
main(){
CD_Tree *cdtree = new CD_Tree();
#ifdef YYDEBUG 
	yydebug = 1;
#endif

	yyparse( cdtree );

}*/

int cdtree_error(char *){
	 return -1;	
}
