
#include "app_syscall.h"
#include "kv.h"
#include "vfs.h"
#include "ff.h"
#include "littlefs_adapter.h"

extern uint8_t app_sgot[];
static const char *TAG = "APP_MAIN_1";
uint32_t num = 0x2;
uint32_t x;

extern void *socket_task(void *param);

static const char *TAG1 = "APP_MAIN_1";
static const char *testTab[] = {
	"AAAA",
	"BBBB",
	"CCCC",
	"DDDD",
	NULL,
};

typedef struct {
	int x;
	int y;
	char *z;
} test_t;

static const test_t testTab3[] = {
	{1, 1, "ABCCC"},
	{2, 1, "222"},
	{3, 1, "333"},
	{3, 1, "3334"},
	NULL,
};

typedef struct {
	pthread_mutex_t m_csTimer;
} CTimer;

void test_socket(void)
{
	APP_LOGI(TAG, "%s\n", __func__);

	pthread_t *thread_handle = (pthread_t *)rtos_mem_malloc(sizeof(pthread_t));

	if (!thread_handle) {
		APP_LOGE(TAG, "Falied to allocate thread handle\n");
		return;
	}

	pthread_attr_t *thread_attr = (pthread_attr_t *)rtos_mem_malloc(sizeof(pthread_attr_t));

	if (!thread_attr) {
		APP_LOGE(TAG, "Falied to allocate thread attr\n");
		return;
	}

	pthread_attr_init(thread_attr);
	pthread_attr_setstacksize(thread_attr, 4096);

	APP_LOGI(TAG, "%s task stack %d\n", __func__, thread_attr->usStackSize);

	if (pthread_create(thread_handle, thread_attr, socket_task, NULL)) {
		APP_LOGE(TAG, "Failed in pthread_create\n");
		rtos_mem_free(thread_handle);
		rtos_mem_free(thread_attr);
		return;
	}
	return;
}

void *pthread_task(void *param)
{
	(void)param;

	APP_LOGI(TAG, "This is a test task\n");
	int count = 0;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	struct timespec tm;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	tm.tv_sec = 30;
	tm.tv_nsec = 100000000; //100ms

	while (1) {
		APP_LOGI(TAG, "This is a test task: %d\n", count);
		// pthread_cond_timedwait(&cond, &mutex, &tm);
		nanosleep(&tm, NULL);
		count++;
	}
	return NULL;
}


void test_pthread(void)
{
	APP_LOGI(TAG, "%s\n", __func__);

	pthread_t *thread_handle = (pthread_t *)_malloc_r(NULL, sizeof(pthread_t));

	if (!thread_handle) {
		APP_LOGE(TAG, "Falied to allocate thread handle\n");
		return;
	}

	pthread_attr_t *thread_attr = (pthread_attr_t *)rtos_mem_malloc(sizeof(pthread_attr_t));

	if (!thread_attr) {
		APP_LOGE(TAG, "Falied to allocate thread attr\n");
		return;
	}

	pthread_attr_init(thread_attr);
	pthread_attr_setstacksize(thread_attr, 4096);

	APP_LOGI(TAG, "%s task stack %d\n", __func__, thread_attr->usStackSize);

	if (pthread_create(thread_handle, thread_attr, pthread_task, NULL)) {
		APP_LOGE(TAG, "Failed in pthread_create\n");
		rtos_mem_free(thread_handle);
		rtos_mem_free(thread_attr);
		return;
	}
	return;
}

void test_pass_params(uint32_t val)
{
	// APP_LOGI(TAG, "%s, num: %d\n", __func__, num);
	num += val;
	// APP_LOGI(TAG, "%s, num: %d\n", __func__, num);
}

void InitializeCriticalSection2(pthread_mutex_t *lpCriticalSection)
{
	APP_LOGI(TAG, "111111111\n");
	pthread_mutex_init(lpCriticalSection, NULL);
	APP_LOGI(TAG, "222222222\n");
}

// dummy_function, dummy_function_2 and the global variables
// are there just to verify if the relocation is actually working
void app_main(void)
{
	APP_LOGI(TAG, "app_main, %d\n", strlen(TAG));
	// test_t *t;

	char *ptr;
	for (int i = 0; i < 10; i++) {
		APP_LOGI(TAG, "i = %d\n", i);
		ptr = testTab[i];
		// t = &testTab3[i];
		if (ptr == NULL) {
			break;
		}

		APP_LOGI(TAG, "%s testTab = %s\n", TAG1, ptr);
		// printf("t->z = %s\n", t->z);
	}

	CTimer *me = malloc(sizeof(CTimer));
	InitializeCriticalSection2(&me->m_csTimer);



	int ret = -1;
	char path[MAX_KEY_LENGTH + 1];
	char prefix[] = "vfs";
	char dir[] = "test";

	printf("prefix: %s\n", prefix);
	printf("dir: %s\n", dir);

	snprintf(path, MAX_KEY_LENGTH + 1, "%s:%s", prefix, dir);
	printf("path: %s\n", path);

	ret = mkdir(path, 0);
	if (ret == LFS_ERR_EXIST || ret == -FR_EXIST) {
		printf("KV dir already exist");
		ret = 0;
	}
	x = 1;
	printf("dir make success!, %d\n", x);

	// char filename[] = "ttt";
	// char path2[MAX_KEY_LENGTH + 2];

	// snprintf(path2, MAX_KEY_LENGTH + 2, "%s:%s/%s", prefix, dir, key);
	// FILE *fs = fopen(path, "w");

}

const apps_1_struct_t __attribute__((__section__(".apps_struct"))) apps_info = {
	{
		.magic = {'a', 'p', 'p', 'i', 'n', 'f', 'o', '\0'},
		.type = AMOD_TYPE_MOD_TEST_1,   // user define
		.got_base_addr = (uint32_t)app_sgot,
		.set_got_base = set_got_base,
	},
	// export api
	.test_pthread = &test_pthread,
	.test_socket = &test_socket,
	.test_pass_params = &test_pass_params,
	.app_main = &app_main,
};
