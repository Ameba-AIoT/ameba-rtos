/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include <atcmd_service.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>
#include <osif.h>

/* Task stack size and priority */
#define BT_ATCMD_TASK_STACK_SIZE  4096
#define BT_ATCMD_TASK_PRIORITY    3

/* Task parameter structures */
typedef struct {
	uint8_t  role;
	uint8_t  enable;
	int      ret;
	void     *sem_handle;
} bt_atcmd_demo_task_param_t;

typedef struct {
	uint8_t  role;
	uint8_t  enable;
	uint32_t channel;
	int      ret;
	void     *sem_handle;
} bt_atcmd_audio_demo_task_param_t;

typedef struct {
	uint8_t  enable;
	uint16_t timeout;
	int      ret;
	void     *sem_handle;
} bt_atcmd_wifimate_demo_task_param_t;

int bt_demo_main(void);
static void bt_demo_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_demo_main();
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_demo(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};

	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_demo_task", bt_demo_task_entry, &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("demo example failed!\r\n");
		return -1;
	}

	BT_LOGA("demo example OK!\r\n");
	return 0;
}

int bt_audio_mp_test_main(uint8_t enable);
static void bt_audio_mp_test_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_audio_mp_test_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_audio_mp_test(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 2) {
		BT_LOGE("Error: wrong value (%d) for bt audio mp test example!\r\n", op);
		return -1;
	}

	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_audio_mp_test_task", bt_audio_mp_test_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("bt audio mp test example failed!\r\n");
		return -1;
	}

	BT_LOGA("bt audio mp test example %s OK!\r\n", action[op]);
	return 0;
}

int ble_central_main(uint8_t enable);
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
static void ble_central_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_central_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}
#endif

int atcmd_bt_central(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
#endif
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for central example!\r\n", op);
		return -1;
	}
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_central_task", ble_central_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("central example failed!\r\n");
		return -1;
	}
#else
	if (ble_central_main(op)) {
		BT_LOGE("Error: central example %s failed!\r\n", action[op]);
		return -1;
	}
#endif
	BT_LOGA("central example %s OK!\r\n", action[op]);
	return 0;
}

int hogp_gamepad_main(uint8_t enable);
static void hogp_gamepad_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = hogp_gamepad_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_hogp_gamepad(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for HOGP example!\r\n", op);
		return -1;
	}

	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_hogp_task", hogp_gamepad_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("HOGP example failed!\r\n");
		return -1;
	}

	BT_LOGA("HOGP example %s OK!\r\n", action[op]);
	return 0;
}

int ble_peripheral_main(uint8_t enable);
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
static void ble_peripheral_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_peripheral_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}
#endif
int atcmd_bt_peripheral(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
#endif
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for peripheral example!\r\n", op);
		return -1;
	}

#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_peripheral_task", ble_peripheral_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("peripheral example failed!\r\n");
		return -1;
	}
#else
	if (ble_peripheral_main(op)) {
		BT_LOGE("Error: peripheral example %s failed!\r\n", action[op]);
		return -1;
	}
#endif
	BT_LOGA("peripheral example %s OK!\r\n", action[op]);
	return 0;
}

int ble_scatternet_main(uint8_t enable);
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
static void ble_scatternet_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_scatternet_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}
#endif

int atcmd_bt_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
#endif
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for scatternet example!\r\n", op);
		return -1;
	}
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_scatternet_task", ble_scatternet_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("scatternet example failed!\r\n");
		return -1;
	}
#else
	if (ble_scatternet_main(op)) {
		BT_LOGE("Error: scatternet example %s failed!\r\n", action[op]);
		return -1;
	}
#endif

	BT_LOGA("scatternet example %s OK!\r\n", action[op]);
	return 0;
}

int atcmd_bt_throughput_test(int argc, char *argv[]);
int ble_throughput_main(uint8_t enable);
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
static void ble_throughput_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_throughput_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}
#endif
int atcmd_bt_throughput(int argc, char *argv[])
{
	uint8_t op;
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
#endif
	char *action[] = {"disable", "enable"};

	if ((strcmp("conn", argv[0]) == 0) || (strcmp("test_start", argv[0]) == 0) ||
		(strcmp("test_stop", argv[0]) == 0)) {
		atcmd_bt_throughput_test(argc, argv);
	} else if ((strcmp("0", argv[0]) == 0) || (strcmp("1", argv[0]) == 0)) {
		op = (uint8_t)str_to_int(argv[0]);
		if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
			BT_LOGE("Error: wrong value (%d) for throughput example!\r\n", op);
			return -1;
		}
#if !defined(CONFIG_RTL8720F) || !CONFIG_RTL8720F
		if (!osif_sem_create(&sem_handle, 0, 1)) {
			BT_LOGE("Error: create sem failed!\r\n");
			return -1;
		}
		task_param.enable = op;
		task_param.sem_handle = sem_handle;
		if (!osif_task_create(&task_handle, "bt_throughput_task", ble_throughput_task_entry,
							  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
			BT_LOGE("Error: create task failed!\r\n");
			osif_sem_delete(sem_handle);
			return -1;
		}
		osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
		osif_sem_delete(sem_handle);
		if (task_param.ret != 0) {
			BT_LOGE("throughput example failed!\r\n");
			return -1;
		}
#else
		if (ble_throughput_main(op)) {
			BT_LOGE("Error: throughput example %s failed!\r\n", action[op]);
			return -1;
		}
#endif
		BT_LOGA("throughput example %s OK!\r\n", action[op]);
	} else {
		BT_LOGE("Input wrong parameters!!!\r\n");
	}
	return 0;
}

int ble_ota_central_main(uint8_t enable);
static void ble_ota_central_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_ota_central_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_ota_central(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for ble ota central example!\r\n", op);
		return -1;
	}

	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_ota_central_task", ble_ota_central_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("ble ota central example failed!\r\n");
		return -1;
	}

	BT_LOGA("ble ota central example %s OK!\r\n", action[op]);
	return 0;
}

