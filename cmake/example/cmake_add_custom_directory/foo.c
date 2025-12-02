#include "foo.h"
#include "foo_subfunction.h"

void func_foo(void)
{
	printf("This is an example of adding a custom directory to Ameba SDK build system!\n");
	sub_funcA();

#ifdef EN_FOO_FUNCB
	sub_funcB();
#endif

}
