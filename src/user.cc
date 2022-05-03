							// by Vooyeck

#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cstable.h"
#include "protocol.h"
#include "logger.h"
#include "xmlgen.h"

//#define _XOPEN_SOURCE
//#include <unistd.h> - to powinno chodzic z tym, ale sie jebie
//dlatego deklaruje recznie :
extern "C"
char *crypt(const char*, const char*);

// Jak to kurna zrobic, zeby mozna bylo uzyc i crypt() i strdup()?
// Jednak nienawidze Linuksa..

#include "codecook.h"
#include "user.h"
#include "datastruct.h"


User::User(char *pname, char *plevel, char *ppasswd, char *pdescr){

	name = strdup(pname);

	if(plevel) 
		level = strdup(plevel);
	else 
		level = strdup("lamer");

	if(ppasswd)
		passwd = strdup(ppasswd);
	else
		passwd = NULL;

	if(pdescr)
		descr = strdup(pdescr);
	else
		descr = NULL;
}
	
void User::SetLevel(char *plevel){

EnterWriter();
	free(level);
	level = strdup(plevel);
LeaveWriter();
}	

void User::SetCryptedPasswd(char *pcrypted_passwd){

EnterWriter();
	free(passwd);
	passwd = strdup(pcrypted_passwd);
LeaveWriter();
}

void User::SetPasswd(char *ppasswd){

EnterWriter();
	free(passwd);
	passwd = strdup(crypt(ppasswd, "jo")) ;
LeaveWriter();
}
	
void User::SetDescr(char *pdescr){

EnterWriter();
	free(descr);
	descr = strdup(pdescr);
LeaveWriter();
}

char *User::GetName(void){

	EnterReader();
		char *localName = name;
	LeaveReader();

return localName;
}

char *User::GetDescr(void){

	EnterReader();
		char *localDescr = descr;
	LeaveReader();

return localDescr;
}

char *User::GetPasswd(void){

	EnterReader();
		char *localPasswd = passwd;
	LeaveReader();

return localPasswd;
}

char *User::GetLevel(void){

	EnterReader();
		char *localLevel = level;
	LeaveReader();

return localLevel;
}
	
int User::CheckPasswd(char *ppasswd){ 

	EnterReader();
		char *localPasswd = passwd;
	LeaveReader();

return !strcmp(	crypt(ppasswd, "jo"), localPasswd ); 
}
			
int User::CheckCryptedPasswd(char *crypted_passwd){

	EnterReader();
		char *localPasswd = passwd;
	LeaveReader();

return !strcmp( crypted_passwd, localPasswd );
}

void User::print(FILE *file){

	fprintf(file, "USER(%s) {\n", name);

	fprintf(file, "\tuser_level = %s;\n", level);

	if(passwd)
		fprintf(file, "\tuser_passwd = %s;\n", passwd);
	if(descr)
		fprintf(file, "\tuser_descr = %s;\n", descr);
	
	fprintf(file, "}\n");
}

// Uwaga - te funkcje nalezy poprawic(ulepszyc - chodzi o wielowatkowosc), 
// ale teraz nie chcialo mi sie.. 

UserList::UserList(){
	list = new TListRoom<User>;	

	cstable->protocol->AddCommand("USER ADD <??????>", new UserAddCommand(), "");
	cstable->protocol->AddCommand("USER DEL <??????>", new UserDeleteCommand(), "");
	cstable->protocol->AddCommand("USER LIST", new AdminListUsersCommand(),"");
	cstable->protocol->AddCommand("USER SETPASS <??????> <??????>", 
		new UserSetPassCommand(),"");
	cstable->protocol->AddCommand("USER LOGIN <??????> <??????>", 
		new UserLoginCommand(),"USER LOGIN <name> <passwd>");
	cstable->protocol->AddCommand("USER SETDESCR <??????> <??????>", 
		new UserSetDescrCommand(), "USER SETDESCR <name> <descr>");

}

void UserList::AddUser(User *user){
	list->add(user);
}

void UserList::AddUser(char *name){
	list->add(new User(name));	
}

void UserList::DelUser(User *user){
	list->del(user);
}

void UserList::DelUser(char *name){

User *tmpUser;

	TIterator<User> *i = new TIterator<User>(list);

	while(!(i->endReached())) {
		tmpUser = i->next();
		if( !strcmp(tmpUser->GetName(), name) ){
			list->del(tmpUser);
			delete i;
			return;
		}
	}

delete i;
}

