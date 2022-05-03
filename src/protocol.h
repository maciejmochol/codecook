#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "blocking.h"
#include "datastruct.h"
#include "command.h"
//#include "thread.h"

class TreeNode {
protected:	
	TList<TreeNode> *subNodes;
	Token *acceptedToken; 
	ExecuteCommand *execute;
	char *definition; //NULL oznacza brak opisu
	//opisy umieszczamy tylko tam gdzie funkcje???
	// nie wiem trzeba poprobowac
	int TryParse(Command *);
public:
	TreeNode();
	TreeNode(Token *);
	TreeNode(Token *, ExecuteCommand *, const char *);
	Token *AcceptsToken();
	virtual ~TreeNode();
	void NaddCommand(Command *, ExecuteCommand *, const char *);
	int NdeleteCommand(Command *);
	char *NparseCommand(Command *);
	TreeNode *findNode(const Token *);
	TreeNode *findExactNode(const Token *);
	void addNode(TreeNode *);
	void insertNode(TreeNode *);
};

class Protocol : public TreeNode, Room {
public:
	Protocol();
	virtual ~Protocol();
	int AddCommand(const char *, ExecuteCommand *, const char *);
	int DeleteCommand(const char *);
	char *ParseCommand(const char *);
	char *PrintHelp(const char *);
};

class HelpCommand : public ExecuteCommand {
public:
	HelpCommand();
	~HelpCommand();
	virtual char * execute(Command *);
};

#endif
