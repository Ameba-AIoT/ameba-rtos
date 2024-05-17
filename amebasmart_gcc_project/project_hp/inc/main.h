#ifndef MAIN_H
#define MAIN_H


void wlan_initialize(void);
int mbedtls_platform_set_calloc_free(
	void *(*calloc_func)(unsigned int, unsigned int),
	void (*free_func)(void *)
);
void SystemSetCpuClk(unsigned char CpuClk);


#endif
