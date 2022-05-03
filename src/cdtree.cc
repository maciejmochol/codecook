// $Id: cdtree.cc,v 1.14 2000/11/05 21:20:55 code Exp $

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string>
#include <assert.h>

#include "datastruct.h"
#include "cdtree.h"
#include "ccerror.h"
#include "cstable.h"
#include "protocol.h"
#include "logger.h"
#include "thread.h"
#include "parser_cdtree.h"
#include "xmlgen.h"

// cdtree.cc

CD_Address::CD_Address(char *in) 
{ 
	size = allocated = 0;
	array = NULL; channel = ""; host = "";
	Parse(in);
}

CD_Address::CD_Address(string &in) 
{ 
	size = allocated = 0;
	array = NULL; channel = "";

	char *c = copy_string(in);

	Parse(c);
	free(c);
}

CD_Address::CD_Address(const CD_Address &in) 
{ 
string sin; 

	size = allocated = 0;
	array = NULL; channel = ""; host = "";

	sin = host;
	sin += in.path();
	sin += ",";
	sin += in.channel;

	char *c = copy_string(sin);
		Parse(c);
	free(c);
}




CD_Address::~CD_Address() 
{ 
	if (!array) return;
	for (int i = 0; i < size; i++) {
		delete array[i];
	}
	free(array);
};

void CD_Address::realloc_array(int tosize)
{

	int tmp = 1;

	while (tmp < tosize) {
		tmp *= 2;
	};

	if (tmp == allocated) return;

	allocated = tmp;
	array = (string**) realloc(array, allocated * sizeof(string*));
}


void CD_Address::NextCDToken(char **token, char *buf, int size, char div_char)
{
 char *c, *cend;
 int i;
 if (!token || !(*token) || !buf || !size) return;
 c = *token;

 ClearDivChar(&c, div_char); /* czysc poczatkowe space */
 *token = c;
  
 if (*c == 0) { *token = NULL; return; };
 bzero(buf, size);
 while (*c != div_char && *c != '\n' && *c != 0) c++;
 i = (c - *token);
 size--;
 if (i > size) i = size;
 strncpy(buf, *token, i);
 cend = buf + strlen(buf);
 while (cend > buf && (*cend == 0 || *cend == ' ' || *cend == '\n')) cend--; /* czysc koncowe space */
 if (cend+1 < buf+size) *(cend+1) = 0;
 *token = c;
}

void CD_Address::ClearDivChar(char **token, char div_char)
{
 if (!token || !*token) return;
 char *c = *token;

 while (*c == ' ' || *c == '\n' || *c == div_char) c++; /* czysc poczatkowe space */
 *token = c;
}

void CD_Address::Parse(char *in)
{
	char *c = in, *c1;
	char buf[100] = "", buf2[100];

	if (!c) return;

	if (strchr(c, '/')) {
		NextCDToken(&c, buf, sizeof(buf), '/');
		host = buf;
		ClearDivChar(&c, '/');
	};

	while (c != NULL) {
		NextCDToken(&c, buf, sizeof(buf), ':');
		// Testuj czy to ostatni token, jak tak to idz do nastepnej
		// sekcji, tam rozdzieli go na kanal
		if (c != NULL) {
			c1 = c; 
			NextCDToken(&c1, buf2, sizeof(buf2), ':');
			if (c1 != NULL && buf2[0]) { // to nie jest ostatni token
				// alokacja tokena adresu 
				realloc_array(size+1);
				array[size++] = new string(buf);
			}
		}
	};
	if (buf[0]) { // Zostal czlon typu x,x
		strncpy(buf2, buf, sizeof(buf2)); c = buf2;
		NextCDToken(&c, buf, sizeof(buf), ',');
		// alokacja tokena adresu 
		realloc_array(size+1);
		array[size++] = new string(buf);

		NextCDToken(&c, buf, sizeof(buf), ',');
		if (c != NULL) {
				channel = string(buf);
		}
	}
}

// CD_Node

string CD_Node::GetChannel(string channel)
{
	CD_Channel **ch = channels.get(channel);
	if (!ch) return "";
	return (*ch)->get();
}