User *UserList::GetUser(char *name){

User *tmpUser;

	TIterator<User> *i = new TIterator<User>(list);

	while(!(i->endReached())) {
		tmpUser = i->next();
		if( !strcmp(tmpUser->GetName(), name) ){
			delete i;
			return tmpUser;
		}
	}

delete i;
return NULL;
}

int UserList::IsUser(char *name) {

	return GetUser(name) ? 1 : 0;
}

char *UserList::GetUserNames(int descr){

	string s = "", l = "";
	User *tmpUser;

	TIterator<User> *i = new TIterator<User>(list);

	while(!(i->endReached())) {
		tmpUser = i->next();
		string dscr = "";
		if(descr){
			char *d = tmpUser->GetDescr();
			if ( d )
				dscr += tmpUser->GetDescr();
		}
		l += XMLgen::User(tmpUser->GetName(), dscr);
	}
	delete i;
	s += XMLgen::Users(l);
	return copy_string(s);
}

void UserList::save(FILE *file){

User *tmpUser;

	fprintf(file, "\n# Users:\n\n");

	TIterator<User> *i = new TIterator<User>(list);

	while(!(i->endReached())) {
		tmpUser = i->next();
		tmpUser->print(file);
	}

	fprintf(file, "\n# End of Users:\n");

delete i;
}

void UserList::SetUserPass(char *name, char *passwd)
{
	User *tmpUser = GetUser(name);

	if( !tmpUser )
		return;

	tmpUser->SetPasswd(passwd);
}

void UserList::SetUserLevel(char *name, char *level)
{
	User *tmpUser = GetUser(name);

	if( !tmpUser )
		return;

	tmpUser->SetLevel(level);
}

void UserList::SetUserDescr(char *name, char *description)
{
	User *tmpUser = GetUser(name);

	if( !tmpUser )
		return;

	tmpUser->SetDescr(description);
}

User *UserList::Login(char *name, char *passwd)
{
	User *tmpUser = GetUser(name);

	if( !tmpUser )
		return NULL;

	if( tmpUser->CheckPasswd(passwd) )
		return tmpUser;

return NULL;
}

// Komendy

// Login

char *UserLoginCommand::execute(Command *command){

	Token *nameTok = (*command)[2];
	Token *passTok = (*command)[3];

	string s = XMLgen::Command(command);

	cstable->logger->Log("login before");

	User *tmpUser = cstable->userList->
		Login(nameTok->string(), passTok->string());

	cstable->logger->Log("login after");

	if(tmpUser) 
		s += XMLgen::Message("User logged in(?)");
	else
		s += XMLgen::Message("Cannot log in");
	return copy_string(s);
}

// SetPass

char *UserSetPassCommand::execute(Command *command){

	Token *nameTok = (*command)[2];
	Token *passTok = (*command)[3];
	string s = XMLgen::Command(command);

	cstable->userList->SetUserPass(nameTok->string(), passTok->string());
	s += XMLgen::Message("Password set");
	return copy_string(s);
}

// Add

char *UserAddCommand::execute(Command *command){
	Token *tok = (*command)[2];
	string s = XMLgen::Command(command);
	cstable->userList->AddUser(tok->string());
	s += XMLgen::Message("User added");
	return copy_string(s);
}

// Delete

char *UserDeleteCommand::execute(Command *command){
	Token *tok = (*command)[2];
	string s = XMLgen::Command(command);
	cstable->userList->DelUser(tok->string());
	s += XMLgen::Message("User deleted");
	return copy_string(s);
}

// SetDescr

char *UserSetDescrCommand::execute(Command *command){

	Token *nameTok = (*command)[2];
	Token *descrTok = (*command)[3];
	string s = XMLgen::Command(command);
	cstable->userList->SetUserDescr(nameTok->string(), descrTok->string());
	s += XMLgen::Message("Description set");
	return copy_string(s);
}

// ListUsers

char *AdminListUsersCommand::execute(Command *command){
	string s = XMLgen::Command(command);
	if (!(cstable->userList))
	    s += XMLgen::Message("NO USERS");
	else
	    s+= cstable->userList->GetUserNames();
	return copy_string(s);
}
