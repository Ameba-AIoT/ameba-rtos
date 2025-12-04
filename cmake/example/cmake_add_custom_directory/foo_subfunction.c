#include "foo_subfunction.h"

void sub_funcA(void)
{
	printf("This is subfunction-A of func_foo!\n");
}

#ifdef EN_FOO_FUNCB
void sub_funcB(void)
{
	printf("This is subfunction-B of func_foo!\n");
}
#endif