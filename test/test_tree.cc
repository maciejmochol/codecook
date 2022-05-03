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

	root.InsertNode("root2", &root2);
	root.InsertNode("root3", &root3);
	root2.InsertNode("subroot1", &subroot1);

//	cout << root << endl;
	cout << root.ShowSubNodes("");
}