int ble_ota_peripheral_main(uint8_t enable);
static void ble_ota_peripheral_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_ota_peripheral_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_ota_peripheral(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for ble ota peripheral example!\r\n", op);
		return -1;
	}

	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_ota_peripheral_task", ble_ota_peripheral_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("ble ota peripheral example failed!\r\n");
		return -1;
	}

	BT_LOGA("ble ota peripheral example %s OK!\r\n", action[op]);
	return 0;
}

int ble_mesh_provisioner_main(uint8_t enable);
static void ble_mesh_provisioner_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_mesh_provisioner_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_mesh_provisioner(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		BT_LOGE("Error: wrong value (%d) for mesh provisioner example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_mesh_prov_task", ble_mesh_provisioner_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("mesh provisioner example failed!\r\n");
		return -1;
	}

	BT_LOGA("mesh provisioner example %s OK!\r\n", action[op]);
	return 0;
}

int ble_mesh_device_main(uint8_t enable);
static void ble_mesh_device_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_mesh_device_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_mesh_device(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		BT_LOGE("Error: wrong value (%d) for mesh device example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_mesh_dev_task", ble_mesh_device_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("mesh device example failed!\r\n");
		return -1;
	}

	BT_LOGA("mesh device example %s OK!\r\n", action[op]);
	return 0;
}

int atcmd_bt_mesh_performence_test(int argc, char *argv[]);
int ble_mesh_provisioner_test_main(uint8_t enable);
static void ble_mesh_provisioner_test_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_mesh_provisioner_test_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_mesh_provisioner_test(int argc, char *argv[])
{
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (!strcmp("start", argv[0]) || !strcmp("result", argv[0]) || !strcmp("cmd", argv[0])) {
		atcmd_bt_mesh_performence_test(argc, argv);
	} else if ((uint8_t)str_to_int(argv[0]) == 0 || (uint8_t)str_to_int(argv[0]) == 1) {
		op = (uint8_t)str_to_int(argv[0]);
		if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
			BT_LOGE("Error: wrong value (%d) for mesh provisioner example!\r\n", op);
			return -1;
		}
		if (!osif_sem_create(&sem_handle, 0, 1)) {
			BT_LOGE("Error: create sem failed!\r\n");
			return -1;
		}
		task_param.enable = op;
		task_param.sem_handle = sem_handle;
		if (!osif_task_create(&task_handle, "bt_mesh_prov_test_task", ble_mesh_provisioner_test_task_entry,
							  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
			BT_LOGE("Error: create task failed!\r\n");
			osif_sem_delete(sem_handle);
			return -1;
		}
		osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
		osif_sem_delete(sem_handle);
		if (task_param.ret != 0) {
			BT_LOGE("mesh provisioner test example failed!\r\n");
			return -1;
		}

		BT_LOGA("mesh provisioner test example %s OK!\r\n", action[op]);
	} else {
		BT_LOGE("Input wrong parameters!!!\r\n");
	}
	return 0;
}

int ble_mesh_device_test_main(uint8_t enable);
static void ble_mesh_device_test_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_mesh_device_test_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_mesh_device_test(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		BT_LOGE("Error: wrong value (%d) for mesh device example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_mesh_dev_test_task", ble_mesh_device_test_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("mesh device test example failed!\r\n");
		return -1;
	}

	BT_LOGA("mesh device test example %s OK!\r\n", action[op]);
	return 0;
}

int ble_mesh_provisioner_scatternet_main(uint8_t enable);
static void ble_mesh_provisioner_scatternet_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_mesh_provisioner_scatternet_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_mesh_provisioner_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		BT_LOGE("Error: wrong value (%d) for mesh provisioner scatternet example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_mesh_prov_scatternet_task", ble_mesh_provisioner_scatternet_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("mesh provisioner scatternet example failed!\r\n");
		return -1;
	}

	BT_LOGA("mesh provisioner scatternet example %s OK!\r\n", action[op]);
	return 0;
}

int ble_mesh_device_scatternet_main(uint8_t enable);
static void ble_mesh_device_scatternet_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_mesh_device_scatternet_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_mesh_device_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		BT_LOGE("Error: wrong value (%d) for mesh device scatternet example!\r\n", op);
		return -1;
	}

	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_mesh_dev_scatternet_task", ble_mesh_device_scatternet_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("mesh device scatternet example failed!\r\n");
		return -1;
	}

	BT_LOGA("mesh device scatternet example %s OK!\r\n", action[op]);
	return 0;
}

int bt_a2dp_main(uint8_t role, uint8_t enable);
static void bt_a2dp_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_a2dp_main(p->role, p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_a2dp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 1;
		BT_LOGA("Set a2dp role (snk)\r\n");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 0;
		BT_LOGA("Set a2dp role (src)\r\n");
	} else {
		BT_LOGE("Invalid role set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for a2dp example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_a2dp_task", bt_a2dp_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("a2dp example failed!\r\n");
		return -1;
	}

	BT_LOGA("a2dp example %s OK!\r\n", action[op]);
	return 0;
}

int bt_a2dp_scatternet_main(uint8_t role, uint8_t enable);
static void bt_a2dp_scatternet_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_a2dp_scatternet_main(p->role, p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_a2dp_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 1;
		BT_LOGA("Set a2dp role (snk)\r\n");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 0;
		BT_LOGA("Set a2dp role (src)\r\n");
	} else {
		BT_LOGE("Invalid role set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for a2dp scatternet example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_a2dp_scatternet_task", bt_a2dp_scatternet_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("a2dp scatternet example failed!\r\n");
		return -1;
	}

	BT_LOGA("a2dp scatternet example %s OK!\r\n", action[op]);
	return 0;
}

