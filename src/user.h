/* $Id: user.h,v 1.3 2000/04/25 17:52:36 code Exp $ */
/* Just 'User'. 
 *	 					by Vooyeck
 */

#ifndef USER_H
#define USER_H

#include "blocking.h"
#include "datastruct.h"
#include "command.h"

class User;
class UserList;

#include <stdio.h> // for FILE..

enum UserLevel { LAMER, READER, READ_WRITER, ADMIN };

class User : public Room {

	char *name;
	char *passwd; // crypted (including salt)
			// if NULL - there's no password.
	//UserLevel level;  
	char *level;	// more elastic than just UserLevel..
	char *descr; // description of the User.

public:
	//User(char *name, UserLevel level);
	User(char *name, char *level=NULL, char *passwd=NULL, 
		char *description=NULL);
	
	//void SetLevel(UserLevel level);
	void SetLevel(char *level);
	void SetCryptedPasswd(char *crypted_passwd); //(?)
	void SetPasswd(char *passwd); // crypt() it first!
	void SetDescr(char *descr); 
	
	char *GetName(void);
	char *GetPasswd(void);
	char *GetDescr(void);
	char *GetCryptedPasswd(void); //(?)
	//UserLevel GetLevel(void);
	char *GetLevel(void);
	void print(FILE *);
	
	int CheckPasswd(char *passwd); 
	int CheckCryptedPasswd(char *crytpted_passwd); //(?)

	// (?) means: 'Will we use it?'
};
			
class UserList {
	TListRoom<User> *list;

public:
	UserList();

	void AddUser(User*);	
	void AddUser(char*name);	
	void DelUser(User*);
	void DelUser(char*name);
	User *GetUser(char *name);
	int  IsUser(char*name);
	void SetUserPass(char *name, char *passwd);
	void SetUserLevel(char *name, char *level);
	void SetUserDescr(char *name, char *descr);
	User *Login(char *name, char *passwd);

	char *GetUserNames(int descriptions = 0);

	void save(FILE *file);
};	

// komendy protokolu

class UserLoginCommand : public ExecuteCommand {
public:
	UserLoginCommand () {};
	~UserLoginCommand () {};
	char * execute(Command *);
};

class UserSetPassCommand : public ExecuteCommand {
public:
	UserSetPassCommand () {};
	~UserSetPassCommand () {};
	char * execute(Command *);
};

class UserAddCommand : public ExecuteCommand {
public:
	UserAddCommand () {};
	~UserAddCommand () {};
	char * execute(Command *);
};

class UserDeleteCommand : public ExecuteCommand {
public:
	UserDeleteCommand () {};
	~UserDeleteCommand () {};
	char * execute(Command *);
};

class UserSetLevelCommand : public ExecuteCommand {
public:
	UserSetLevelCommand () {};
	~UserSetLevelCommand () {};
	char * execute(Command *);
};

class UserSetDescrCommand : public ExecuteCommand {
public:
	UserSetDescrCommand () {};
	~UserSetDescrCommand () {};
	char * execute(Command *);
};

class AdminListUsersCommand : public ExecuteCommand {
public:
	AdminListUsersCommand () {};
	~AdminListUsersCommand () {};
	char * execute(Command *);
};

#endif