string CD_Node::GetChannel(string channel, int &found)
{
	found = 0;
	CD_Channel **ch = channels.get(channel);
	if (!ch) return "";
	found = 1;
	return (*ch)->get();
}

DataRecord CD_Node::GetChannelDataRecord(string channel)
{
	CD_Channel **ch = channels.get(channel);
	if (!ch) return DataRecord();
	return (*ch)->GetDataRecord();
}

DataRecord CD_Node::GetChannelDataRecord(string channel, int &found)
{
	found = 0;
	CD_Channel **ch = channels.get(channel);
	if (!ch) return DataRecord();
	found = 1;
	return (*ch)->GetDataRecord();
}

void CD_Node::AddChannel(string name, CD_Channel* channel)
{
	if (name != "" && channel != NULL) channels.insert(name, channel);
}

void CD_Node::AddChannel(string name, string value)
{
	if (name != "") channels.insert(name, new CD_Channel(value));
}

void CD_Node::AddChannel(string name, int value)
{
	if (name != "") channels.insert( name, new CD_IntChannel(value) );
}

void CD_Node::SetChannel(string name, string value)
{
	if (name != "")
	if (name == "TYPE") throw exc_CD_Tree_ChannelReadOnly("CD_Node::SetChannel");
	else {
		CD_Channel **ch = channels.get(name);
		if (ch)
			(*ch)->set(value);
		else
			throw exc_CD_Tree_ChannelNotFound("CD_Node::SetChannel");
	}
}

void CD_Node::SetChannel(string name, int value)
{
	if (name != "") {
		CD_Channel **ch = channels.get(name);
		if ((*ch)->value_type != INT_VALUE) {
			// throw ...
		} else {
			**ch = CD_IntChannel(value);
		}
	}
}

CD_Node* CD_Node::FindNode(string name)
{
  	CD_Node **res = children.get(name);
	if (res) return *res;

	return NULL; 
}

void CD_Node::InsertNode(string name, CD_Node* node)
{
	children.insert(name, node);
}

string CD_Node::ShowChannels(bool CalledFromAlias = false)
{
	TIndexIterator_Names<CD_Channel*> it(channels);
	string s = "", ch;
	string type_name = "";

	do {
		ch = it.next();
		if (ch != "") {

			CD_Channel **channel = channels.get(ch);
			if (channel) {
				type_name = "UNKNOWN";
				switch ((*channel)->GetType()) {
					case INT_VALUE: type_name = "INT"; break;
					case DOUBLE_VALUE: type_name = "DOUBLE"; break;
					case STRING_VALUE: type_name = "STRING"; break;
				}
			}

			if (CalledFromAlias)
				if (ch == "TYPE") { ch = "TARGET_TYPE"; type_name = "STRING"; }
//PP
			s += XMLgen::Channel(ch, type_name);
		}
		else break;
	} while (1);
	return s;
}

string CD_Node::ShowChannelsValues(bool CalledFromAlias = false)
{
	TIndexIterator_Names<CD_Channel*> it(channels);
	string s = "", ch, chlower;
	string type_name = "";
	string space = "";

	do {
		ch = it.next();
		if (ch != "") {

			chlower = "";
			for (unsigned int i = 0; i < ch.length(); i++) chlower += tolower(ch[i]);
			s += space + XMLgen::Attribute(chlower, GetChannel(ch));
			space = " ";
		} else break;

	} while (1);
	
	s = XMLgen::Node(GetChannel("TYPE"), s);

	return s;
}

void CD_Node::ShowSubNodes(string &stream, string s = "")
{
	string s1 = "";

	TIndexIterator<CD_Node*> ii(children);
	TIndexIterator_Names<CD_Node*> inames(children);

	while (!ii.IsEnd()) {
		CD_Node **node = ii.next();
		if (node && *node) {
//PP
			s1 = s + inames.next();
			if ((*node)->GetChannel("TYPE") == "ALIAS") 
				stream += XMLgen::Node("ALIAS", 
					XMLgen::Attribute("name", s1) + " " +
					XMLgen::Attribute("target", (*node)->GetChannel("TARGET"))
			    	);
			else
			    stream += XMLgen::NamedNode((*node)->GetChannel("TYPE"),
				     s1);
			(*node)->ShowSubNodes(stream, s1 + ":");
		}
	}
}

