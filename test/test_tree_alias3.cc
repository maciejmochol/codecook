#include "thread.h"
#include "cdtree.h"

int main(void)
{
	CD_Node_Root root;
	CD_Node_Root root2;
	CD_Node_Root root3;
	CD_Node_Root subroot1;
	CD_Tree tree;

	root.AddChannel("ala", "NOVALUE");
	root2.AddChannel("juzek", "NOVALUE");

	tree.InsertNode("praterm:t1", &root3);
	tree.InsertNode("praterm:t2", &root2);

	CD_Node_Alias alias1(tree, "praterm");
	tree.InsertNode("raports:praterm", &alias1);

	CD_Node_Alias alias2(tree, "raports:praterm");
	tree.InsertNode("raportsall:raport_from_praterm", &alias2);

	string s;
	s = "";
	cout << "ALIAS:" << endl;
	tree.SearchNodes(s, "ALIAS");
	cout << s;

	s = "";
	cout << "ROOT:" << endl;
	tree.SearchNodes(s, "ROOT");
	cout << s;

	cout << "raports:praterm channels: " << endl;
	cout << tree.ShowChannels("raports:praterm") << endl;	
}