int bt_a2dp_sink_pbp_source_main(uint8_t enable);
static void bt_a2dp_pbp_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_a2dp_sink_pbp_source_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_a2dp_pbp(int argc, char *argv[])
{
	(void)argc;
	uint8_t op = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)(str_to_int(argv[0]))) > 2) {
		BT_LOGE("Error: wrong value (%d) for a2dp pbp example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_a2dp_pbp_task", bt_a2dp_pbp_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("a2dp pbp example failed!\r\n");
		return -1;
	}

	BT_LOGA("a2dp pbp example %s OK!\r\n", action[op]);
	return 0;
}

int bt_a2dp_hfp_pbp_main(uint8_t enable);
static void bt_a2dp_hfp_pbp_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_a2dp_hfp_pbp_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_a2dp_hfp_pbp(int argc, char *argv[])
{
	(void)argc;
	uint8_t op = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)(str_to_int(argv[0]))) > 2) {
		BT_LOGE("Error: wrong value (%d) for a2dp hfp pbp example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_a2dp_hfp_pbp_task", bt_a2dp_hfp_pbp_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("a2dp hfp pbp example failed!\r\n");
		return -1;
	}

	BT_LOGA("a2dp hfp pbp example %s OK!\r\n", action[op]);
	return 0;
}

int bt_a2dp_hfp_pbp_peripheral_main(uint8_t enable);
static void bt_a2dp_hfp_pbp_peripheral_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_a2dp_hfp_pbp_peripheral_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_a2dp_hfp_pbp_peripheral(int argc, char *argv[])
{
	(void)argc;
	uint8_t op = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)(str_to_int(argv[0]))) > 2) {
		BT_LOGE("Error: wrong value (%d) for a2dp hfp pbp peripheral example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_a2dp_hfp_pbp_periph_task", bt_a2dp_hfp_pbp_peripheral_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("a2dp hfp pbp peripheral example failed!\r\n");
		return -1;
	}

	BT_LOGA("a2dp hfp pbp peripheral example %s OK!\r\n", action[op]);
	return 0;
}

int bt_a2dp_sink_tmap_main(uint8_t enable, uint8_t role);
static void bt_a2dp_tmap_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_a2dp_sink_tmap_main(p->enable, p->role);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_a2dp_tmap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role = 0;
	uint8_t op = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "ums") == 0) {
		role = 0x04;
		BT_LOGA("Set a2dp sink ums role\r\n");
	} else if (strcmp(argv[0], "bms") == 0) {
		role = 0x10;
		BT_LOGA("Set a2dp sink bms role\r\n");
	} else {
		BT_LOGE("Invalid role set\r\n");
		return -1;
	}

	if ((op = (uint8_t)(str_to_int(argv[1]))) > 2) {
		BT_LOGE("Error: wrong value (%d) for a2dp sink tmap example!\r\n", op);
		return -1;
	}

	if (role == 0x04 || role == 0x10) {
		if (!osif_sem_create(&sem_handle, 0, 1)) {
			BT_LOGE("Error: create sem failed!\r\n");
			return -1;
		}
		task_param.role = role;
		task_param.enable = op;
		task_param.sem_handle = sem_handle;
		if (!osif_task_create(&task_handle, "bt_a2dp_tmap_task", bt_a2dp_tmap_task_entry,
							  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
			BT_LOGE("Error: create task failed!\r\n");
			osif_sem_delete(sem_handle);
			return -1;
		}
		osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
		osif_sem_delete(sem_handle);
		if (task_param.ret != 0) {
			BT_LOGE("a2dp sink tmap example failed!\r\n");
			return -1;
		}
		BT_LOGA("a2dp sink tmap example %s OK!\r\n", action[op]);
	}

	return 0;
}

int bt_hfp_main(uint8_t role, uint8_t enable);
static void bt_hfp_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_hfp_main(p->role, p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_hfp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "hf") == 0) {
		role = 1;
		BT_LOGA("Set hfp role (hand free)\r\n");
	} else if (strcmp(argv[0], "ag") == 0) {
		role = 0;
		BT_LOGA("Set hfp role (audio gate)\r\n");
	} else {
		BT_LOGE("Invalid role set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for hfp example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_hfp_task", bt_hfp_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("hfp example failed!\r\n");
		return -1;
	}

	BT_LOGA("hfp example %s OK!\r\n", action[op]);
	return 0;
}

int bt_audio_a2dp_hfp_main(uint8_t role, uint8_t enable);
static void bt_a2dp_hfp_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_audio_a2dp_hfp_main(p->role, p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_a2dp_hfp(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	uint8_t role;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 1;
		BT_LOGA("Set a2dp role (snk) hfp role (hf)\r\n");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 0;
		BT_LOGA("Set a2dp role (src) hfp role (ag)\r\n");
	} else {
		BT_LOGE("Invalid role set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for a2dp hfp example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_a2dp_hfp_task", bt_a2dp_hfp_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("a2dp hfp example failed!\r\n");
		return -1;
	}

	BT_LOGA("a2dp hfp example %s OK!\r\n", action[op]);
	return 0;
}

int bt_spp_main(uint8_t role, uint8_t enable);
static void bt_spp_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_spp_main(p->role, p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_spp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "client") == 0) {
		role = 0;
		BT_LOGA("Set spp role (client)\r\n");
	} else if (strcmp(argv[0], "server") == 0) {
		role = 1;
		BT_LOGA("Set spp role (server)\r\n");
	} else {
		BT_LOGE("Invalid role set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for spp!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_spp_task", bt_spp_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("spp example failed!\r\n");
		return -1;
	}

	BT_LOGA("spp example %s OK!\r\n", action[op]);
	return 0;
}

int bt_rfc_main(uint8_t enable);
static void bt_rfc_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_rfc_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_rfc(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 2) {
		BT_LOGE("Error: wrong value (%d) for spp!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_rfc_task", bt_rfc_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("rfc example failed!\r\n");
		return -1;
	}

	BT_LOGA("rfc example %s OK!\r\n", action[op]);
	return 0;
}

