/* $Id: parsing.h,v 1.7 2000/04/25 17:52:36 code Exp $ */


#ifndef PARSING_H
#define PARSING_H


#include <string>
#include <iostream>
#include <strstream>
#include <fstream>
#include "datastruct.h"

//////////////////////////////////////////////////////////////////////////
/// Versatile Parser class

	class Variable;
	class VariableList;
	class CValue;

		typedef union {

			/* types for parser cdtree */
			int i;
			char *mstring;
			Variable *var;
			VariableList *varList;


			/* types for parser mathforms */
			double d;
			char c;
			float f;
			class CValue *value;
			char* str;
			/* string for use in mathforms scanner */
			char *string;
		} universal_YYSTYPE;
	
		#define YYSTYPE universal_YYSTYPE

#include "grammar_cdtree.tab.h"
#include "grammar_mathforms.tab.h"


enum ParserState { ps_Undefined, ps_ParseError, ps_ParsedOK,
		ps_NoFileSetError, pa_CannotOpenError };

class Parser {

protected:
	ParserState p_state;  
	int	error_line;
	string file_name, lString;
	
public:
	Parser(char *name=NULL) { 
		if(name) 
			file_name = name; 
		else
			file_name = ""; 

		p_state = ps_Undefined;
	}
	Parser(string name) { file_name = name; p_state = ps_Undefined; };
	virtual ~Parser() { };

	ParserState GetState(){ return p_state; }
	int GetErrorLine(){ return error_line; }

	void SetConfigFile(char *name){ file_name = name; }
	string GetConfigFile(){ return file_name ; }

	virtual void *ParseString(char *mstring) {
			istrstream in(mstring);
			ostrstream out;

		return Parse(in, out);
	}

	virtual void *ParseString(string pString) { 

		char *tmpC = copy_string(pString); 

			istrstream in(tmpC);
			ostrstream out;

		void *ptr = Parse(in, out);
	
		free(tmpC);

	return ptr;
	}	

	void *ParseFile() {
		char *tmpC = copy_string(file_name); 

		ifstream lFileStream(tmpC);	
		ostrstream lScannerOutput;

		if (!lFileStream.is_open()){
		//	cstable->logger->Log("Cannot open file");
		// POWINNO BYC LOG
			cout << "Cannot open file" << file_name << endl;
			return 0;
		}

		void *ptr = Parse(lFileStream, lScannerOutput);			

		free(tmpC);

	return ptr;	
	}
	
	virtual void *Parse(istream &in, ostream &out) = 0;
	// what is the return value? it depends on what we parse
};


#endif
