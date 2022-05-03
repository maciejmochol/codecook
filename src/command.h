// $Id: command.h,v 1.5 2000/11/05 21:20:55 code Exp $

#ifndef __COMMAND_H__
#define __COMMAND_H__

#ifndef _REENTRANT
#define _REENTRANT
#endif

#define _ALL_STRINGS_ "<??????>"

#include "datastruct.h"

class Command {
	TList<Token> *tokens;
	TIterator<Token> *iterator;
public:
	Command(const char *);
	~Command();
	Token* operator[](int);
	Token *nextToken();
	int onTheEnd();
	void beginWithFirstToken();
	int countOfTokens() const;
	char *toString() const;
	char *toStringBeginWith(int ) const;
};

class ExecuteCommand {
public:
	ExecuteCommand() { };
	virtual ~ExecuteCommand() { };
	virtual char * execute(Command *) = 0;
};


#endif