int bt_hid_main(uint8_t role, uint8_t enable);
static void bt_hid_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_hid_main(p->role, p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_hid(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "gamepad") == 0) {
		role = 0;
		BT_LOGA("Set hid role (gamepad)\r\n");
	} else if (strcmp(argv[0], "mouse") == 0) {
		role = 1;
		BT_LOGA("Set hid role (mouse)\r\n");
	} else {
		BT_LOGE("Invalid role set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for hid example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_hid_task", bt_hid_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("hid example failed!\r\n");
		return -1;
	}

	BT_LOGA("hid example %s OK!\r\n", action[op]);
	return 0;
}

int bt_le_iso_main(uint8_t role, uint8_t enable);
static void bt_le_iso_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_le_iso_main(p->role, p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_ble_iso(int argc, char **argv)
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "bis") == 0) {
		if (strcmp(argv[1], "broadcaster") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR */
			role = 0x03;
			BT_LOGA("Set bis broadcaster\r\n");
		} else if (strcmp(argv[1], "receiver") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK */
			role = 0x04;
			BT_LOGA("Set bis receiver\r\n");
		} else {
			BT_LOGE("Invalid bis role set\r\n");
			return -1;
		}
	} else if (strcmp(argv[0], "cis") == 0) {
		if (strcmp(argv[1], "initiator") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI */
			role = 0x01;
			BT_LOGA("Set cis initiator\r\n");
		} else if (strcmp(argv[1], "acceptor") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER */
			role = 0x02;
			BT_LOGA("Set cis acceptor\r\n");
		} else {
			BT_LOGE("Invalid cis role set\r\n");
			return -1;
		}
	} else {
		BT_LOGE("Invalid iso cis / bis set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[2])) > 2) {
		BT_LOGE("Error: wrong value (%d) for iso example!\r\n", op);
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_le_iso_task", bt_le_iso_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("iso example failed!\r\n");
		return -1;
	}

	BT_LOGA("iso example %s OK!\r\n", action[op]);
	return 0;
}

int bt_le_audio_generic_demo_main(uint8_t role, uint8_t enable, uint32_t sound_channel);
static void bt_le_audio_generic_demo_task_entry(void *param)
{
	bt_atcmd_audio_demo_task_param_t *p = (bt_atcmd_audio_demo_task_param_t *)param;
	p->ret = bt_le_audio_generic_demo_main(p->role, p->enable, p->channel);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_le_audio_generic_demo(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	uint32_t channel = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_audio_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "acceptor") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR */
		role = 0x01;
		BT_LOGA("Set cap acceptor\r\n");
	} else if (strcmp(argv[0], "initiator") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR */
		role = 0x02;
		BT_LOGA("Set cap initiator\r\n");
	} else if (strcmp(argv[0], "commander") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER */
		role = 0x04;
		BT_LOGA("Set cap commander\r\n");
	} else {
		BT_LOGE("Invalid unicast role set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for cap example!\r\n", op);
		return -1;
	}
	if (argc == 3) {
		if (strcmp(argv[2], "left") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FL */
			channel = 0x01;
			BT_LOGA("Set channel left \r\n");
		} else if (strcmp(argv[2], "right") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FR */
			channel = 0x02;
			BT_LOGA("Set channel right \r\n");
		} else if (strcmp(argv[2], "stereo") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR */
			channel = 0x03;
			BT_LOGA("Set channel stereo \r\n");
		} else {
			BT_LOGE("Error: cap example only support left, right and stereo channel!\r\n");
			return -1;
		}
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.channel = channel;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_cap_demo_task", bt_le_audio_generic_demo_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("cap example failed!\r\n");
		return -1;
	}

	BT_LOGA("cap example %s OK!\r\n", action[op]);
	return 0;
}

int bt_pbp_main(uint8_t role, uint8_t enable, uint32_t sound_channel);
static void bt_pbp_task_entry(void *param)
{
	bt_atcmd_audio_demo_task_param_t *p = (bt_atcmd_audio_demo_task_param_t *)param;
	p->ret = bt_pbp_main(p->role, p->enable, p->channel);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_pbp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	uint32_t channel = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_audio_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "source") == 0) {
		/* RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SOURCE */
		role = 0x01;
		BT_LOGA("Set pbp broadcast source\r\n");
	} else if (strcmp(argv[0], "sink") == 0) {
		/* RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK */
		role = 0x02;
		BT_LOGA("Set pbp broadcast sink\r\n");
	} else if (strcmp(argv[0], "assistant") == 0) {
		/* RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT */
		role = 0x04;
		BT_LOGA("Set pbp broadcast assistant\r\n");
	} else {
		BT_LOGE("Invalid pbp role\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for pbp example!\r\n", op);
		return -1;
	}

	if (argc == 3) {
		if (strcmp(argv[2], "left") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FL */
			channel = 0x01;
			BT_LOGA("Set channel left \r\n");
		} else if (strcmp(argv[2], "right") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FR */
			channel = 0x02;
			BT_LOGA("Set channel right \r\n");
		} else if (strcmp(argv[2], "stereo") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR */
			channel = 0x03;
			BT_LOGA("Set channel stereo \r\n");
		} else {
			BT_LOGE("Error: PBP example only support left, right or stereo channel!\r\n");
			return -1;
		}
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.channel = channel;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_pbp_task", bt_pbp_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("pbp example failed!\r\n");
		return -1;
	}

	BT_LOGA("pbp example %s OK!\r\n", action[op]);
	return 0;
}

