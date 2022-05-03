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

	tree.InsertNode("a:a:a", &root3);
	tree.InsertNode("a:a", &root2);

	CD_Node_Alias alias1(tree, "a");
	tree.InsertNode("alias1", &alias1);

	CD_Node_Alias alias2(tree, "alias1");
	tree.InsertNode("a:a:alias2", &alias2);

	tree.InsertNode("a", &root);
	try {
		tree.InsertNode("a:a:a", &root3);
	}
	catch (exc_Codecook &exc) {
		cout << exc << endl;
	}

	string s;
	tree.ShowNodes(s);
	cout << s;
	
	try {
		cout << tree.GetNode("a:a:alias2:a:alias2:a:alias2:huj") << endl;
	}
	catch (exc_CD_Tree_Alias &exc) {
		cout << exc;
	}
}