void CD_Node::ScanAllSubNodes(string &stream, string s = "", TSortedArray<CD_Node*> *array = NULL)
{
	string s1 = "", name = "";
	TSortedArray<CD_Node*> *arr;
	CD_Node *ref;

	if (array) arr = array; else arr = new TSortedArray<CD_Node*>();

	TIndexIterator<CD_Node*> ii(children);
	TIndexIterator_Names<CD_Node*> inames(children);

	if (GetChannel("TYPE") == "ALIAS") { // zaczynamy jako alias

		try {
			ref = ((CD_Node_Alias*) this)->Resolve();
			s1 =  ((CD_Node_Alias*) this)->ResolveName();

			try {
				arr->insert(ref);
			}
			catch (exc_TSortedArray_AlreadyExists) {
				return; // ten wezel juz byl zaliczony
			}

			ref->ScanAllSubNodes(stream, s1 + ":", array);
			return;
		}
		catch (exc_CD_Tree_Alias) {
			return; // blad aliasa
		}

		return;

	}

	while (!ii.IsEnd()) {
		CD_Node **node = ii.next();
		if (!node || !*node) continue;

		ref = *node;		

		name = inames.next(); // czlon poczatkowy adresu
		s1 = s + name;

		try {
			if (ref->GetChannel("TYPE") == "ALIAS") {
				ref = ((CD_Node_Alias*) *node)->Resolve();
				name = s1 =  ((CD_Node_Alias*) *node)->ResolveName();
			}
		}
		catch (exc_CD_Tree_Alias) {
			continue; // blad aliasa
		}

		try {
			arr->insert(ref);
		}
		catch (exc_TSortedArray_AlreadyExists) {
			continue; // ten wezel juz byl zaliczony
		}
//PP
		if (array) stream += XMLgen::NamedNode(ref->GetChannel("TYPE"), s1);
		else stream += XMLgen::NamedNode(ref->GetChannel("TYPE"), name);
		ref->ScanAllSubNodes(stream, s1 + ":", arr);
	}

	if (!array && arr) delete arr;
}

void CD_Node::ScanSubNodes(string &stream, string WhichNodes, string s = "", TSortedArray<CD_Node*> *array = NULL)
{
	string s1 = "", name = "";
	TSortedArray<CD_Node*> *arr;

	if (array) arr = array; else arr = new TSortedArray<CD_Node*>();

	TIndexIterator<CD_Node*> ii(children);
	TIndexIterator_Names<CD_Node*> inames(children);

	CD_Node *ref;

	if (GetChannel("TYPE") == "ALIAS") { // zaczynamy jako alias

		try {
			ref = ((CD_Node_Alias*) this)->Resolve();
			s1 =  ((CD_Node_Alias*) this)->ResolveName();

			try {
				arr->insert(ref);
			}
			catch (exc_TSortedArray_AlreadyExists) {
				return; // ten wezel juz byl zaliczony
			}

			ref->ScanSubNodes(stream, WhichNodes, s1 + ":", array);
			return;
		}
		catch (exc_CD_Tree_Alias) {
			return; // blad aliasa
		}

		return;

	}

	while (!ii.IsEnd()) {
		CD_Node **node = ii.next();
		if (!node || !*node) continue;
		ref = *node;

		name = inames.next(); // czlon poczatkowy adresu
		s1 = s + name;

		try {
			if ((*node)->GetChannel("TYPE") == "ALIAS") {
				ref = ((CD_Node_Alias*) *node)->Resolve();
				name = s1 =  ((CD_Node_Alias*) *node)->ResolveName();
			}
		}
		catch (exc_CD_Tree_Alias) {
			continue; // blad aliasa
		}

		try {
			arr->insert(ref);
		}
		catch (exc_TSortedArray_AlreadyExists) {
			continue; // ten wezel juz byl zaliczony
		}

		if (ref->GetChannel("TYPE") == WhichNodes) { 
			// to te wezly, dodaj do strumienia
//PP
			if (array) stream += XMLgen::NamedNode(ref->GetChannel("TYPE"), s1);
			else stream += XMLgen::NamedNode(ref->GetChannel("TYPE"), name);
		}

		ref->ScanSubNodes(stream, WhichNodes, s1 + ":", arr);
	}

	if (!array && arr) delete arr;
}

