#include "thread.h"
#include "ptable.h"

Param_Tables *pt;

void * process(void * arg)
{
  int i;
  int h1, h2, h3;
  Device_Param_Table *dpt;

  dpt = pt->Lock_Table_To_Write("a1");
  h1 = dpt->AddInt();
  h1 = dpt->AddInt();
  h1 = dpt->AddInt();
  pt->Unlock_Table_From_Write("a1");


  for (i = 0; i < 100; i++) {
    dpt = pt->Lock_Table_To_Aquisition("a1");
    dpt->Set(0, &i);
    dpt->Set(1, &i);
    dpt->Set(2, &i);
	sleep(1);
	
try {
//      h1 = dpt->AddInt();
}
catch (exc_Device_Param_Table_ShouldBeWriter exc) {
	cout << exc;
}

cout << "Yeah" << endl;

    pt->Unlock_Table_From_Aquisition("a1");

    cout << "#" << i << "# " << endl;
  }
  return NULL;
}

void *get(void * arg)
{
	Param_Rec param(INT_VALUE);
  	Device_Param_Table *dpt;

	int i;

	for (i = 0; i < 100; i++) {
		sleep(2);
		dpt = pt->Lock_Table_To_Aquisition("a1");
		try {
			dpt->Get(i, param);
		}
		catch ( exc_Device_Param_Table_BadHandle ) {
			cout << "Zly uchwyt !" << endl;
		};
		cout << i << ": " << (int) param.data << " " << dpt->GetCounter() << endl;
		pt->Unlock_Table_From_Aquisition("a1");
	};
	return NULL;
}

int main(void)
{
  int retcode;
  pthread_t th_a, th_b, th_c;
  void * retval;

	pt = new Param_Tables();
	pt->Create_Table("a1");

	try {
		pt->Create_Table("a1");
	}
	catch (exc_Param_Tables &exc) {
		cout << exc;
	}


  retcode = pthread_create(&th_a, NULL, process, NULL);
  if (retcode != 0) fprintf(stderr, "create a failed %d\n", retcode);
  retcode = pthread_create(&th_b, NULL, process, NULL);
  if (retcode != 0) fprintf(stderr, "create b failed %d\n", retcode);
  retcode = pthread_create(&th_c, NULL, get, NULL);
  if (retcode != 0) fprintf(stderr, "create c failed %d\n", retcode);
  retcode = pthread_join(th_a, &retval);
  if (retcode != 0) fprintf(stderr, "join a failed %d\n", retcode);
  retcode = pthread_join(th_b, &retval);
  if (retcode != 0) fprintf(stderr, "join b failed %d\n", retcode);
  retcode = pthread_join(th_c, &retval);
  if (retcode != 0) fprintf(stderr, "join c failed %d\n", retcode);
  return 0;
}