int bt_tmap_main(uint8_t role, uint8_t enable, uint32_t sound_channel);
static void bt_tmap_task_entry(void *param)
{
	bt_atcmd_audio_demo_task_param_t *p = (bt_atcmd_audio_demo_task_param_t *)param;
	p->ret = bt_tmap_main(p->role, p->enable, p->channel);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_tmap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	uint32_t channel = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_audio_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "cg") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_CALL_GATEWAY_ROLE */
		role = 0x01;
		BT_LOGA("Set tmap Call Gateway\r\n");
	} else if (strcmp(argv[0], "ct") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_CALL_TERMINAL_ROLE */
		role = 0x02;
		BT_LOGA("Set tmap Call Terminal\r\n");
	} else if (strcmp(argv[0], "ums") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_UNICAST_MEDIA_SENDER_ROLE */
		role = 0x04;
		BT_LOGA("Set tmap Unicast Media Sender\r\n");
	} else if (strcmp(argv[0], "umr") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_UNICAST_MEDIA_RECEIVER_ROLE */
		role = 0x08;
		BT_LOGA("Set tmap Unicast Media Receiver\r\n");
	} else if (strcmp(argv[0], "bms") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_BROADCAST_MEDIA_SENDER_ROLE */
		role = 0x10;
		BT_LOGA("Set tmap Broadcast Media Sender\r\n");
	} else if (strcmp(argv[0], "bmr") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_BROADCAST_MEDIA_RECEIVER_ROLE */
		role = 0x20;
		BT_LOGA("Set tmap Broadcast Media Receiver\r\n");
	} else {
		BT_LOGE("Invalid tmap role\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for tmap example!\r\n", op);
		return -1;
	}
	if (argc == 3) {
		if (strcmp(argv[2], "left") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FL */
			channel = 0x01;
			BT_LOGA("Set channel left \r\n");
		} else if (strcmp(argv[2], "right") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FR */
			channel = 0x02;
			BT_LOGA("Set channel right \r\n");
		} else if (strcmp(argv[2], "stereo") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR */
			channel = 0x03;
			BT_LOGA("Set channel stereo \r\n");
		} else {
			BT_LOGE("Error: cap example only support left, right and stereo channel!\r\n");
			return -1;
		}
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.channel = channel;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_tmap_task", bt_tmap_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("tmap example failed!\r\n");
		return -1;
	}

	BT_LOGA("tmap example %s OK!\r\n", action[op]);
	return 0;
}

int bt_gmap_main(uint8_t role, uint8_t enable, uint32_t sound_channel);
static void bt_gmap_task_entry(void *param)
{
	bt_atcmd_audio_demo_task_param_t *p = (bt_atcmd_audio_demo_task_param_t *)param;
	p->ret = bt_gmap_main(p->role, p->enable, p->channel);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_gmap(int argc, char *argv[])
{
	uint8_t role;
	uint8_t op;
	uint32_t channel = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_audio_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "ugg") == 0) {
		/* RTK_BT_LE_AUDIO_GMAP_ROLE_UGG */
		role = 0x01;
		BT_LOGA("Set gmap UGG\r\n");
	} else if (strcmp(argv[0], "ugt") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_UGT */
		role = 0x02;
		BT_LOGA("Set gmap UGT\r\n");
	} else if (strcmp(argv[0], "bgs") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_BGS */
		role = 0x04;
		BT_LOGA("Set gmap BGS\r\n");
	} else if (strcmp(argv[0], "bgr") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_BGR */
		role = 0x08;
		BT_LOGA("Set gmap BGR\r\n");
	} else {
		BT_LOGE("Invalid GMAP role set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for gmap example!\r\n", op);
		return -1;
	}
	if (argc == 3) {
		if (strcmp(argv[2], "left") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FL */
			channel = 0x01;
			BT_LOGA("Set channel left \r\n");
		} else if (strcmp(argv[2], "right") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FR */
			channel = 0x02;
			BT_LOGA("Set channel right \r\n");
		} else if (strcmp(argv[2], "stereo") == 0) {
			/* RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR */
			channel = 0x03;
			BT_LOGA("Set channel stereo \r\n");
		} else {
			BT_LOGE("Error: cap example only support left, right and stereo channel!\r\n");
			return -1;
		}
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = role;
	task_param.enable = op;
	task_param.channel = channel;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_gmap_task", bt_gmap_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("gmap example failed!\r\n");
		return -1;
	}

	BT_LOGA("gmap example %s OK!\r\n", action[op]);
	return 0;
}

int le_audio_generic_demo_peripheral(uint8_t enable, uint32_t sound_channel);
static void le_audio_generic_demo_peripheral_task_entry(void *param)
{
	bt_atcmd_audio_demo_task_param_t *p = (bt_atcmd_audio_demo_task_param_t *)param;
	p->ret = le_audio_generic_demo_peripheral(p->enable, p->channel);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_le_audio_generic_demo_peripheral(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	uint32_t channel = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_audio_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 2) {
		BT_LOGE("Error: wrong value (%d) for gmap example!\r\n", op);
		return -1;
	}

	if (strcmp(argv[1], "left") == 0) {
		/* RTK_BT_LE_AUDIO_LOCATION_FL */
		channel = 0x01;
		BT_LOGA("Set channel left \r\n");
	} else if (strcmp(argv[1], "right") == 0) {
		/* RTK_BT_LE_AUDIO_LOCATION_FR */
		channel = 0x02;
		BT_LOGA("Set channel right \r\n");
	} else if (strcmp(argv[1], "stereo") == 0) {
		/* RTK_BT_LE_AUDIO_LOCATION_FL | RTK_BT_LE_AUDIO_LOCATION_FR */
		channel = 0x03;
		BT_LOGA("Set channel stereo \r\n");
	} else {
		BT_LOGE("Error: cap example only support left, right and stereo channel!\r\n");
		return -1;
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.role = 0;
	task_param.enable = op;
	task_param.channel = channel;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_le_audio_periph_task", le_audio_generic_demo_peripheral_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("cap acceptor peripheral example failed!\r\n");
		return -1;
	}

	BT_LOGA("cap acceptor peripheral example %s OK!\r\n", action[op]);
	return 0;
}