void CD_Node::SearchSubNodes(string &stream, string WhichNodes, string s = "")
{
	string s1 = "", s2 = "";

	TIndexIterator<CD_Node*> ii(children);
	TIndexIterator_Names<CD_Node*> inames(children);

	while (!ii.IsEnd()) {
		CD_Node **node = ii.next();
		if (node && *node) {
			s1 = s + inames.next();
			s2 = XMLgen::NamedNode(WhichNodes, s1);

			if ((*node)->GetChannel("TYPE") == WhichNodes) stream += s2; 

			(*node)->SearchSubNodes(stream, WhichNodes, s1 + ":");
		}
	}
}

// CD_Tree

CD_Tree::CD_Tree() 
{ 
	cstable->protocol->AddCommand("LIST", new CD_Tree_Cmd_List(),
		"listuje drzewo");
	cstable->protocol->AddCommand("LIST <??????>", new CD_Tree_Cmd_Search(),
		"listuje wszystkie wezly o podanym kanale TYPE");
	cstable->protocol->AddCommand("SHOWCHANNELS <??????>", new CD_Tree_Cmd_ShowChannels(), 
		"pokazuje kanaly danego wezla");
	cstable->protocol->AddCommand("SHOW <??????>", new CD_Tree_Cmd_Show(), 
		"pokazuje wartosc danego kanalu");
	cstable->protocol->AddCommand("SET <??????> <??????>", new CD_Tree_Cmd_Set(), 
		"ustawia wartosc danego kanalu");
	cstable->protocol->AddCommand("ALIAS <??????> <??????>", new CD_Tree_Cmd_Alias(), 
		"dodaje alias");
	cstable->protocol->AddCommand("SCAN <??????>", new CD_Tree_Cmd_ScanAll(), 
		"przeszukuje poddrzewo uwzgledniajac aliasy");
	cstable->protocol->AddCommand("SCAN <??????> <??????>", new CD_Tree_Cmd_Scan(), 
		"przeszukuje poddrzewo uwzgledniajac aliasy i znajdujac podane wezly");
};

void CD_Tree::ReplaceEmptySubNode(CD_Node *ref, string name, CD_Node *newnode)
{
	CD_Node *myref = ref->FindNode(name);
	if (!myref) throw exc_CD_Tree_NodeNotFound("CD_Tree::ReplaceEmptySubNode(node="+name+")");
	if (myref->GetChannel("TYPE") == "EMPTY") {
  		newnode->children = myref->children;
		ref->children.del(name);
		ref->children.insert(name, newnode);
		delete myref;
	}
	else throw exc_CD_Tree_NonEmptyNodeExists("CD_Tree::ReplaceEmptySubNode(node="+name+")");
}

void CD_Tree::InsertNode(const CD_Address &path, CD_Node* node)
{
	EnterWriter();

	CD_Node *ref = &root;

	if (!node) {
		LeaveWriter();
		throw exc_CD_Tree_NodeNotFound("CD_Tree::InsertNode");
	}

	if (path.length()) {
		for (int i = 0; i < path.length(); i++) {
			if (i + 1 == path.length()) {
				try {
					ref->InsertNode(path[i], node);
				}
				catch (exc_TIndex_AlreadyExists) {
					if (node->GetChannel("TYPE") == "ALIAS") {
						LeaveWriter();
						throw exc_CD_Tree_CannotInsertAlias("CD_Tree::InsertNode");
					}
					try {
						ReplaceEmptySubNode(ref, path[i], node);
					}
					catch (exc_Codecook &exc) {
						LeaveWriter();
						ThrowCorrectException(exc); // trzeba odblokowac
					}
				}
			}
			else {
			 	try {
				 ref->InsertNode(path[i], new CD_Node_Empty()); // Wstaw "pusty" wezel
				}
				catch (exc_TIndex_AlreadyExists) { }; // ...no, chyba ze istnieje juz jakis
			}
			ref = ref->FindNode(path[i]);
		};
	}

	LeaveWriter();
};

