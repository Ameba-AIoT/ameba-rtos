#ifndef MAIN_H
#define MAIN_H

int mbedtls_platform_set_calloc_free(
	void *(*calloc_func)(unsigned int, unsigned int),
	void (*free_func)(void *)
);
extern void wifi_FW_init_ram(void);





#endif
