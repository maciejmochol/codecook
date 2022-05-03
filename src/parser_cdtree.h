/* $Id: parser_cdtree.h,v 1.4 2000/04/15 08:21:56 vooyeck Exp $ */


#ifndef PARSER_CDTREE_H
#define PARSER_CDTREE_H

#include "parsing.h"

#include <string>
#include "datastruct.h"
#include "cdtree.h"
//#include "grammar_cdtree.tab.h"

//
// Note for developers (code, 01.02.2000)
//
// In class VariableList there are methods
// CreateNode, Create* for adding new static
// CD_Tree nodes to the parser
//


//
// klasa CD_Parser
//


class CD_Parser : public Parser {
	CD_Tree *TreePtr;
public:
	CD_Parser(CD_Tree *tree, char *name=NULL) : 
		Parser(name), TreePtr(tree) { }; 
	CD_Parser(CD_Tree *tree, string name) : 
		Parser(name), TreePtr(tree) { };

	void *Parse(istream &in, ostream &out);
};


//
// definicje do parsera
//

//#ifdef YYSTYPE
//	#undef YYSTYPE
//#endif	
#ifndef DONT_INCLUDE_FLEXLEXER_H 
	#undef yyFlexLexer 
	#define yyFlexLexer cdtree_FlexLexer 
	#include "FlexLexer.h" 
#endif

class Variable;
class VariableList;

/*
	typedef union {
		int i;
		float f;

		char *mstring;
		Variable *var;
		VariableList *varList;
	} universal_YYSTYPE;

	#define YYSTYPE universal_YYSTYPE	
*/

class cdtree_LexerClass;
int cdtree_parse(void*);
int cdtree_lex(YYSTYPE*, void*);
int cdtree_error(char*); 


struct cdtree_ParserStruct {
	CD_Tree *tree;
	cdtree_LexerClass *lexer;
	// .. more (if needed)

	cdtree_ParserStruct(CD_Tree *pTree=NULL, cdtree_LexerClass *pLexer=NULL)
	{
		tree=pTree; lexer=pLexer; // ..
	}
};

class cdtree_LexerClass : public cdtree_FlexLexer {

	YYSTYPE *yylval; 
	int line_number;

public:
	// Nie bedziemy obslugiwac plikow przekierowanych na stdin - 
	// bo wtedy nasze REENTRANTNESS bierze w leb. Stad konstruktor 
	// MUSI miec parametry.

	cdtree_LexerClass(istream* pYYIn, ostream* pYYOut) : 
		cdtree_FlexLexer(pYYIn, pYYOut) { 
			yylval = new YYSTYPE;
			line_number=0;
			}	
	~cdtree_LexerClass()  { delete yylval; }

	YYSTYPE *GetYYlval() { return yylval; }
	virtual int yylex();
	
	void popState(){ yy_pop_state(); }
	void pushState(int state){ yy_push_state(state); }
	int  GetLineNumber() { return line_number; }
};

////////////////////////////////////////////////////////////////////////////
/// Used to temporaily store Node's variables

class Variable
{
	char *name;
	char *value;

public:
	Variable(char*pname, char*pvalue); 
	Variable();
	~Variable();

	char *getName() const { return name; }
	char *getValue() const { return value; }
};

class VariableList
{
	TList<Variable> *lista;

public:
	VariableList(){ lista = new TList<Variable>(); }
	~VariableList(){ 
		while( lista->getLength() )
			delete lista->getEltInfo(0);
	}

	void  Add(Variable *i) { lista->add(i);}
	void  Insert(Variable *i) { lista->insert(i);}

	char *getValue( char* name ) const;
	
	void CreateNode(CD_Tree *tree, CD_Address *addr);
	void AddStandardChannels(CD_Node *node);
	// methods for create correct CD_Nodes
	void CreateEmptyNode(CD_Tree *tree, CD_Address *addr);
	void CreateAlarmNode(CD_Tree *tree, CD_Address *addr);
	void CreateFormulaNode(CD_Tree *tree, CD_Address *addr);
	void CreateConstantNode(CD_Tree *tree, CD_Address *addr);
	void CreateAlias(CD_Tree *tree, CD_Address *addr);
//	void CreateAlarm(CD_Tree *tree, CD_Address *addr);
	void CreateRaport(CD_Tree *tree, CD_Address *addr);
};

#endif
