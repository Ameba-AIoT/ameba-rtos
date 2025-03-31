#ifndef MAIN_H
#define MAIN_H

void wififw_task_create(void);
void wifi_init(void);
int mbedtls_platform_set_calloc_free(
	void *(*calloc_func)(unsigned int, unsigned int),
	void (*free_func)(void *)
);
int rt_kv_init(void);

#endif
