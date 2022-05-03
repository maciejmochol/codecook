/* $Id: parser_cdtree.cc,v 1.6 2000/04/25 17:52:36 code Exp $ */

#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <strstream>


#include "parser_cdtree.h"
#include "alarmserv.h"

//#define YYDEBUG 1
extern int cdtree_debug ;

//
// Class Variable
//

Variable::Variable(char*pname, char*pvalue) { 
	name = strdup(pname); 
	value = strdup(pvalue); 
} 
Variable::Variable() { 
	name = value = NULL; 
}
Variable::~Variable() { 
	free(name); 
	free(value); 
}

//
// Class VariableList
//

char *VariableList::getValue( char* name ) const 
{
		TIterator<Variable> *i = new TIterator<Variable>(lista);
		Variable *tmpDef;

		while(!(i->endReached())) {
			tmpDef = i->next();
			if( !strcmp(tmpDef->getName(), name) ){
				delete i;
				return tmpDef->getValue();
			}
		}
delete i;
return NULL;
}

void VariableList::AddStandardChannels(CD_Node *node)
// Doesn't add TYPE !
{
char *lChannel;

//	if( (lChannel = getValue("NAME")) != NULL ) 
//		node->AddChannel("NAME", lChannel);
	if( (lChannel = getValue("UNIT")) != NULL ) 
		node->AddChannel("UNIT", lChannel );
	if( (lChannel = getValue("MIN")) != NULL ) 
		node->AddChannel("MIN", lChannel);
	if( (lChannel = getValue("MAX")) != NULL ) 
		node->AddChannel("MAX", lChannel);
	if( (lChannel = getValue("SYMBOL")) != NULL ) 
		node->AddChannel("SYMBOL", lChannel);

		if( (lChannel = getValue("DATA_TYPE")) != NULL ) 
			node->AddChannel("DATA_TYPE", lChannel);
		else 	
			node->AddChannel("DATA_TYPE", "DOUBLE");

		if( (lChannel = getValue("PRECISION")) != NULL ) 
			node->AddChannel("PRECISION", lChannel);
		else 	
			node->AddChannel("PRECISION", "2");
//	
//	if( (lChannel = getValue("VALUE")) != NULL ) 
//		node->AddChannel("VALUE", lChannel);


}

//
// methods for creating correct CD_Nodes
//

void VariableList::CreateNode(CD_Tree *tree, CD_Address *addr) {


	char *lNodeType = getValue("TYPE");

		if( !lNodeType )
			return; // Olej wezly bez typu 

		else if( !strcmp(lNodeType, "ALIAS") )
			CreateAlias(tree, addr);

		else if( !strcmp(lNodeType, "EMPTY") )
			CreateEmptyNode(tree, addr);

		else if( !strcmp(lNodeType, "ALARM") )
			CreateAlarmNode(tree, addr);

		else if( !strcmp(lNodeType, "FORMULA") )
			CreateFormulaNode(tree, addr);

			// na razie.. a moze i na stale..
		else if( !strcmp(lNodeType, "CONSTANT") )
			CreateConstantNode(tree, addr);

		else if( !strcmp(lNodeType, "RAPORT") )
			CreateRaport(tree, addr);

		//
		// Tutaj dodaj pozostale wezly
		// alarmy, raporty, itd.
		//

		else 
			return; // Olej wezly nieznanego typu

}

void VariableList::CreateAlias(CD_Tree *tree, CD_Address *addr)
{
	if (!tree || !addr) return;

		// cel
	char *target = getValue("TARGET");
	if (!target) return; // Alias bez celu

	CD_Node_Alias *alias = new CD_Node_Alias(*tree, target);
	// AddStandardChannels(alias);

	try {
		tree->InsertNode(*addr, alias);
	}
	catch (exc_CD_Tree_CannotInsertAlias) {
		// Nie mozna dodac aliasa
	}

}

void VariableList::CreateEmptyNode(CD_Tree *tree, CD_Address *addr)
{
	if (!tree || !addr) return;

	CD_Node* lNode = new CD_Node_Empty();

	tree->InsertNode(*addr, lNode);
}

void VariableList::CreateAlarmNode(CD_Tree *tree, CD_Address *addr)
{
	if (!tree || !addr) return;

	CD_Node* lNode = new CD_Node_Alarm();
	AddStandardChannels(lNode);

	char *target = getValue("FORMULA");
	if(target)
		lNode->AddChannel(string("FORMULA"), string(target));

	tree->InsertNode(*addr, lNode);
}

void VariableList::CreateFormulaNode(CD_Tree *tree, CD_Address *addr)
{
	if (!tree || !addr) return;

	CD_Node* lNode = new CD_Node_Formula();
	AddStandardChannels(lNode);

	char *target = getValue("FORMULA");
	if(target)
		lNode->AddChannel(string("FORMULA"), string(target));

	tree->InsertNode(*addr, lNode);
}

void VariableList::CreateConstantNode(CD_Tree *tree, CD_Address *addr)
{
	if (!tree || !addr) return;

	CD_Node* lNode = new CD_Node_Constant();
	AddStandardChannels(lNode);

	char *target = getValue("VALUE");
	if(target)
		lNode->AddChannel(string("VALUE"), string(target));

	tree->InsertNode(*addr, lNode);
}

void VariableList::CreateRaport(CD_Tree *tree, CD_Address *addr)
{
	if (!tree || !addr) return;

//	char *name = getValue("NAME");
//	if (!name) return; // Raport bez nazwy

	CD_Node* lNode = new CD_Node_Raport();
	AddStandardChannels(lNode);

//	lNode->SetChannel("NAME", name);

	tree->InsertNode(*addr, lNode);

}


//
// Klasa CD_Parser
//

void *CD_Parser::Parse(istream &in, ostream &out) {  

	if (!TreePtr) return NULL;

#ifdef YYDEBUG
	cdtree_debug = 1;
#endif

	cdtree_ParserStruct lPS(TreePtr, 
		new cdtree_LexerClass(&in, &out));
				// dzieki temu kazde wywolanie jest reentrant
				// yyparse() ma tylko lokalne zmienne
				// a yylex() jest moja funkcja, ktora
				// wywoluje metody klasy pochodnej FlexLexer();
	int ret = !cdtree_parse((void*)&lPS);
	delete lPS.lexer;	

	return (void*) ret;
};
