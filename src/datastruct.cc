/* $Id: datastruct.cc,v 1.5 2000/04/25 17:52:36 code Exp $ */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "datastruct.h"

//
// copy_string
//

char * copy_string(string &s) {

	char *c;

	//c = strdup(s.data());
	c = (char*) malloc(s.length()+1);
	memcpy(c, s.data(), s.length());
	c[s.length()] = 0;
	
	return c;
};


//
//	Token
//

Token::Token() {
	token = NULL;
};

Token::Token(Token *t) {
	if(t->token == NULL)
		token = NULL;
	else
		token = strdup(t->token);
};

Token::Token(const char *tok) {
	token = strdup(tok);
};

Token::~Token() {
	if (token != NULL )
		free(token);
};

int Token::operator==(const Token *t) {
	if( token == NULL || t->token == NULL) {
		return 1;
	}
	if( strcmp(token, t->token) == 0 ) {
		return 1;
	}
	else {
		return 0;
	}
};

int Token::operator||(const Token *t) {
//dokladne porownanie
	if( token == NULL && t->token == NULL) {
		return 1;
	}
	if( token == NULL || t->token == NULL) {
		return 0;
	}
	if( strcmp(token, t->token) == 0 ) {
		return 1;
	}
	else {
		return 0;
	}
};

char *Token::string() {
	return token;
};

char *Token::print() {
	if(token == NULL)
		return NULL;
	else
		return strdup(token);
};

int Token::acceptsAll() {
	return token == NULL;
};

int Token::isHelpToken() {
	if(token == NULL)
		return 0;
	else
		if(strcmp(token, "HELP") == 0)
			return 1;
		else
			return 0;
};

//
//	DynamicBuffer
//

DynamicBuffer::DynamicBuffer() {
	string = (char *) malloc(sizeof(char));
	string[0] = '\0';
	length = 1;
	line = 0;
};

DynamicBuffer::~DynamicBuffer() {
	if(string != NULL)
		free(string);
};

DynamicBuffer& DynamicBuffer::operator<<(char *s) {
	int pom = strlen(s);
	string = (char *) realloc(string, (length + pom) * sizeof(char));
	memcpy(string + (length - 1) * sizeof(char), s, pom * sizeof(char));
	string[length + pom - 1] = '\0';
	length += pom;
	return *this;
};

char *DynamicBuffer::toString() const {
	return strdup(string);
};
void DynamicBuffer::startLines() {
	line = 0;
};

char *DynamicBuffer::nextLine() {
	if(line + 1 >= length)
		return NULL;
	int pom = line;
	while(line < length && string[line] != '\n' && string[line] != '\0')
		line++;
	char *ret = (char *) malloc((line - pom + 1) * sizeof(char));
	memcpy(ret, string + pom, (line - pom) * sizeof(char));
	ret[line - pom] = 0;
	line++;
	return ret;
};

int DynamicBuffer::numberOfLines() {
	int ret = 0;
	for(int i = 0; i < length; i++) {
		if(string[i] == '\n')
			ret++;
	}
	return ret;
};