int atcmd_bt_pts_cmd(int argc, char *argv[]);
int bt_pts_main(uint8_t enable);
static void bt_pts_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_pts_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_pts(int argc, char *argv[])
{
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if (!strcmp("adv_data", argv[0])) {
		atcmd_bt_pts_cmd(argc, argv);
	} else if ((uint8_t)str_to_int(argv[0]) == 1 || (uint8_t)str_to_int(argv[0]) == 0) {
		op = (uint8_t)str_to_int(argv[0]) > 2 ? 2 : (uint8_t)str_to_int(argv[0]);
		if (!osif_sem_create(&sem_handle, 0, 1)) {
			BT_LOGE("Error: create sem failed!\r\n");
			return -1;
		}
		task_param.enable = op;
		task_param.sem_handle = sem_handle;
		if (!osif_task_create(&task_handle, "bt_pts_task", bt_pts_task_entry,
							  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
			BT_LOGE("Error: create task failed!\r\n");
			osif_sem_delete(sem_handle);
			return -1;
		}
		osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
		osif_sem_delete(sem_handle);
		if (task_param.ret != 0) {
			BT_LOGE("pts example failed!\r\n");
			return -1;
		}
		BT_LOGA("pts example %s OK!\r\n", action[op]);
	} else {
		BT_LOGE("Error: wrong pts atcmd!\r\n");
	}
	return 0;
}

int ble_transfer_module_main(uint8_t enable);
int atcmd_bt_transfer_module_cmd(int argc, char *argv[]);
static void ble_transfer_module_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_transfer_module_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_transfer_module(int argc, char *argv[])
{
	int ret = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};

	if ((strcmp("0", argv[0]) == 0) || (strcmp("1", argv[0]) == 0)) {
		uint8_t op = (uint8_t)str_to_int(argv[0]);
		if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
			BT_LOGE("Error: wrong parameter (%d) for transfer module example!\r\n", op);
			return BT_AT_ERR_PARAM_INVALID;
		}
		if (!osif_sem_create(&sem_handle, 0, 1)) {
			BT_LOGE("Error: create sem failed!\r\n");
			return BT_AT_FAIL;
		}
		task_param.enable = op;
		task_param.sem_handle = sem_handle;
		if (!osif_task_create(&task_handle, "bt_transfer_module_task", ble_transfer_module_task_entry,
							  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
			BT_LOGE("Error: create task failed!\r\n");
			osif_sem_delete(sem_handle);
			return BT_AT_FAIL;
		}
		osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
		osif_sem_delete(sem_handle);
		if (task_param.ret != 0) {
			BT_LOGE("transfer module example failed!\r\n");
			return BT_AT_FAIL;
		}

		BT_LOGA("transfer module example %s OK!\r\n", (op == 1) ? "enable" : "disable");
	} else {
		ret = atcmd_bt_transfer_module_cmd(argc, argv);
	}
	return ret;
}

int ble_wifimate_device_main(uint8_t enable, uint16_t timeout);
static void ble_wifimate_device_task_entry(void *param)
{
	bt_atcmd_wifimate_demo_task_param_t *p = (bt_atcmd_wifimate_demo_task_param_t *)param;
	p->ret = ble_wifimate_device_main(p->enable, p->timeout);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_wifimate_device(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_wifimate_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};
	uint16_t timeout = 60;

	if (argc < 1 || argc > 2) {
		BT_LOGE("Error: wrong parameter number\r\n");
		return BT_AT_ERR_PARAM_INVALID;
	}

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for ble wifimate device example!\r\n", op);
		return BT_AT_ERR_PARAM_INVALID;
	}

	if (argc > 1) {
		timeout = (uint16_t)str_to_int(argv[1]);
		BT_LOGA("Ble wifimate timeout=%d\r\n", timeout);
	}
	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return BT_AT_FAIL;
	}
	task_param.enable = op;
	task_param.timeout = timeout;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_wifimate_dev_task", ble_wifimate_device_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return BT_AT_FAIL;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("Ble wifimate device example failed!\r\n");
		return BT_AT_FAIL;
	}

	BT_LOGA("Ble wifimate device example %s OK!\r\n", action[op]);
	return 0;
}

int ble_wifimate_configurator_main(uint8_t enable);
int atcmd_bt_wifimate_configurator_cmd(int argc, char *argv[]);
static void ble_wifimate_configurator_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = ble_wifimate_configurator_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_wifimate_configurator(int argc, char *argv[])
{
	int ret = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};

	if ((strcmp("0", argv[0]) == 0) || (strcmp("1", argv[0]) == 0)) {
		uint8_t op = (uint8_t)str_to_int(argv[0]);
		if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
			BT_LOGE("Error: wrong parameter (%d) for ble wifimate configurator example!\r\n", op);
			return BT_AT_ERR_PARAM_INVALID;
		}
		if (!osif_sem_create(&sem_handle, 0, 1)) {
			BT_LOGE("Error: create sem failed!\r\n");
			return BT_AT_FAIL;
		}
		task_param.enable = op;
		task_param.sem_handle = sem_handle;
		if (!osif_task_create(&task_handle, "bt_wifimate_cfg_task", ble_wifimate_configurator_task_entry,
							  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
			BT_LOGE("Error: create task failed!\r\n");
			osif_sem_delete(sem_handle);
			return BT_AT_FAIL;
		}
		osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
		osif_sem_delete(sem_handle);
		if (task_param.ret != 0) {
			BT_LOGE("ble wifimate configurator example failed!\r\n");
			return BT_AT_FAIL;
		}

		BT_LOGA("ble wifimate configurator example %s OK!\r\n", (op == 1) ? "enable" : "disable");
	} else {
		ret = atcmd_bt_wifimate_configurator_cmd(argc, argv);
	}
	return ret;
}

