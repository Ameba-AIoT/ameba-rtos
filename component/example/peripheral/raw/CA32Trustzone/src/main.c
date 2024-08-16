/*
 * Copyright (C) 2020 Realtek Semiconductor Corp.  All Rights Reserved.
 */

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Scheduler includes. */

#include "ameba_soc.h"
#include "vfs.h"
#include "os_wrapper.h"
#include <stdio.h>

u32 use_hw_crypto_func;

static void *app_mbedtls_calloc_func(size_t nelements, size_t elementSize)
{
	size_t size;
	void *ptr = NULL;
	size = nelements * elementSize;
	ptr = rtos_mem_malloc(size);
	if (ptr) {
		memset(ptr, 0, size);
	}
	return ptr;
}

static void app_mbedtls_free_func(void *buf)
{
	rtos_mem_free(buf);
}

void app_mbedtls_rom_init(void)
{
	mbedtls_platform_set_calloc_free(app_mbedtls_calloc_func, app_mbedtls_free_func);
	use_hw_crypto_func = 0;
	rtl_cryptoEngine_init();
}
/*
 * This function will be replaced when Sdk example is compiled using CMD "make all EXAMPLE=xxx" or "make xip EXAMPLE=xxx"
 * To aviod compile error when example is not compiled
 */
_WEAK void app_pre_example(void)
{


}

_WEAK void app_example(void)
{


}

#if defined(CONFIG_IMQ_EN) && CONFIG_IMQ_EN
extern void ipc_app_init(void);
#elif defined(CONFIG_IMQ2_EN) && CONFIG_IMQ2_EN
extern void ipc2_app_init(void);
#endif
#ifdef CONFIG_WLAN
extern void wlan_initialize(void);
#endif

extern int rt_kv_init(void);

void app_filesystem_init(void)
{
#if defined(CONFIG_AS_INIC_AP)
	int ret = 0;
	vfs_init();
	ret = vfs_user_register(VFS_PREFIX, VFS_LITTLEFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
	if (ret == 0) {
		ret = rt_kv_init();
		if (ret == 0) {
			printf("File System Init Success \n");
			return;
		}
	}

	printf("[err]File System Init Fail \n");
#endif
	return;
}

struct arm_smccc_res {
	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
};

/**
 * struct arm_smccc_quirk - Contains quirk information
 * @id: quirk identification
 * @state: quirk specific information
 * @a6: Qualcomm quirk entry for returning post-smc call contents of a6
 */
struct arm_smccc_quirk {
	int	id;
	union {
		unsigned long a6;
	} state;
};

extern void __arm_smccc_smc(unsigned long a0, unsigned long a1,
							unsigned long a2, unsigned long a3, unsigned long a4,
							unsigned long a5, unsigned long a6, unsigned long a7,
							struct arm_smccc_res *res, struct arm_smccc_quirk *quirk);

#define arm_smccc_smc(...) __arm_smccc_smc(__VA_ARGS__, NULL)


static unsigned long invoke_securetest(unsigned long function_id,
									   unsigned long arg0, unsigned long arg1,
									   unsigned long arg2)
{
	struct arm_smccc_res res;

	arm_smccc_smc(function_id, arg0, arg1, arg2, 0, 0, 0, 0, &res);

	printf("value: %lu %lu %lu\n", res.a0, res.a1, res.a2);

	return res.a0;
}

/*
 * Starts all the other tasks, then starts the scheduler.
 */
int main(void)
{
	InterruptRegister(IPC_INTHandler, IPC_AP_IRQ, (u32)IPCAP_DEV, INT_PRI_MIDDLE);
	InterruptEn(IPC_AP_IRQ, INT_PRI_MIDDLE);

#ifdef CONFIG_MBED_TLS_ENABLED
	app_mbedtls_rom_init();
#endif

	ipc_table_init(IPCAP_DEV);

	/* init console */
	shell_init_rom(0, 0);
	shell_init_ram();

	app_filesystem_init();

	/* pre-processor of application example */
	app_pre_example();

	/* TODO: wifi init*/
#ifdef CONFIG_WLAN
	wlan_initialize();
#endif

#if defined(CONFIG_IMQ_EN) && CONFIG_IMQ_EN
	ipc_app_init();
#elif defined(CONFIG_IMQ2_EN) && CONFIG_IMQ2_EN
	ipc2_app_init();
#endif

	/* Execute application example */
	app_example();

	/* here secure function entry */
	invoke_securetest(0x82000001, 0, 0, 0);

	/* Start the tasks and timer running. */
	printf("Cortex-A Start Scheduler\n");
	rtos_sched_start();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	to be created.  See the memory management section on the FreeRTOS web site
	for more details.  */
	for (;;);
}

