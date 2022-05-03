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

	tree.InsertNode("ciep:praterm:t1", &root3);
	tree.InsertNode("ciep:praterm:t2", &root2);
	tree.InsertNode("raports:praterm2:t3", &root);

	CD_Node_Alias alias1(tree, "ciep");
	try {
		tree.InsertNode("raports:praterm", &alias1);
	}
	catch (exc_Codecook &exc) {
		cout << exc << endl;
	}

	CD_Node_Alias alias2(tree, "raports:praterm");
	tree.InsertNode("raports:raport_from_praterm", &alias2);

	string s;
	s = "";
	cout << "Scanning:" << endl;
	tree.ScanAllNodes("raports", s);
	cout << s << endl;

	s = "";
	cout << "Scanning:" << endl;
	tree.ScanAllNodes("raports", s);
	cout << s << endl;

	s = "";
	cout << "Scanning:" << endl;
	tree.ScanNodes("raports", "EMPTY", s);
	cout << s << endl;

	s = "";
	cout << "Scanning:" << endl;
	tree.ScanNodes("raports", "ROOT", s);
	cout << s << endl;

	s = "";
	cout << "Scanning:" << endl;
	tree.ScanAllNodes("raports", s);
	cout << s << endl;
	
}