#if defined(CONFIG_BT_FUZZ_TEST) && CONFIG_BT_FUZZ_TEST
extern int bt_fuzz_test_main(uint8_t enable, uint8_t role);
extern int atcmd_bt_fuzz_test_cmd(int argc, char *argv[]);
static void bt_fuzz_test_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = bt_fuzz_test_main(p->enable, p->role);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_fuzz_test(int argc, char *argv[])
{
	int ret = 0;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};

	if ((strcmp("0", argv[0]) == 0) || (strcmp("1", argv[0]) == 0)) {
		uint8_t op = (uint8_t)str_to_int(argv[0]);
		if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
			BT_LOGE("Error: wrong parameter (%d) for bt fuzz test example!\r\n", op);
			return BT_AT_ERR_PARAM_INVALID;
		}
		if (!osif_sem_create(&sem_handle, 0, 1)) {
			BT_LOGE("Error: create sem failed!\r\n");
			return BT_AT_FAIL;
		}
		task_param.role = 1;
		task_param.enable = op;
		task_param.sem_handle = sem_handle;
		if (!osif_task_create(&task_handle, "bt_fuzz_test_task", bt_fuzz_test_task_entry,
							  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
			BT_LOGE("Error: create task failed!\r\n");
			osif_sem_delete(sem_handle);
			return BT_AT_FAIL;
		}
		osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
		osif_sem_delete(sem_handle);
		if (task_param.ret != 0) {
			BT_LOGE("bt fuzz test example failed!\r\n");
			return BT_AT_FAIL;
		}

		BT_LOGA("bt fuzz test example %s OK!\r\n", (op == 1) ? "enable" : "disable");
	} else {
		ret = atcmd_bt_fuzz_test_cmd(argc, argv);
	}
	return ret;
}
#endif

int hogp_ull_device_main(uint8_t enable);
static void hogp_ull_device_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = hogp_ull_device_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_hogp_ull_device(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for HOGP ULL device example!\r\n", op);
		return -1;
	}

	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_hogp_ull_dev_task", hogp_ull_device_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("HOGP ULL device example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("HOGP ULL device example %s OK!\r\n", action[op]);
	return 0;
}

int hogp_ull_report_host_main(uint8_t enable);
static void hogp_ull_report_host_task_entry(void *param)
{
	bt_atcmd_demo_task_param_t *p = (bt_atcmd_demo_task_param_t *)param;
	p->ret = hogp_ull_report_host_main(p->enable);
	osif_sem_give(p->sem_handle);
	osif_task_delete(NULL);
}

