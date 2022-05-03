#include "thread.h"
#include "cdtree.h"
#include <string>

int main(void)
{
	int i, j, k, l;
	CD_Node_Root root;
	CD_Node_Root *sub_i, *sub_j, *sub_k, *sub_l;
	string s;
	char ss[100];

	for (i = 0; i < 10; i++) {
		sub_i = new CD_Node_Root();
		sprintf(ss, "%d", i);
		s = ss;

		root.InsertNode(s, sub_i);
		cout << "added " << s << endl;
		
		for (j = 0; j < 10; j++) {
			sub_j = new CD_Node_Root();
			sprintf(ss, "%d", j);
			s = ss;
			sub_i->InsertNode(s, sub_j);
		
			for (k = 0; k < 10; k++) {
				sub_k = new CD_Node_Root();
				sprintf(ss, "%d", k);
				s = ss;

				sub_j->InsertNode(s, sub_k);
		
				for (l = 0; l < 10; l++) {
					sub_l = new CD_Node_Root();
					sprintf(ss, "%d", l);
					s = ss;

					sub_k->InsertNode(s, sub_l);
				}
			}
		}
	}

	cout << "Showing..." << endl;
	string _s;
	root.ShowSubNodes(_s);

}
