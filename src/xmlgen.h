
// Static class XMLgen - generating various XML tokens - just to keep it
// in one place.
// pawel

#ifndef __XMLGEN_H__
#define __XMLGEN_H__

#include <string.h>
#include "command.h"

class XMLgen {
    public:
	static string Message(string message);
	    // <MESSAGE message="[message]"/>
	static string Error(string message);
	    // <ERROR type="unknown" message="[message]"/>
	static string Error(exc_Codecook *exc, string name);
	    // <ERROR type="exception" name="name" code="[exc->id]" 
	    //	throw_from="[exc->throw_from]" name="[exc->name]"/>
	static string Channel(string name, string type);
	    // <CHANNEL name="name" type = "type"/>
	static string Showchannels_res(string node, string channels);
	    // <SHOWCHANNELS_RES node="[node]">
	    // [channels]
	    // </SHOWCHANNELS_RES>
	static string Attribute(string name, string value);
	    // name="value"
	static string Node(string type, string attrs);
	    // <type attrs>
	static string NamedNode(string type, string name);
	    // <[type] name="[name]"/>	    
	static string List_res(string nodes);
	    // <LIST_RES>
	    // [...]
	    // </LIST_RES>
	static string Show_res(string node, string res);
	    // <SHOW_RES node = "node"> res </SHOW_RES>
	static string Set_res(string res);
	    // <SET_RES> res </SHOW_RES>
	static string Alias_res(string res);
	    // <ALIAS_RES> res </ALIAS_RES>
	static string Scan_res(string res);
	    // <SCAN_RES> res </SCAN_RES>
	static string Command(string text);
	    // <COMMAND text="[text]"/>
	static string Command(class Command *cmd);
	    // <COMMAND text="[resolved cmd]"/>
	static string Result(string status, string descr);
	    // <RESULT status="[status]" description="[descr]"/>
	static string TaggedResult(string tag, string status, string descr);
	    // <[tag]>
	    // <RESULT status="[status]" description="[descr]"/>
	    // </[tag]>
	static string Device_list_working_res(string list);
	    // <DEVICE_LIST_WORKING_RES>
	    // [list]
	    // </DEVICE_LIST_WORKING_RES>
	static string Device_list_registered_res(string list);
	    // <DEVICE_LIST_REGISTERED_RES>
	    // [list]
	    // </DEVICE_LIST_REGISTERED_RES>
	static string Admin_threads_res(string list);
	    // <ADMIN_THREADS_RES>
	    // [list]
	    // </ADMIN_THREADS_RES>
	static string Thread(string name, long id, int pid, string type);
	    // <THREAD name="[name]" id="[id]" pid="[pid]" type="[type]"/>


// Unsupported (not included in DTD) - code 06.11.2000
	static string RegisteredDevice(string name, string * comment = NULL, 
		int * curc = NULL, int * maxc = NULL, 
		string * starttime = NULL);
	    // <DEVICE name="[name]" comment="[comment]" current_counter 
	    //  ="[curc]" max_counter="[maxc]" start_time="[starttime]"/>
	static string Users(string list);
	    // <USERS_LIST>
	    // [list]
	    // </USERS_LIST>
	static string User(string name, string descr = "");
	    // <USER name="[name]" description="[descr]"/>
};

#endif
