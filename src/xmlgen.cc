// pawel

#include "xmlgen.h"
#include "codecook.h"
#include <string.h>

string XMLgen::Message(string message)
{
    string s;
    s += "<MESSAGE message=\"";
    s += message;
    s += "\">\n";
    return s;

}

string XMLgen::Error(string message) 
{
    string s = "";
    s += "<ERROR type=\"unknown\" message=\"";
    s += message;
    s += "\">\n";
    return s;
}

string XMLgen::Error(exc_Codecook *exc, string name)
{
    char tmp[80];
    string s = "";
    s += "<ERROR type=\"exception\" code=\"";
    snprintf(tmp, 80, "%d", exc->code);
    s += tmp;
    s += "\" name=\"" + name + "\" throw_from=\"";
    s += exc->throw_from;
    s += "\" name=\"";
    s += exc->s;
    s += "\"/>\n";
    return s;
}

string XMLgen::Attribute(string name, string value)
{
    string s = "";
    s = name + "=\"" + value + "\"";
    return s;
}

string XMLgen::Showchannels_res(string node, string channels) 
{
    string s = "";
    s += "<SHOWCHANNELS_RES " + Attribute("node", node) + ">\n" + channels + "</SHOWCHANNELS_RES>\n";
    return s;
}

string XMLgen::Channel(string name, string type)
{
    string s = "";
    s += "<CHANNEL name=\"" + name + "\" type=\"" + type + "\"/>\n";
    return s;
}

string XMLgen::Node(string type, string attrs)
{
    string s = "";
    s = "<" + type + " " + attrs + "/>\n";
    return s;
}

string XMLgen::NamedNode(string type, string name)
{
    string s = "";
    s +=  "<" + type + " " + Attribute("name", name) + "/>\n";
    return s;
}

string XMLgen::Show_res(string node, string res)
{
    string s = "";
    s +=  "<SHOW_RES " + Attribute("node", node) + ">\n" + res + "</SHOW_RES>\n";
    return s;
}

string XMLgen::Set_res(string res)
{
    string s = "";
    s +=  "<SHOW_RES>\n" + res + "</SHOW_RES>\n";
    return s;
}

string XMLgen::Alias_res(string res)
{
    string s = "";
    s +=  "<ALIAS_RES>\n" + res + "</ALIAS_RES>\n";
    return s;
}

string XMLgen::Scan_res(string res)
{
    string s = "";
    s +=  "<SCAN_RES>\n" + res + "</SCAN_RES>\n";
    return s;
}

string XMLgen::List_res(string res)
{
    string s = "";
    s +=  "<LIST_RES>\n" + res + "</LIST_RES>\n";
    return s;
}

string XMLgen::Command(string text)
{
    string s = "";
    s +=  "<COMMAND text\"" + text + "\"/>\n";
    return s;
}

string XMLgen::Command(class Command *cmd)
{
    string s = "";
    if (cmd == NULL)
	s += "<COMMAND text=\"Unknown\"/>\n";
    else {
	s += "<COMMAND text=\"";
	cmd->beginWithFirstToken();
	while (! cmd->onTheEnd()) {
	    s += (cmd->nextToken())->string();
	    s += " ";
	}
	s += "\"/>\n";
    }
    return s;
}

string XMLgen::Thread(string name, long id, int pid, string type)
{
    char tmp[80];
    
    string s = "";
    s += "<THREAD name=\"";
    s += copy_string(name);
    snprintf(tmp, 80, "%ld", id);
    s += "\" id=\"";
    s += tmp;
    snprintf(tmp, 80, "%d", pid);
    s += "\" pid=\"";
    s += tmp;
    s += "\" type=\"" + type + "\"/>\n";
    return s;
}


string XMLgen::RegisteredDevice(string name, string * comment = NULL, 
	int * curc = NULL, int * maxc = NULL, 
	string * starttime = NULL)
	    // <DEVICE name="[name]" comment="[comment]" current_counter 
	    //  ="[curc]" max_counter="[maxc]" start_time="[starttime]"/>
{
#define __TMP_SIZE 80
    char tmp[__TMP_SIZE];
    string s = "<DEVICE name=\"" + name;
    if (comment) s += "\" comment=\"" + *comment;
    if (curc) {
	snprintf(tmp, __TMP_SIZE, "%d", *curc);
	s += "\" curent_counter=\"";
	s += tmp;
    }
    if (maxc) {
	snprintf(tmp, __TMP_SIZE, "%d", *maxc);
	s += "\" max_counter=\"";
	s += tmp;    
    }
    if (starttime) s += "\" start_time=\"" + *starttime;    
    s += "\"/>\n";
    return s;
}

string XMLgen::Users(string list)
{
    string s = "<USERS_LIST>\n" + list + "</USERS_LIST>\n";
    return s;
}

string XMLgen::User(string name, string descr = "")
{
    string s = "<USER name=\"" + name;
    if (descr != "")
	s += "\" description=\"" + descr;
    s += "\"/>\n";
    return s;
}

string XMLgen::Result(string status, string descr)
{
    string s = "";
    s += "<RESULT " + Attribute("status", status) + " " + Attribute("description", descr) + "/>\n";
    return s;
}

string XMLgen::Device_list_working_res(string list)
{
    string s = "";
    s += "<DEVICE_LIST_WORKING_RES>\n" + list + "</DEVICE_LIST_WORKING_RES>";
    return s;
}

string XMLgen::Device_list_registered_res(string list)
{
    string s = "";
    s += "<DEVICE_LIST_REGISTERED_RES>\n" + list + "</DEVICE_LIST_REGISTERED_RES>";
    return s;
}

string XMLgen::Admin_threads_res(string list)
{
    string s = "";
    s += "<ADMIN_THREADS_RES>\n" + list + "</ADMIN_THREADS_RES>";
    return s;
}

string XMLgen::TaggedResult(string tag, string status, string descr)
{
    string s = "";
    s += "<" + tag + ">\n" + XMLgen::Result(status, descr) + "</" + tag + ">";
    return s;
}

