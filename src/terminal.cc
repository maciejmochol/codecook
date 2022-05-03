/* $Id: terminal.cc,v 1.18 2000/11/09 21:59:08 code Exp $ */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "terminal.h"
#include "cstable.h"
#include "protocol.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

// 
// class Terminal
//

Terminal::Terminal(char *s) : Codecook_Thread(s, TERMINAL) {
	out_buffer = NULL;
	Start();
};
	

int Terminal::Write()
{
 	return printf("%s", out_buffer); 
}

int Terminal::Read()
{
	return scanf("%s", in_buffer); 
}

int Terminal::read_line( int deskr, char *s, int nbytes ) {
	int nleft, nread;
	nleft = nbytes;
	
	while((nread = read(deskr, s , 1)) > 0){
		nleft -= 1;
		s += 1;
		if( (s - 1)[0] == '\n'  )
			{
				return nbytes-nleft;
			}
	}	   
	if (nread < 0){
			return nread;
		}
	else
		{
			return nbytes-nleft;
		}
};

int Terminal::write_line( int deskr, char *s, int nbytes) {
	int nleft, nwritten;
	nleft = nbytes;
	
	while (nleft > 0) {
		nwritten = write(deskr, s , nleft);
		if (nwritten <= 0)
			return nwritten;
		nleft -=nwritten;
		s += nwritten;
	}
	return nbytes-nleft;
};                              

int Terminal::RunCommand() {
	
	if(out_buffer != NULL) {
		free(out_buffer); 
	}
	
	out_buffer = cstable->protocol->ParseCommand(in_buffer);
	//na koniec wyczysc buffery
	memset(in_buffer, 0, IN_BUFFER_SIZE);

	return TERM_OK;
}

void * Terminal::Handler() {
	
	Log("%s starting...", name);

	try {

		int status;
		out_buffer = strdup(__codecook_version__);		
		out_buffer = (char*) realloc(out_buffer, strlen(out_buffer)+5);
		strcat(out_buffer, "\n");
		Write();

		while (1) {
			status = Read();
			if (status == TERM_QUIT) break;
			status = RunCommand();
			Write();
			if (status == TERM_QUIT) break;
		}

		Close();
		Log("%s closing connection, exiting...", name);

	}
	catch (exc_Codecook &exc) {
		cout << "Terminal got exception !!!" << endl;
		cout << exc;
		return NULL;
	}

	return NULL;
};

void Terminal::Close()
{
}

Terminal::~Terminal(){
}

//
// class Terminal_Console  
//

Terminal_Console::Terminal_Console() : Terminal("Console Terminal")
{
	Log("Starting Console Terminal");
}

int Terminal_Console::Write()
{
	if(out_buffer == NULL) {
		out_buffer = "<UNKNOWN_COMMAND/>\n";
		//write(1, out_buffer, strlen(out_buffer) );
		Log(out_buffer);
		out_buffer = NULL;
	} else {		
		if(strlen(out_buffer))
			//write(1, out_buffer, strlen(out_buffer) );
			Log(out_buffer);
	}

return 1;
}

int Terminal_Console::Read()
{
	write(1, ">", 1);
	if(read_line(0, in_buffer, sizeof(in_buffer)) > 0){
		return TERM_OK;
	}
	else
		return TERM_QUIT;

}


//
// class Terminal_RunScript
//


Terminal_RunScript::Terminal_RunScript(char *pscript_file)
		: Terminal("Script Runner")
{
	script_file = strdup(pscript_file);
	file = fopen(script_file, "r");

	if(file)
		Log("Running %s", script_file);
	else
		Log("Cannot open %s", script_file);
}

Terminal_RunScript::~Terminal_RunScript()
{
	free(script_file);
	fclose(file);
}

int Terminal_RunScript::Write()
{

	if(out_buffer == NULL) {
		out_buffer = "<UNKNOWN_COMMAND/>\n";
		Log("Codecook: {%s%s", out_buffer, "}" );
		out_buffer = NULL;
	} else {		
		if(strlen(out_buffer))
			Log("Codecook: {%s%s", out_buffer, "}" );
		else
			Log("Codecook: (NONE)" );
	}

return 1;
}

int Terminal_RunScript::Read()
{

	if(read_line(fileno(file), in_buffer, sizeof(in_buffer)) > 0){
		in_buffer[strlen(in_buffer)-1] = 0;	
		Log("Script line: { %s %s" , in_buffer, " }");	
		return TERM_OK;
	}
	else
		return TERM_QUIT;
}

//
// class Terminal_TCPIP
//


Terminal_TCPIP::Terminal_TCPIP(int portnum)
	: Terminal(""), tcp_port(portnum)
{
 char b[100];

	snprintf(b, sizeof(b), "TCP_terminal(port = %d)", portnum);
	delete name;
	name = strdup(b);
}

int Terminal_TCPIP::Write()
{
int ret;

	if(out_buffer == NULL) {
		out_buffer = "<UNKNOWN_COMMAND/>\n";
		ret = write(tcp_port, out_buffer, strlen(out_buffer) );
		out_buffer = NULL;
	} else {		
		ret = write(tcp_port, "<CODECOOK_RESPONSE>\n", sizeof("<CODECOOK_RESPONSE>\n"));
		ret += write(tcp_port, out_buffer, strlen(out_buffer) ); 
		ret += write(tcp_port, "</CODECOOK_RESPONSE>\n", sizeof("</CODECOOK_RESPONSE>\n"));
	}

return ret;
}

int Terminal_TCPIP::Read() {

	if(read_line(tcp_port, in_buffer, sizeof(in_buffer)) > 0)
		return TERM_OK;
	else
		return TERM_QUIT;

}

void Terminal_TCPIP::Close()
{
	shutdown(tcp_port, 2);
	close(tcp_port);
}

Terminal_TCPIP::~Terminal_TCPIP() {
	Close();
}