CD_Node* CD_Tree::_GetNode(const CD_Address &path)
{
	CD_Node *ref = &root;

	if (path.length()) {
		for (int i = 0; i < path.length(); i++) {
			ref = ref->FindNode(path[i]);
			if (!ref) return NULL;
		};
	}
	return ref;
};

string CD_Tree::GetChannel(const CD_Address &path)
{
	int found; 

	return GetChannel(path, found);
};

string CD_Tree::GetChannel(const CD_Address &path, int &found)
{
	string s;

	EnterReader();
	CD_Node *ref = _GetNode(path);
	if (ref) {
		try {
			s = ref->GetChannel(path.channel, found);
		}
		catch (exc_Codecook &exc) {
			LeaveReader();
			ThrowCorrectException(exc);
		}
		LeaveReader();
		return s;
	}
	else {
		LeaveReader();
		throw exc_CD_Tree_NodeNotFound("CD_Tree::GetChannel");
//		return "";
	}
//	return "";
// Nie ma prawa tutaj byc, ma zwrocic albo good rezultat, albo exception
	assert(0);
};

void CD_Tree::SetChannel(const CD_Address &path, string value)
{
	EnterReader();
	CD_Node *ref = _GetNode(path);
	if (ref) {
		try {
			ref->SetChannel(path.channel, value);
		}
		catch (exc_Codecook &exc) {
			LeaveReader();
			ThrowCorrectException(exc);
		}
		LeaveReader();
		return;
	}
	else {
		LeaveReader();
		throw exc_CD_Tree_NodeNotFound("CD_Tree::SetChannel");
//		return;
	}
//	return;
// Nie ma prawa tutaj byc, ma zwrocic albo good rezultat, albo exception
	assert(0);
};

DataRecord CD_Tree::GetChannelDataRecord(const CD_Address &path)
{
	int found; 

	return GetChannelDataRecord(path, found);
};

DataRecord CD_Tree::GetChannelDataRecord(const CD_Address &path, int &found)
{
	DataRecord dr;

	EnterReader();
	CD_Node *ref = _GetNode(path);
	if (ref) {
		try {
			dr = ref->GetChannelDataRecord(path.channel, found);
		}
		catch (exc_Codecook &exc) {
			LeaveReader();
			ThrowCorrectException(exc);
		}
		LeaveReader();
		return dr;
	}
	else {
		LeaveReader();
		throw exc_CD_Tree_NodeNotFound("CD_Tree::GetChannelDataRecord");
		assert(0);
	}
	assert(0);
};

string CD_Tree::ShowChannels(const CD_Address &path)
{
	string s;

	EnterReader();

	CD_Node *ref = _GetNode(path);
	if (ref) {
		try {
			s = ref->ShowChannels();
		}
		catch (exc_Codecook &exc) {
			LeaveReader(); // zwalnia klase
			ThrowCorrectException(exc); // i jup !
		}
		LeaveReader();
		return s;
	}
	else {
		LeaveReader();
		throw exc_CD_Tree_NodeNotFound("CD_Tree::ShowChannels");
	}
	assert(0);
};

string CD_Tree::ShowChannelsValues(const CD_Address &path)
{
	string s;

	EnterReader();

	CD_Node *ref = _GetNode(path);
	if (ref) {
		try {
			s = ref->ShowChannelsValues();
		}
		catch (exc_Codecook &exc) {
			LeaveReader(); // zwalnia klase
			ThrowCorrectException(exc); // i jup !
		}
		LeaveReader();
		return s;
	}
	else {
		LeaveReader();
		throw exc_CD_Tree_NodeNotFound("CD_Tree::ShowChannelsValues");
	}
	assert(0);
};


