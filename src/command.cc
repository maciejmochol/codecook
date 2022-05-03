#include "command.h"

#include <stdlib.h>
#include <string.h>

//
//	Command
//

Command::Command(const char *command) {
	//ok tu moznaby uzyc strtok - ale w manualu do tej funkcji 
	//napisano zeby jej nigdy nie uzywac
	tokens = new TList<Token>;
	char *copy;
	int max = strlen(command) + 1;
	copy = strdup(command);
	int j = 0;
	for(int i = 0; i < max; i++) {
		while( i < max && (command[i] == ' ' || command[i] == '\t' 
				|| command[i] == '\n' || command[i] == '\r'))
			i++;
		j = i;
		if( i < max && command[i] == '"' ) {
			j++;
			i++;	
			while( i < max && command[i] != '\n' && command[i] != '\r'
					&& command[i] != '\0' && command[i] != '"') {
			printf("HHH:%c:\n", command[i]);
				i++;
			}
			if( command[i] != '"' ) {
				//byl blad nie dostalem zamykajacego cudzyslowu
				free(copy);	
				tokens->destroy();
				tokens = new TList<Token>;
				iterator = new TIterator<Token>(tokens);
				return;
			}	
		} else {
			while( i < max && command[i] != ' ' && command[i] != '\t'
				         && command[i] != '\n' && command[i] != '\r'
					 && command[i] != '\0')
				i++;
		}
		if( j != i ) {
			copy[i] = '\0';
			if( strcmp(copy + j, _ALL_STRINGS_ ) == 0 )
				tokens->add(new Token());
			else
				tokens->add(new Token(copy + j));	
		}
		j = i + 1;
	}
	free(copy);	
	iterator = new TIterator<Token>(tokens);
};

Command::~Command() {
	delete iterator;
	tokens->destroy();
};

Token *Command::operator[](int number) {
	if( number < 0 || number >= tokens->getLength())
		return NULL;
	TIterator<Token> *iter = new TIterator<Token>(tokens);
	Token *tok;
	for(int i = 0; i <= number; i++)
		tok = iter->next();
	delete iter;
	return tok;
};

Token *Command::nextToken() {
	return iterator->next();		
};

int Command::onTheEnd() {
	return iterator->endReached();
};

void Command::beginWithFirstToken() {
	iterator->goToStart();
};

int Command::countOfTokens() const {
	return tokens->getLength();
};

char *Command::toString() const {
	TIterator<Token> *iter = new TIterator<Token>(tokens);
	Token *tok;
	char *ret = (char *) malloc(1);
	ret[0] = 0;
	char *pom;
	while(!iter->endReached()) {
		tok = iter->next();
		pom = tok->string();
		if(pom == NULL) {
			ret = (char *) realloc(ret, strlen(_ALL_STRINGS_ + 1));
			ret = strcat(ret, _ALL_STRINGS_);
			ret = strcat(ret, " ");
		} else {
			ret = (char *) realloc(ret, strlen(pom + 1));
			ret = strcat(ret, pom);
			ret = strcat(ret, " ");
		}		
	}
	delete iter;
	return ret;
};

char *Command::toStringBeginWith(int beg) const {
	TIterator<Token> *iter = new TIterator<Token>(tokens);
	Token *tok;
	char *ret = (char *) malloc(1);
	ret[0] = 0;
	char *pom;
	int i = 0;
	while(!iter->endReached() && i < beg) {
		tok = iter->next();
		i++;
	}

	while(!iter->endReached()) {
		tok = iter->next();
		pom = tok->string();
		if(pom == NULL) {
			ret = (char *) realloc(ret, strlen(_ALL_STRINGS_ + 1));
			ret = strcat(ret, _ALL_STRINGS_);
			ret = strcat(ret, " ");
		} else {
			ret = (char *) realloc(ret, strlen(pom + 1));
			ret = strcat(ret, pom);
			ret = strcat(ret, " ");
		}		
	}
	delete iter;
	return ret;
};

