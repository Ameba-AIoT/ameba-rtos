
#include "app_syscall.h"

extern uint8_t app_sgot[];
static const char *TAG = "APP_MAIN_2";
uint32_t num = 0x2;

extern void *socket_task(void *param);

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
		count = rand() % 1000;
		APP_LOGI(TAG, "This is a test task: %d\n", count);
		// pthread_cond_timedwait(&cond, &mutex, &tm);
		nanosleep(&tm, NULL);
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
	APP_LOGI(TAG, "%s, num: %d\n", __func__, num);
	num += val;
	APP_LOGI(TAG, "%s, num: %d\n", __func__, num);
}

// dummy_function, dummy_function_2 and the global variables
// are there just to verify if the relocation is actually working
void app_main(void)
{
	APP_LOGI(TAG, "app_main, %d\n", strlen(TAG));
}

const apps_2_struct_t __attribute__((__section__(".apps_struct"))) apps_info = {
	{
		.magic = {'a', 'p', 'p', 'i', 'n', 'f', 'o', '\0'},
		.type = AMOD_TYPE_MOD_TEST_2,   // user define
		.got_base_addr = (uint32_t)app_sgot,
		.set_got_base = &set_got_base,
	},
	// export api
	.test_pthread = &test_pthread,
};
