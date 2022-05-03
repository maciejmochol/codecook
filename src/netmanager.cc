/* $Id: netmanager.cc,v 1.9 2000/04/15 08:21:56 vooyeck Exp $ */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include <stdio.h>

#include "netmanager.h"
#include "terminal.h"
#include "codecook.h"
#include "cstable.h"

//#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>
/* #include <netdb.h> */
#include <errno.h>

#define PORT 2930
#define TRAN_SIZE 8192


// 
// class NetManager
//

NetManager::NetManager() : Codecook_Thread("NetManager", NETMANAGER) {
	Start();
};


void * NetManager::Handler() {

	Log("Starting NetManager...");

	//u_long adres;
	struct sockaddr_in sin;
	struct sockaddr_in cin;
	struct sockaddr_in sname;
	unsigned int cinlen, snamelen; //, ngniazdo;
	int on = 1, socknum, nsocknum;

	socknum = socket(AF_INET, SOCK_STREAM, 0);
	//to nalezy jakos lepiej obsluzyc - zastanow sie jak!!!
	if (socknum == -1) Fatal("NetManager: socket() error");

	if (setsockopt(socknum, SOL_SOCKET, SO_REUSEADDR,
          &on, sizeof(on)) < 0) { 
		close(socknum); Fatal("NetManager: setsockopt() error");
	}//to tez jakos lepiej obsluz
                                                                
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(PORT);

	if (bind(socknum, (struct sockaddr *)&sin, sizeof(sin)))
		Fatal("NetManager: bind() error"); 
	//obsluz lepiej - jak ten blad wyskoczy to bedzie si mozna tu 
	//telnetowac i wszystko bedzie znikalo w czarnej dziurze
	//albo probuj bindowac jeszcze kilka razy albo zamknij socket
	
 snamelen = sizeof(sname);
 if (getsockname(socknum, (struct sockaddr *)&sname, &snamelen))
   Fatal("NetManager: getsockname() error");  
 
 switch(sname.sin_family) {
	case AF_INET:
		Log("NetManager: starting TCP/IP server, port = %d", 
    			(int) ntohs(((struct sockaddr_in *)&sname)->sin_port));
   		break;

  	default:
		Fatal("NetManager: unknown family");
 }

 if (listen(socknum, 1))
  Fatal("NetManager: listen() error");

	Terminal *terminal;

 	while (1)
    		{
     			nsocknum = accept(socknum, (struct sockaddr *)&cin, &cinlen);
     			if (nsocknum == -1) Fatal("NetManager: accept() error");

        		Log("NetManager: connection");
			terminal = new Terminal_TCPIP(nsocknum);
    		}		

  	close(socknum);	

	return NULL;
}
