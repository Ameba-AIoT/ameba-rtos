#include "ameba_soc.h"
#include "platform_stdlib.h"

#include "unity.h"
#include "unity_fixture.h"

static void RunAllTests(void){
RUN_TEST_GROUP(Full_POSIX_CLOCK);
	RUN_TEST_GROUP(Full_POSIX_MQUEUE);
	RUN_TEST_GROUP(Full_POSIX_PTHREAD);
	RUN_TEST_GROUP(Full_POSIX_SEMAPHORE);
	RUN_TEST_GROUP(Full_POSIX_STRESS);
	RUN_TEST_GROUP(Full_POSIX_TIMER);
	RUN_TEST_GROUP(Full_POSIX_UNISTD);
	RUN_TEST_GROUP(Full_POSIX_UTILS);
}

int unity_test(u16 argc, u8 *argv[]){
	return UnityMain(argc, argv, RunAllTests);
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE posix_test_cmd_table[] = {
	{
		(const u8 *)"unity_test",  1, unity_test,
		(const u8 *)"\t unity_test\n"
	},
};