void CD_Tree::ScanNodes(const CD_Address &path, string WhichNodes, string &s)
{
	EnterReader();

	CD_Node *ref = _GetNode(path);
	if (ref) {
		ref->ScanSubNodes(s, WhichNodes, path.path() + ":");
	}
	else {
		LeaveReader();
		throw exc_CD_Tree_NodeNotFound("CD_Tree::ScanNodes");
	}

	LeaveReader();
}

void CD_Tree::ScanAllNodes(const CD_Address &path, string &s)
{
	EnterReader();

	CD_Node *ref = _GetNode(path);
	if (ref) {
		ref->ScanAllSubNodes(s, path.path() + ":");
	}
	else {
		LeaveReader();
		throw exc_CD_Tree_NodeNotFound("CD_Tree::ScanAllNodes");
	}

	LeaveReader();
}


// Tree blocked methods

CD_Node* CD_Tree::GetNode(const CD_Address &path)
{
	EnterReader();
		CD_Node *ret = _GetNode(path);
	LeaveReader();

	return ret;
}

void CD_Tree::Load(string file_name)
{

	CD_Parser lParse(this, file_name);

	if(!lParse.ParseFile()) {
		cstable->logger->Log(
		"(CD_Tree) : Cannot load tree (from : %s)!", 
			file_name.data()); // do poprawienia ta data()
	}
	else {
	// string lS;
	// cstable->tree->ShowNodes(lS);
		cstable->logger->Log(
		"(CD_Tree) : Loaded tree (from : %s)", file_name.data());
	// cstable->logger->Log(lS.data());		// do poprawienia	
	}
}


// CD_Node_Empty

CD_Node_Empty::CD_Node_Empty() : CD_Node()
{
	AddChannel("TYPE", "EMPTY");
};

// CD_Node_Root

CD_Node_Root::CD_Node_Root() : CD_Node()
{
	AddChannel("TYPE", "ROOT");
	AddChannel("CDVERSION", "$Id: cdtree.cc,v 1.14 2000/11/05 21:20:55 code Exp $");
};

// CD_Node_Alias

CD_Node_Alias::CD_Node_Alias(CD_Tree &tree, string target) : CD_Node()
{
	CD_Node::AddChannel("TYPE", "ALIAS");
	CD_Node::AddChannel("TARGET", target);
	cdtree = &tree;
};

CD_Node* CD_Node_Alias::Resolve()
{
	int level = 0;

	string s = GetChannel("TARGET");
	CD_Node *node = cdtree->_GetNode( s );

	while (node) {
		if (node->GetChannel("TYPE") == "ALIAS") {
			level++;
			if (level > 40) throw exc_CD_Tree_Alias_TooManyLinks("CD_Node_Alias::Resolve");

			s = node->GetChannel("TARGET");
			node = cdtree->GetNode(s);
		} else break;
	}
	if (!node) throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::Resolve");
	return node;
};

string CD_Node_Alias::ResolveName()
{
	int level = 0;

	string s = GetChannel("TARGET");
	CD_Node *node = cdtree->_GetNode( s );

	while (node) {
		if (node->GetChannel("TYPE") == "ALIAS") {
			level++;
			if (level > 40) throw exc_CD_Tree_Alias_TooManyLinks("CD_Node_Alias::ResolveName");

			s = node->GetChannel("TARGET");			
			node = cdtree->GetNode(s);
		} else break;
	}
	if (!node) throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::ResolveName");
	return s;
};

string CD_Node_Alias::GetChannel(string channel, int &found)
{
	if (channel == "TYPE" || channel == "TARGET") {
		found = 1;
		return CD_Node::GetChannel(channel);
	}

	if (channel == "TARGET_TYPE") {
		found = 1;
		channel = "TYPE";	
	}

	CD_Node *node = Resolve();

	if (node) return node->GetChannel(channel, found);
	else throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::GetChannel");
};

void CD_Node_Alias::AddChannel(string name, CD_Channel *channel)
{
	CD_Node *node = Resolve();

	if (node) node->AddChannel(name, channel);
	else throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::AddChannel");
};

