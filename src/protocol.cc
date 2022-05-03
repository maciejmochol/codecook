#include "protocol.h"
#include "cstable.h"
#include "device_list.h"
#include "thread.h"
#include "parsing.h"
#include "user.h"
#include "xmlgen.h"

//
//	TreeNode
//

TreeNode::TreeNode() {
	subNodes = new TList<TreeNode>;
	acceptedToken = NULL;
	execute = NULL;
	definition = NULL;
};

TreeNode::TreeNode(Token *tok) {
	subNodes = new TList<TreeNode>;
	acceptedToken = tok;
	execute = NULL;
	definition = NULL;
};

TreeNode::TreeNode(Token *tok, ExecuteCommand *ex, const char *def) {
	subNodes = new TList<TreeNode>;
	acceptedToken = tok;
	execute = ex;
	definition = strdup(def);
};
	
Token *TreeNode::AcceptsToken() {
	return acceptedToken;
};

TreeNode::~TreeNode() {
	if( acceptedToken != NULL )
		delete acceptedToken;
	if( execute != NULL )
		delete execute;
	if( definition != NULL )
		delete definition;
	delete subNodes;
};

void TreeNode::NaddCommand(Command *command, ExecuteCommand *ex, const char *descr) {
	if( command->onTheEnd() ) {
		execute = ex;
		definition = strdup(descr);
	} else {
		Token *tok = command->nextToken();
		TreeNode *found = NULL;
		if ( (found = findExactNode(tok)) == NULL ) {
			found = new TreeNode(new Token(tok));
			if(tok->acceptsAll()) {	//token akceptuje wszystko
				addNode(found);		//dodaj go na koniec
			} else {
				insertNode(found);
			}
		}
		found->NaddCommand(command, ex, descr);
	}
};

int TreeNode::NdeleteCommand(Command *command) {
	if( !command->onTheEnd() ) {
		Token *tok = command->nextToken();
		TreeNode *found = NULL;
		found = findNode(tok);
		if(found->NdeleteCommand(command))
			subNodes->rmElt(found);
	}	
	return ( subNodes->getLength() == 0 );
};

int TreeNode::TryParse(Command *command) {
	//sprawdz czy drzewo zaakceptuje command

	if( command->onTheEnd() && execute != NULL)
		return 1;
	if( command->onTheEnd() && execute == NULL)
		return 0;
	TreeNode *found;
	Token *tok = command->nextToken();
	if((found = findExactNode(tok)) == NULL)
		return 0;
	else
		return found->TryParse(command);
};

char *TreeNode::NparseCommand(Command *command) {
	if( command->onTheEnd() && execute == NULL)
		return NULL;
	if( command->onTheEnd() && execute != NULL)  //?? moze ||
		return execute->execute(command);
	
	Token *tok = command->nextToken();
	TreeNode *found;
	if((found = findNode(tok)) == NULL)
		return NULL;
	else
		return found->NparseCommand(command);
};


TreeNode *TreeNode::findNode(const Token *tok) {
	TIterator<TreeNode> *iter = new TIterator<TreeNode>(subNodes);
	TreeNode *t;
	while(!iter->endReached()) {
		t = iter->next();
		if( *(t->AcceptsToken()) == tok ) {
			delete iter;
			return t;
		}
	}
	delete iter;
	return NULL;
};

TreeNode *TreeNode::findExactNode(const Token *tok) {
	TIterator<TreeNode> *iter = new TIterator<TreeNode>(subNodes);
	TreeNode *t;
	while(!iter->endReached()) {
		t = iter->next();
		if( *(t->AcceptsToken()) || tok ) {
			delete iter;
			return t;
		}
	}
	delete iter;
	return NULL;
};

void TreeNode::addNode(TreeNode *n) {
	subNodes->add(n);
};

void TreeNode::insertNode(TreeNode *n) {
	subNodes->insert(n);
};




//
//	Protocol
//

Protocol::Protocol() : TreeNode(), Room() {
	//tutaj jest minimalny zbior instrukcji akceptowanych przez
	//protokol:

//dodaj obsluge bledow: log(nie udalo sie dodac command...)
//najpierw porzadek z loggerem i managerem!!!

//popraw opisy

//trzymam tu wiele identycznych obiektow, dlatego ze bycmoze bede 
//chcial cos usunac z protokolu - i tak jest wygodniej,
//to sa male obiekty, jak by byly duze to trzeba byloby trzymac liczbe po 
//jednym i liczbe dowiazan do niego
	
	AddCommand("HELP", new HelpCommand(), "pupa opis 2");
};

Protocol::~Protocol() {
};


int Protocol::AddCommand(const char *command, ExecuteCommand *func, const char *descr) {
	Command *com = new Command(command);

	EnterWriter();
	if( TryParse(com) ) {
		LeaveWriter();
		delete com;
		return 0;
	}
	com->beginWithFirstToken();
	NaddCommand(com, func, descr);
	LeaveWriter();
	delete com;
	return 1;
};

int Protocol::DeleteCommand(const char *command) {
	Command *com = new Command(command);
	EnterWriter();
	if( !TryParse(com) ) {
		LeaveWriter();
		delete com;
		return 0;
	}
	com->beginWithFirstToken();
	NdeleteCommand(com);
	LeaveWriter();
	delete com;
	return 1;
};

char *Protocol::ParseCommand(const char *command) {
	char *ret = NULL;
	Command *com = new Command(command);
	EnterReader();
	ret = NparseCommand(com);
	LeaveReader();
	delete com;
	return ret;
};

char *Protocol::PrintHelp(const char *command) {
	DynamicBuffer buf;
	if(command == NULL) {
		buf << "<HELP>\n<HELP_DESCR><LINE>\nThis is main help,"
		<< " you can write these commands to get more help:\n"
		<< "</LINE><HELP_DESCR>\n"
		<< "<HELP_TOPIC><LINE>\nHELP COMMANDS\n</LINE></HELP_TOPIC>\n"
		<< "<HELP_TOPIC_DESCR><LINE>\nto get help on all commands\n"
		<< "</LINE></HELP_TOPIC_DESCR>\n"
		<< "<HELP_TOPIC><LINE>\nHELP [command]\n<LINE></HELP_TOPIC>\n"
		<< "<HELP_TOPIC_DESCR><LINE>\nwhere [command] is\n</LINE>\n<LIST>\n";
		
		EnterReader();
			TIterator<TreeNode> *iter = new TIterator<TreeNode>(subNodes);
			Token *t;
			while( !iter->endReached() ) {
				t = iter->next()->AcceptsToken();
				if(!t->acceptsAll() && !t->isHelpToken()) {
					buf << "<LIST_ELT><LINE>\n"
					<< t->string()
					<< "\n</LINE></LIST_ELT>\n";
				}
			}
		LeaveReader();
		
		buf << "</LIST>\n<LINE>\nto get help on particular command\n</LINE>"
		<< "</HELP_TOPIC_DESCR>\n"
		<< "</HELP>\n";
	} else {
	}
		
	return buf.toString();
};


//
//	HelpCommand
//

HelpCommand::HelpCommand() {
};

HelpCommand::~HelpCommand() {
};

char *HelpCommand::execute(Command *command) {
	string s = XMLgen::Command(command);

	s += cstable->protocol->PrintHelp(NULL);
	return copy_string(s);
};