int atcmd_bt_hogp_ull_report_host(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	void *task_handle = NULL;
	void *sem_handle = NULL;
	bt_atcmd_demo_task_param_t task_param = {0};
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for HOGP ULL report host example!\r\n", op);
		return -1;
	}

	if (!osif_sem_create(&sem_handle, 0, 1)) {
		BT_LOGE("Error: create sem failed!\r\n");
		return -1;
	}
	task_param.enable = op;
	task_param.sem_handle = sem_handle;
	if (!osif_task_create(&task_handle, "bt_hogp_ull_host_task", hogp_ull_report_host_task_entry,
						  &task_param, BT_ATCMD_TASK_STACK_SIZE, BT_ATCMD_TASK_PRIORITY)) {
		BT_LOGE("Error: create task failed!\r\n");
		osif_sem_delete(sem_handle);
		return -1;
	}
	osif_sem_take(sem_handle, BT_TIMEOUT_FOREVER);
	osif_sem_delete(sem_handle);
	if (task_param.ret != 0) {
		BT_LOGE("HOGP ULL report host example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("HOGP ULL report host example %s OK!\r\n", action[op]);
	return 0;
}

int atcmd_bt_hogp_ull_cmd(int argc, char *argv[]);

int atcmd_bt_hogp_ull(int argc, char *argv[])
{
	return atcmd_bt_hogp_ull_cmd(argc, argv);
}

static const cmd_table_t example_table[] = {
#if defined(CONFIG_BT_AUDIO_MP_TEST) && CONFIG_BT_AUDIO_MP_TEST
	{"bt_audio_mp_test", atcmd_bt_audio_mp_test,    2, 2},
#endif
#if defined(CONFIG_BT_PERIPHERAL) && CONFIG_BT_PERIPHERAL
	{"peripheral",       atcmd_bt_peripheral,       2, 2},
#endif
#if defined(CONFIG_BT_HOGP) && CONFIG_BT_HOGP
	{"hogp",       atcmd_bt_hogp_gamepad,           2, 2},
#endif
#if defined(CONFIG_BT_CENTRAL) && CONFIG_BT_CENTRAL
	{"central",          atcmd_bt_central,          2, 2},
#endif
#if defined(CONFIG_BT_SCATTERNET) && CONFIG_BT_SCATTERNET
	{"scatternet",       atcmd_bt_scatternet,       2, 2},
#endif
#if defined(CONFIG_BT_THROUGHPUT) && CONFIG_BT_THROUGHPUT
	{"throughput",       atcmd_bt_throughput,       2, 13},
#endif
#if defined(CONFIG_BT_OTA_CENTRAL) && CONFIG_BT_OTA_CENTRAL
	{"ota_central",      atcmd_bt_ota_central,      2, 2},
#endif
#if defined(CONFIG_BT_OTA_PERIPHERAL) && CONFIG_BT_OTA_PERIPHERAL
	{"ota_peripheral",   atcmd_bt_ota_peripheral,   2, 2},
#endif
#if defined(CONFIG_BT_MESH_PROVISIONER) && CONFIG_BT_MESH_PROVISIONER
	{"provisioner",      atcmd_bt_mesh_provisioner, 2, 2},
#endif
#if defined(CONFIG_BT_MESH_DEVICE) && CONFIG_BT_MESH_DEVICE
	{"device",           atcmd_bt_mesh_device,      2, 2},
#endif
#if defined(CONFIG_BT_MESH_PROVISIONER_SCATTERNET) && CONFIG_BT_MESH_PROVISIONER_SCATTERNET
	{"provisioner_scatternet", atcmd_bt_mesh_provisioner_scatternet, 2, 2},
#endif
#if defined(CONFIG_BT_MESH_DEVICE_SCATTERNET) && CONFIG_BT_MESH_DEVICE_SCATTERNET
	{"device_scatternet", atcmd_bt_mesh_device_scatternet, 2, 2},
#endif
#if defined(CONFIG_BT_MESH_PROVISIONER_TEST) && CONFIG_BT_MESH_PROVISIONER_TEST
	{"provisioner_test", atcmd_bt_mesh_provisioner_test, 2, 9},
#endif
#if defined(CONFIG_BT_MESH_DEVICE_TEST) && CONFIG_BT_MESH_DEVICE_TEST
	{"device_test", atcmd_bt_mesh_device_test, 2, 2},
#endif
#if defined(CONFIG_BT_ISO_TEST) && CONFIG_BT_ISO_TEST
	{"iso",              atcmd_ble_iso,             4, 4},
#endif
#if defined(CONFIG_BT_A2DP) && CONFIG_BT_A2DP
	{"a2dp",             atcmd_bt_a2dp,             3, 3},
#endif
#if defined(CONFIG_BT_A2DP_SCATTERNET) && CONFIG_BT_A2DP_SCATTERNET
	{"a2dp_scatternet",  atcmd_bt_a2dp_scatternet,  3, 3},
#endif
#if defined(CONFIG_BT_A2DP_PBP) && CONFIG_BT_A2DP_PBP
	{"a2dp_pbp",         atcmd_bt_a2dp_pbp,         2, 2},
#endif
#if defined(CONFIG_BT_A2DP_HFP_PBP) && CONFIG_BT_A2DP_HFP_PBP
	{"a2dp_hfp_pbp",     atcmd_bt_a2dp_hfp_pbp,     2, 2},
#endif
#if defined(CONFIG_BT_A2DP_HFP_PBP_PERIPHERAL) && CONFIG_BT_A2DP_HFP_PBP_PERIPHERAL
	{"a2dp_hfp_pbp_peripheral",     atcmd_bt_a2dp_hfp_pbp_peripheral,     2, 2},
#endif
#if defined(CONFIG_BT_A2DP_TMAP) && CONFIG_BT_A2DP_TMAP
	{"a2dp_tmap",        atcmd_bt_a2dp_tmap,        3, 3},
#endif
#if defined(CONFIG_BT_SPP) && CONFIG_BT_SPP
	{"spp",              atcmd_bt_spp,              3, 6},
#endif
#if defined(CONFIG_BT_RFC) && CONFIG_BT_RFC
	{"rfc",              atcmd_bt_rfc,              2, 3},
#endif
#if defined(CONFIG_BT_HID) && CONFIG_BT_HID
	{"hid",              atcmd_bt_hid,              2, 3},
#endif
#if defined(CONFIG_BT_HFP) && CONFIG_BT_HFP
	{"hfp",              atcmd_bt_hfp,              3, 3},
#endif
#if defined(CONFIG_BT_A2DP_HFP) && CONFIG_BT_A2DP_HFP
	{"a2dp_hfp",         atcmd_bt_a2dp_hfp,         3, 3},
#endif
#if defined(CONFIG_BT_LE_AUDIO_GENERIC_DEMO) && CONFIG_BT_LE_AUDIO_GENERIC_DEMO
	{"le_audio_generic_demo", atcmd_bt_le_audio_generic_demo, 3, 4},
#endif
#if defined(CONFIG_BT_PBP) && CONFIG_BT_PBP
	{"pbp",              atcmd_bt_pbp,              3, 4},
#endif
#if defined(CONFIG_BT_TMAP) && CONFIG_BT_TMAP
	{"tmap",             atcmd_bt_tmap,             3, 4},
#endif
#if defined(CONFIG_BT_GMAP) && CONFIG_BT_GMAP
	{"gmap",             atcmd_bt_gmap,             3, 4},
#endif
#if defined(CONFIG_BT_LE_AUDIO_GENERIC_DEMO_PERIPHERAL) && CONFIG_BT_LE_AUDIO_GENERIC_DEMO_PERIPHERAL
	{"le_audio_generic_demo_peripheral",         atcmd_bt_le_audio_generic_demo_peripheral,   3, 3},
#endif
#if defined(CONFIG_BT_PTS) && CONFIG_BT_PTS
	{"pts",              atcmd_bt_pts,              2, 4},
#endif
	// {"demo",             atcmd_bt_demo,             1, 1},
#if defined(CONFIG_BT_TRANSFER_MODULE) && CONFIG_BT_TRANSFER_MODULE
	{"transfer_module",  atcmd_bt_transfer_module,  2, 6},
#endif
#if defined(CONFIG_BT_WIFIMATE_DEVICE) && CONFIG_BT_WIFIMATE_DEVICE
	{"ble_wifimate_device", atcmd_bt_wifimate_device, 2, 3},
#endif
#if defined(CONFIG_BT_WIFIMATE_CONFIGURATOR) && CONFIG_BT_WIFIMATE_CONFIGURATOR
	{"ble_wifimate_configurator", atcmd_bt_wifimate_configurator, 2, 6},
#endif
#if defined(CONFIG_BT_FUZZ_TEST) && CONFIG_BT_FUZZ_TEST
	{"fuzz_test",        atcmd_bt_fuzz_test, 2, 6},
#endif
#if defined(CONFIG_BT_HOGP_ULL) && CONFIG_BT_HOGP_ULL
	{"hogp_ull_device",      atcmd_bt_hogp_ull_device,      2, 2},
	{"hogp_ull_report_host", atcmd_bt_hogp_ull_report_host,  2, 2},
	{"hogp_ull",             atcmd_bt_hogp_ull,              2, 10},
#endif
	{NULL,},
};

void fBTDEMO(u16 argc, char *argv[])
{
	int ret = atcmd_bt_excute(argc - 1, &argv[1], example_table, "[AT+BTDEMO]");

	if (ret == 0) {
		BT_AT_PRINTOK();
	} else {
		BT_AT_PRINTERROR(ret);
	}
}