void CD_Node_Alias::AddChannel(string name, string value)
{
	CD_Node *node = Resolve();

	if (node) node->AddChannel(name, value);
	else throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::AddChannel");
};

void CD_Node_Alias::AddChannel(string name, int value)
{
	CD_Node *node = Resolve();

	if (node) node->AddChannel(name, value);
	else throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::AddChannel");
};

void CD_Node_Alias::SetChannel(string name, string value)
{
	CD_Node *node = Resolve();

	if (node) node->SetChannel(name, value);
	else throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::SetChannel");
};

void CD_Node_Alias::SetChannel(string name, int value)
{
	CD_Node *node = Resolve();

	if (node) node->SetChannel(name, value);
	else throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::SetChannel");
};

string CD_Node_Alias::ShowChannels(bool CalledFromAlias = false) 
{
	string s = "";

	CD_Node *node = Resolve();

	s += CD_Node::ShowChannels();

	if (node) {
		s += node->ShowChannels(true);
	}
	else throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::ShowChannels");


	return s;
};

string CD_Node_Alias::ShowChannelsValues(bool CalledFromAlias = false) 
{
	string s = "";

	CD_Node *node = Resolve();

	s += CD_Node::ShowChannelsValues();

	if (node) {
		s += node->ShowChannelsValues(true);
	}
	else throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::ShowChannelsValues");


	return s;
};

void CD_Node_Alias::InsertNode(string name, CD_Node* node)
{
	CD_Node *_node = Resolve();

	if (_node) _node->InsertNode(name, node);
	else throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::InsertNode");
}

CD_Node* CD_Node_Alias::FindNode(string name)
{
	CD_Node *node = Resolve();

	if (node) return node->FindNode(name);
	else throw exc_CD_Tree_UnresolvedAlias("CD_Node_Alias::FindNode");
}


// CD_Node_Raport

CD_Node_Raport::CD_Node_Raport() : CD_Node()
{
	AddChannel("TYPE", "RAPORT");
	AddChannel("RAPORT_NAME", "Empty raport");
};


// CD_Tree commands


char * CD_Tree_Cmd_List::execute(Command *cmd)
{
	string s, s1 = "";
//PP
	//cout << "MEM USAGE: " << ((Thread*) cstable->meaner)->UnsharedDataSize() << endl;
	cstable->tree->ShowNodes(s);
	s1 += XMLgen::Command(cmd) + XMLgen::List_res(s);
	//cout << "AFTER - MEM USAGE: " << ((Thread*) cstable->meaner)->UnsharedDataSize() << endl;

	return copy_string(s1);
};

char * CD_Tree_Cmd_Search::execute(Command *cmd)
{
	string s, s1 = "";

	Token *tok = (*cmd)[1];

	cstable->tree->SearchNodes(s, tok->string());
//PP
	s1 += XMLgen::Command(cmd) + XMLgen::List_res(s);

	return copy_string(s1);
};


char * CD_Tree_Cmd_ShowChannels::execute(Command *cmd)
{
	string s = "";

	Token *tok = (*cmd)[1];

	s = XMLgen::Command(cmd);

	try {
//<PP>
		s += XMLgen::Showchannels_res(tok->string(), 
		    cstable->tree->ShowChannels(tok->string()) );
	}
	catch (exc_CD_Tree_NodeNotFound &exc) {
		s += XMLgen::Command(cmd) + XMLgen::Error(&exc, 
		    "exc_CD_Tree_NodeNotFound");
	}
	catch (exc_CD_Tree_UnresolvedAlias &exc) {
		s += XMLgen::Command(cmd) + XMLgen::Error(&exc, 
		    "exc_CD_Tree_UnresolvedAlias");
	}
	return copy_string(s);
};

