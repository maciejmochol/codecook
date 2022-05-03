/* cdtree.h */

/* Copyright <C> 1999 by Codematic Solutions */

#ifndef _CDTREE_H_
#define _CDTREE_H_

#ifndef _REENTRANT
#define _REENTRANT
#endif

#include "datastruct.h"
#include "command.h"
#include "ptable.h"

// DataRecord

enum { DEFINITE, INDEFINITE, NOT_CALC }; 
struct _DataRecord {
	int type; 	/* DEFINITE, INDEFINITE */
	DataTypes val_type; 	

	union {
		double value;
		int    value_i;
		char   *c;
	};

	_DataRecord() { type = INDEFINITE; val_type = INT_VALUE; }
	
	_DataRecord(int t, double v=0.0) {  
		type = t; 
		value = v; 
		val_type = DOUBLE_VALUE;
	}
	
	_DataRecord(int t, int v=0){
		type = t; 
		value_i = v; 
		val_type = INT_VALUE;
	}

	_DataRecord(int t, string s = "") {
		type = t; 
		c = copy_string(s); 
		val_type = STRING_VALUE;
	}

	// copy
	_DataRecord(const _DataRecord &dr) {
		type = dr.type;
		val_type = dr.val_type;
		switch (val_type) {
			case DOUBLE_VALUE: value = dr.value; break;
			case INT_VALUE: value_i = dr.value_i; break;
			case STRING_VALUE: c = strdup(dr.c); break;
		}
	}
};
typedef struct _DataRecord DataRecord;

// CD_Address

struct CD_Address {
	int size, allocated;
	string **array;

	void NextCDToken(char **token, char *buf, int size, char div_char);
	void ClearDivChar(char **token, char div_char);
	void realloc_array(int tosize);
public: 
	string host;
	string channel;

	CD_Address(char *in);
	CD_Address(string &in);
	CD_Address(const CD_Address &in);
	~CD_Address();
	void Parse(char *in);	
	int length() const { return size; };
	string path() const { 
		string s = "";
		for (int i = 0; i < length(); i++) {
			s += (*this)[i];
			if (i+1 < length()) s += ":";
		};
		return s;
	};

	void SetChannel(string s) { channel = s; }
	
	string show() const { 
		string s = "";
		
		if (host != "") s = host + "/";
		s += path();  
  		if (channel != "") s += "," + channel;
		return s;	
	}

	string operator [] (int i) const {
		if (i < 0 || i >= size) return string("");
		return string(*(array[i]));
	};
	
	friend ostream& operator << (ostream &o, const CD_Address &addr) { 
		o << addr.show() << endl;
		return o;
	};
};

// CD_Channel

class CD_Channel {
	friend class CD_Node;
protected:
	string value;
	DataTypes value_type;
public:
	CD_Channel() : value(""), value_type(STRING_VALUE) { };
	CD_Channel(string s) { set(s); value_type = STRING_VALUE; };
	CD_Channel(char *c) : value(c) { value_type = STRING_VALUE; };

	bool operator == (CD_Channel &i) { return value == i.value; };
	bool operator != (CD_Channel &i) { return value != i.value; };
	friend ostream& operator << (ostream &o, const CD_Channel &ch) { o << ch.value; return o; };

	virtual void set(string content) { value = content; };
	virtual string get() { return value; };
	virtual DataTypes GetType() { return value_type; };
	virtual DataRecord GetDataRecord() { return DataRecord(DEFINITE, value); };
	virtual ~CD_Channel() { };
};

// CD_IntChannel 

class CD_IntChannel : public CD_Channel {
	int intvalue;
public:
	CD_IntChannel() : intvalue(0) { value_type = INT_VALUE; };
	CD_IntChannel(int i) : intvalue(i) { value_type = INT_VALUE; };
	CD_IntChannel(string s) { set(s); value_type = INT_VALUE; };
	
	virtual void set(string s) {
		char * _s = copy_string(s); 

		intvalue = strtol(_s, NULL, 10);
		// there should be generated exception if bad int value
		// FIX IT
	};
	virtual string get() {
		char s[20];
		snprintf(s, sizeof(s), "%d", intvalue);
		return string(s);
	}
	virtual DataRecord GetDataRecord() { return DataRecord(DEFINITE, intvalue); };
	bool operator == (CD_IntChannel &i) { return intvalue == i.intvalue; };
	bool operator != (CD_IntChannel &i) { return intvalue != i.intvalue; };
};

// CD_Node
// klasa CD_Node jest abstrakcyjna, nie mo¿na jej u¿ywaæ

class CD_Node {
	friend class CD_Tree;
public:
	TIndex<CD_Channel*> channels;
  	TIndex<CD_Node*> children;

	CD_Node() { };
	virtual ~CD_Node() { };

	virtual string GetChannel(string channel, int &found);
	string GetChannel(string channel);
	virtual DataRecord GetChannelDataRecord(string channel, int &found);
	DataRecord GetChannelDataRecord(string channel);

	virtual void AddChannel(string name, CD_Channel *channel);
	virtual void AddChannel(string name, string value);
	virtual void AddChannel(string name, int value);
	virtual void SetChannel(string name, string value);
	virtual void SetChannel(string name, int value);

	virtual CD_Node* FindNode(string name);
	virtual void InsertNode(string name, CD_Node* node);

