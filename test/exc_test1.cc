#include <iostream>
#include "../src/ccerror.h"

main()
{

try {
	throw exc_CD_Tree_UnresolvedAlias("ala ma kotka");
}
catch (exc_CD_Tree_UnresolvedAlias &exc) {
	cout << "Got it !";
	cout << exc;
}
catch (exc_Codecook &exc) {
	cout << exc;
}

}