char * CD_Tree_Cmd_Show::execute(Command *cmd)
{
	string s = "";
	int found;

	Token *tok = (*cmd)[1];

	CD_Address addr(tok->string());

	s += XMLgen::Command(cmd);

	try {

		if (addr.channel == "") {
			// Show all channels
			CD_Address addr2 = addr;
			s += XMLgen::Show_res( 
				addr.show(),
				cstable->tree->ShowChannelsValues(addr)
			);
		} else {
			// Show one channel
			string s1 = cstable->tree->GetChannel(addr, found);
			CD_Address addr2 = addr;
			addr2.channel = "TYPE";
			string stype = cstable->tree->GetChannel(addr2);
			string chlower = "";
			for (unsigned int i = 0; i < addr.channel.length(); i++) 
				chlower += tolower(addr.channel[i]);

			s += XMLgen::Show_res( 
				addr.show(),
				XMLgen::Node(stype, XMLgen::Attribute(chlower, s1))
			);
		}		
//<PP>
		if (!found) s += XMLgen::Error("Channel not found");
	}
	catch (exc_CD_Tree_NodeNotFound &exc) {
		s += XMLgen::Error(&exc, "exc_CD_Tree_NodeNotFound");
	}
	catch (exc_CD_Tree_Alias &exc) {
		s += XMLgen::Error(&exc, "exc_CD_Tree_Alias");
	}

	return copy_string(s);
};

char * CD_Tree_Cmd_Set::execute(Command *cmd)
{
	string s;
	int found;

	Token *tok = (*cmd)[1];
	Token *val = (*cmd)[2];

	CD_Address addr(tok->string());
//PP
	s = XMLgen::Command(cmd);
	try {
		cstable->tree->GetChannel(addr, found);
		if (!found) 
		    s += XMLgen::Error("Channel not found");
		else {
			cstable->tree->SetChannel(addr, string(val->string()));
			s += XMLgen::Set_res(XMLgen::Result("OK", "Value set to channel"));
		}
	}
	catch (exc_CD_Tree_NodeNotFound &exc) {
		s += XMLgen::Error(&exc, "exc_CD_Tree_NodeNotFound");
	}
	catch (exc_CD_Tree_Alias &exc) {
		s += XMLgen::Error(&exc, "exc_CD_Tree_Alias");
	}
	catch (exc_CD_Tree_ChannelReadOnly &exc) {
		s += XMLgen::Error(&exc, "exc_CD_Tree_ChannelReadOnly");
	}

	return copy_string(s);
};

char * CD_Tree_Cmd_Alias::execute(Command *cmd)
{
	string s;

		// cel
	Token *tok = (*cmd)[2];
	CD_Node_Alias *alias = new CD_Node_Alias(*(cstable->tree), tok->string());

		// nazwa aliasa
	tok = (*cmd)[1];
	CD_Address addr(tok->string());
// PP
	s = XMLgen::Command(cmd);
	try {
		cstable->tree->InsertNode(addr, alias);
		s += XMLgen::Alias_res(XMLgen::Result("OK", "Alias added"));
	}
	catch (exc_CD_Tree_CannotInsertAlias &exc) {
		s += XMLgen::Error(&exc, "exc_CD_Tree_CannotInsertAlias");
	}

	return copy_string(s);
};

char * CD_Tree_Cmd_ScanAll::execute(Command *cmd)
{
	string s, s1;

		// poddrzewo
	Token *tok = (*cmd)[1];
	CD_Address addr(tok->string());
// PP
	s = XMLgen::Command(cmd);
	try {
		cstable->tree->ScanAllNodes(addr, s1);
		s += XMLgen::Scan_res(s1);
	}
	catch (exc_CD_Tree_NodeNotFound &exc) {
		s += XMLgen::Error(&exc, "exc_CD_Tree_NodeNotFound");
	}

	return copy_string(s);
};

char * CD_Tree_Cmd_Scan::execute(Command *cmd)
{
	string s, s1;

		// poddrzewo
	Token *tok = (*cmd)[1];
	CD_Address addr(tok->string());

		// ktore wezly
	tok = (*cmd)[2];
// PP
	s = XMLgen::Command(cmd);
	try {
		cstable->tree->ScanNodes(addr, tok->string(), s1);
		s += XMLgen::Scan_res(s1);
	}
	catch (exc_CD_Tree_NodeNotFound &exc) {
		s += XMLgen::Error(&exc, "exc_CD_Tree_NodeNotFound");
	}

	return copy_string(s);
};