	virtual string ShowChannels(bool CalledFromAlias = false);
	virtual string ShowChannelsValues(bool CalledFromAlias = false);
	void ShowSubNodes(string &stream, string s = "");
	void SearchSubNodes(string &stream, string WhichNodes, string s = "");
	void ScanSubNodes(string &stream, string WhichNodes, string s = "", TSortedArray<CD_Node*> *array = NULL); 
		// z obsluga aliasow - unika cykli - zre pamiec
	void ScanAllSubNodes(string &stream, string s = "", TSortedArray<CD_Node*> *array = NULL); 
		// z obsluga aliasow - unika cykli - zre pamiec
	virtual void Use() = 0;

	friend ostream& operator << (ostream &o, const CD_Node &node) { 
		o << "NODE[chan= " << node.channels << ",child= " << node.children << "]"; 
		return o; 
	};
};

// pochodne klasy CD_Node - rozne typy wezlow drzewa

// Empty

class CD_Node_Empty : public CD_Node {
public:
	CD_Node_Empty();
	void Use() { };
};

// Root

class CD_Node_Root : public CD_Node {
public:
	CD_Node_Root();
	void Use() { };
};

// Alias

class CD_Node_Alias : public CD_Node {
	CD_Tree *cdtree;
public:
	CD_Node_Alias(CD_Tree &tree, string target);
	CD_Node* Resolve();
	string ResolveName();
	void Use() { };

	string GetChannel(string channel, int &found);
	string GetChannel(string channel) { 
		int found; 
		return GetChannel(channel, found); 
	}

	void AddChannel(string name, CD_Channel *channel);
	void AddChannel(string name, string value);
	void AddChannel(string name, int value);
	void SetChannel(string name, string value);
	void SetChannel(string name, int value);
	CD_Node* FindNode(string name);
	void InsertNode(string name, CD_Node* node);

	virtual string ShowChannels(bool CalledFromAlias = false);
	virtual string ShowChannelsValues(bool CalledFromAlias = false);
};


// Raport

class CD_Node_Raport : public CD_Node {
public:
	CD_Node_Raport();
	void Use() { };
};


// CD_Tree

class CD_Tree : public Room {
	friend class CD_Node_Alias;
	
	CD_Node_Root root;

protected:
	void ReplaceEmptySubNode(CD_Node *ref, string name, CD_Node *newnode);
	CD_Node* _GetNode(const CD_Address &path);
	
public:

	CD_Tree();
	~CD_Tree() { };

	void InsertNode(const CD_Address &path, CD_Node* node);
	CD_Node* GetNode(const CD_Address &path);
	string GetChannel(const CD_Address &path);
	string GetChannel(const CD_Address &path, int &found);
	void SetChannel(const CD_Address &path, string value);
	DataRecord GetChannelDataRecord(const CD_Address &path, int &found);
	DataRecord GetChannelDataRecord(const CD_Address &path);
	string ShowChannels(const CD_Address &path);
	string ShowChannelsValues(const CD_Address &path);
	void ShowNodes(string &s) { EnterReader(); root.ShowSubNodes(s); LeaveReader(); };
	void ScanNodes(const CD_Address &path, string WhichNodes, string &s);
	void ScanAllNodes(const CD_Address &path, string &s);
	void SearchNodes(string &s, string WhichNodes) { 
		EnterReader(); 
		root.SearchSubNodes(s, WhichNodes); 
		LeaveReader();
	};
	void Load(string file_name);
};

// CD_Tree commands

class CD_Tree_Cmd_List : public ExecuteCommand {
public:
	CD_Tree_Cmd_List() { };
	~CD_Tree_Cmd_List() { };
	virtual char * execute(Command *);
};

class CD_Tree_Cmd_Search : public ExecuteCommand {
public:
	CD_Tree_Cmd_Search() { };
	~CD_Tree_Cmd_Search() { };
	virtual char * execute(Command *);
};

class CD_Tree_Cmd_ShowChannels : public ExecuteCommand {
public:
	CD_Tree_Cmd_ShowChannels() { };
	~CD_Tree_Cmd_ShowChannels() { };
	virtual char * execute(Command *);
};

class CD_Tree_Cmd_Show : public ExecuteCommand {
public:
	CD_Tree_Cmd_Show() { };
	~CD_Tree_Cmd_Show() { };
	virtual char * execute(Command *);
};

class CD_Tree_Cmd_Set : public ExecuteCommand {
public:
	CD_Tree_Cmd_Set() { };
	~CD_Tree_Cmd_Set() { };
	virtual char * execute(Command *);
};

class CD_Tree_Cmd_Alias : public ExecuteCommand {
public:
	CD_Tree_Cmd_Alias() { };
	~CD_Tree_Cmd_Alias() { };
	virtual char * execute(Command *);
};

class CD_Tree_Cmd_ScanAll : public ExecuteCommand {
public:
	CD_Tree_Cmd_ScanAll() { };
	~CD_Tree_Cmd_ScanAll() { };
	virtual char * execute(Command *);
};

class CD_Tree_Cmd_Scan : public ExecuteCommand {
public:
	CD_Tree_Cmd_Scan() { };
	~CD_Tree_Cmd_Scan() { };
	virtual char * execute(Command *);
};



#endif
