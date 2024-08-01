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

int bt_demo_main(void);
int atcmd_bt_demo(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	if (bt_demo_main()) {
		BTDEMO_AT_PRINTK("Error: demo example failed!");
		return -1;
	}

	BTDEMO_AT_PRINTK("demo example OK!");
	return 0;
}

int bt_audio_mp_test_main(uint8_t enable);
int atcmd_bt_audio_mp_test(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	uint8_t op;

	char *action[] = {"disable", "enable"};
	if ((op = (uint8_t)str_to_int(argv[0])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for bt audio mp test demo !", op);
		return -1;
	}
	if (bt_audio_mp_test_main(op)) {
		BTDEMO_AT_PRINTK("Error: bt audio mp test example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("bt audio mp test example %s OK!", action[op]);
	return 0;
}

int ble_central_main(uint8_t enable);
int atcmd_bt_central(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for central!", op);
		return -1;
	}

	if (ble_central_main(op)) {
		BTDEMO_AT_PRINTK("Error: central example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("central example %s OK!", action[op]);
	return 0;
}

int ble_peripheral_main(uint8_t enable);
int atcmd_bt_peripheral(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for peripheral!", op);
		return -1;
	}

	if (ble_peripheral_main(op)) {
		BTDEMO_AT_PRINTK("Error: peripheral example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("peripheral example %s OK!", action[op]);
	return 0;
}

int ble_scatternet_main(uint8_t enable);
int atcmd_bt_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for scatternet!", op);
		return -1;
	}

	if (ble_scatternet_main(op)) {
		BTDEMO_AT_PRINTK("Error: scatternet example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("scatternet example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_throughput_test(int argc, char *argv[]);
int ble_throughput_main(uint8_t enable);
int atcmd_bt_throughput(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if ((strcmp("conn", argv[0]) == 0) || (strcmp("test_start", argv[0]) == 0) ||
		(strcmp("test_stop", argv[0]) == 0)) {
		atcmd_bt_throughput_test(argc, argv);
	} else if ((strcmp("0", argv[0]) == 0) || (strcmp("1", argv[0]) == 0)) {
		op = (uint8_t)str_to_int(argv[0]);
		if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
			BTDEMO_AT_PRINTK("Error: wrong value (%d) for throughput!", op);
			return -1;
		}

		if (ble_throughput_main(op)) {
			BTDEMO_AT_PRINTK("Error: throughput example %s failed!", action[op]);
			return -1;
		}

		BTDEMO_AT_PRINTK("throughput example %s OK!", action[op]);
	} else {
		BTDEMO_AT_PRINTK("Input wrong parameters!!! ");
	}
	return 0;
}

int ble_mesh_provisioner_main(uint8_t enable);
int atcmd_bt_mesh_provisioner(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for mesh provisioner!", op);
		return -1;
	}

	if (ble_mesh_provisioner_main(op)) {
		BTDEMO_AT_PRINTK("Error: mesh provisioner example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("mesh provisioner example %s OK!", action[op]);
	return 0;
}

int ble_mesh_device_main(uint8_t enable);
int atcmd_bt_mesh_device(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for mesh device!", op);
		return -1;
	}

	if (ble_mesh_device_main(op)) {
		BTDEMO_AT_PRINTK("Error: mesh device example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("mesh device example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_mesh_performence_test(int argc, char *argv[]);
int ble_mesh_provisioner_test_main(uint8_t enable);
int atcmd_bt_mesh_provisioner_test(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (!strcmp("start", argv[0]) || !strcmp("result", argv[0]) || !strcmp("cmd", argv[0])) {
		atcmd_bt_mesh_performence_test(argc, argv);
	} else if ((uint8_t)str_to_int(argv[0]) == 0 || (uint8_t)str_to_int(argv[0]) == 1) {
		op = (uint8_t)str_to_int(argv[0]);
		if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
			BTDEMO_AT_PRINTK("Error: wrong value (%d) for mesh provisioner!", op);
			return -1;
		}

		if (ble_mesh_provisioner_test_main(op)) {
			BTDEMO_AT_PRINTK("Error: mesh provisioner example %s failed!", action[op]);
			return -1;
		}

		BTDEMO_AT_PRINTK("mesh provisioner test example %s OK!", action[op]);
	} else {
		AT_PRINTK("Input wrong parameters!!! \r\n");
	}
	return 0;
}

int ble_mesh_device_test_main(uint8_t enable);
int atcmd_bt_mesh_device_test(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for mesh device!", op);
		return -1;
	}

	if (ble_mesh_device_test_main(op)) {
		BTDEMO_AT_PRINTK("Error: mesh device test example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("mesh device test example %s OK!", action[op]);
	return 0;
}

int ble_mesh_provisioner_scatternet_main(uint8_t enable);
int atcmd_bt_mesh_provisioner_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for mesh provisioner scatternet!", op);
		return -1;
	}

	if (ble_mesh_provisioner_scatternet_main(op)) {
		BTDEMO_AT_PRINTK("Error: mesh provisioner scatternet example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("mesh provisioner scatternet example %s OK!", action[op]);
	return 0;
}

int ble_mesh_device_scatternet_main(uint8_t enable);
int atcmd_bt_mesh_device_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for mesh device scatternet!", op);
		return -1;
	}

	if (ble_mesh_device_scatternet_main(op)) {
		BTDEMO_AT_PRINTK("Error: mesh device scatternet example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("mesh device scatternet example %s OK!", action[op]);
	return 0;
}

int bt_a2dp_main(uint8_t role, uint8_t enable);
int atcmd_bt_a2dp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 1;
		BTDEMO_AT_PRINTK("Set a2dp role (snk)");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 0;
		BTDEMO_AT_PRINTK("Set a2dp role (src)");
	} else {
		BTDEMO_AT_PRINTK("invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for a2dp demo !", op);
		return -1;
	}
	if (bt_a2dp_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: a2dp example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("a2dp example %s OK!", action[op]);
	return 0;
}

int bt_a2dp_scatternet_main(uint8_t role, uint8_t enable);
int atcmd_bt_a2dp_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 1;
		BTDEMO_AT_PRINTK("Set a2dp role (snk)");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 0;
		BTDEMO_AT_PRINTK("Set a2dp role (src)");
	} else {
		BTDEMO_AT_PRINTK("invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for a2dp demo !", op);
		return -1;
	}
	if (bt_a2dp_scatternet_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: a2dp scatternet example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("a2dp scatternet example %s OK!", action[op]);
	return 0;
}

int bt_a2dp_provisioner_scatternet_main(uint8_t role, uint8_t enable);
int atcmd_bt_a2dp_provisioner_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 1;
		BTDEMO_AT_PRINTK("Set a2dp role (snk)");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 0;
		BTDEMO_AT_PRINTK("Set a2dp role (src)");
	} else {
		BTDEMO_AT_PRINTK("invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for a2dp provisioner scatternet demo !", op);
		return -1;
	}
	if (bt_a2dp_provisioner_scatternet_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: a2dp provisioner scatternet example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("a2dp provisioner scatternet example %s OK!", action[op]);
	return 0;
}

int bt_a2dp_sink_pbp_source_main(uint8_t enable);
int atcmd_bt_a2dp_pbp(int argc, char *argv[])
{
	(void)argc;
	uint8_t enable = 0;
	char *action[] = {"disable", "enable"};

	if ((enable = (uint8_t)(str_to_int(argv[0]))) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for a2dp pbp demo !", enable);
		return -1;
	}

	if (bt_a2dp_sink_pbp_source_main(enable)) {
		BTDEMO_AT_PRINTK("Error: example %s failed!", __func__);
		return -1;
	}

	BTDEMO_AT_PRINTK("%s: %s OK!", __func__, action[enable]);
	return 0;
}

int bt_a2dp_sink_tmap_main(uint8_t role, uint8_t enable);
int atcmd_bt_a2dp_tmap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role = 0;
	uint8_t op = 0;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "ums") == 0) {
		role = 0x04;
		BTDEMO_AT_PRINTK("Set a2dp sink ums role");
	} else if (strcmp(argv[0], "bms") == 0) {
		role = 0x10;
		BTDEMO_AT_PRINTK("Set a2dp sink bms role");
	} else {
		BTDEMO_AT_PRINTK("invalid role set");
		return -1;
	}

	if ((op = (uint8_t)(str_to_int(argv[1]))) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for a2dp tmap demo !", op);
		return -1;
	}

	if (bt_a2dp_sink_tmap_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("a2dp sink tmap example %s OK!", action[op]);
	return 0;
}

int bt_hfp_main(uint8_t role, uint8_t enable);
int atcmd_bt_hfp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "hf") == 0) {
		role = 1;
		BTDEMO_AT_PRINTK("Set hfp role (hand free)");
	} else if (strcmp(argv[0], "ag") == 0) {
		role = 0;
		BTDEMO_AT_PRINTK("Set hfp role (audio gate)");
	} else {
		BTDEMO_AT_PRINTK("invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for hfp demo !", op);
		return -1;
	}
	if (bt_hfp_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: hfp example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("hfp example %s OK!", action[op]);
	return 0;
}

int bt_audio_a2dp_hfp_main(uint8_t role, uint8_t enable);
int atcmd_bt_a2dp_hfp(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	uint8_t role;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 1;
		BTDEMO_AT_PRINTK("Set a2dp role (snk) hfp role (hf)");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 0;
		BTDEMO_AT_PRINTK("Set a2dp role (src) hfp role (ag)");
	} else {
		BTDEMO_AT_PRINTK("invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for a2dp hfp demo !", op);
		return -1;
	}
	if (bt_audio_a2dp_hfp_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: a2dp hfp example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("a2dp hfp example %s OK!", action[op]);
	return 0;
}

int bt_spp_main(uint8_t role, uint8_t enable);
int atcmd_bt_spp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "client") == 0) {
		role = 0;
		BTDEMO_AT_PRINTK("Set spp role (client)");
	} else if (strcmp(argv[0], "server") == 0) {
		role = 1;
		BTDEMO_AT_PRINTK("Set spp role (server)");
	} else {
		BTDEMO_AT_PRINTK("invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for spp!", op);
		return -1;
	}
	if (bt_spp_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: spp example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("spp example %s OK!", action[op]);
	return 0;
}

int bt_hid_main(uint8_t role, uint8_t enable);
int atcmd_bt_hid(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "gamepad") == 0) {
		role = 0;
		BTDEMO_AT_PRINTK("Set hid role (gamepad)");
	} else if (strcmp(argv[0], "mouse") == 0) {
		role = 1;
		BTDEMO_AT_PRINTK("Set hid role (mouse)");
	} else {
		BTDEMO_AT_PRINTK("invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for hid demo !", op);
		return -1;
	}
	if (bt_hid_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: hid example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("hid example %s OK!", action[op]);
	return 0;
}

int bt_le_iso_main(uint8_t role, uint8_t enable);
int atcmd_ble_iso(int argc, char **argv)
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "bis") == 0) {
		if (strcmp(argv[1], "broadcaster") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR */
			role = 0x03;
			BTDEMO_AT_PRINTK("Set bis broadcaster");
		} else if (strcmp(argv[1], "receiver") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK */
			role = 0x04;
			BTDEMO_AT_PRINTK("Set bis receiver");
		} else {
			BTDEMO_AT_PRINTK("invalid bis role set");
			return -1;
		}
	} else if (strcmp(argv[0], "cis") == 0) {
		if (strcmp(argv[1], "initiator") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI */
			role = 0x01;
			BTDEMO_AT_PRINTK("Set cis initiator");
		} else if (strcmp(argv[1], "acceptor") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER */
			role = 0x02;
			BTDEMO_AT_PRINTK("Set cis acceptor");
		} else {
			BTDEMO_AT_PRINTK("invalid cis role set");
			return -1;
		}
	} else {
		BTDEMO_AT_PRINTK("invalid iso cis / bis set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[2])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for iso demo !", op);
		return -1;
	}
	if (bt_le_iso_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: iso example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("iso example %s OK!", action[op]);
	return 0;
}

int bt_bap_main(uint8_t role, uint8_t enable);
int atcmd_bt_bap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "broadcast") == 0) {
		if (strcmp(argv[1], "source") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR */
			role = 0x01;
			BTDEMO_AT_PRINTK("Set bap broadcast source");
		} else if (strcmp(argv[1], "sink") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK */
			role = 0x02;
			BTDEMO_AT_PRINTK("Set bap broadcast sink");
		} else if (strcmp(argv[1], "assistant") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI */
			role = 0x04;
			BTDEMO_AT_PRINTK("Set bap broadcast assistant");
		} else if (strcmp(argv[1], "delegate") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_SCAN_DELE */
			role = 0x08;
			BTDEMO_AT_PRINTK("Set bap scan delegate");
		} else {
			BTDEMO_AT_PRINTK("invalid broadcast role set");
			return -1;
		}
	} else if (strcmp(argv[0], "unicast") == 0) {
		if (strcmp(argv[1], "client") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI */
			role = 0x20;
			BTDEMO_AT_PRINTK("Set bap unicast client");
		} else if (strcmp(argv[1], "server") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER */
			role = 0x10;
			BTDEMO_AT_PRINTK("Set bap unicast server");
		} else {
			BTDEMO_AT_PRINTK("invalid unicast role set");
			return -1;
		}
	} else {
		BTDEMO_AT_PRINTK("invalid bap broadcast / unicast set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[2])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for bap demo !", op);
		return -1;
	}
	if (bt_bap_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: bap example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("bap example %s OK!", action[op]);
	return 0;
}

int bt_cap_main(uint8_t role, uint8_t enable);
int atcmd_bt_cap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "initiator") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR */
		role = 0x01;
		BTDEMO_AT_PRINTK("Set cap initiator");
	} else if (strcmp(argv[0], "acceptor") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR */
		role = 0x02;
		BTDEMO_AT_PRINTK("Set cap acceptor");
	} else if (strcmp(argv[0], "commander") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER */
		role = 0x04;
		BTDEMO_AT_PRINTK("Set cap commander");
	} else {
		BTDEMO_AT_PRINTK("invalid unicast role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for cap demo !", op);
		return -1;
	}
	if (bt_cap_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: cap example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("cap example %s OK!", action[op]);
	return 0;
}

int bt_pbp_main(uint8_t role, uint8_t enable);
int atcmd_bt_pbp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "source") == 0) {
		/* RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SOURCE */
		role = 0x01;
		BTDEMO_AT_PRINTK("Set pbp broadcast source");
	} else if (strcmp(argv[0], "sink") == 0) {
		/* RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK */
		role = 0x02;
		BTDEMO_AT_PRINTK("Set pbp broadcast sink");
	} else if (strcmp(argv[0], "assistant") == 0) {
		/* RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT */
		role = 0x04;
		BTDEMO_AT_PRINTK("Set pbp broadcast assistant");
	} else {
		BTDEMO_AT_PRINTK("invalid pbp role");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for pbp demo !", op);
		return -1;
	}
	if (bt_pbp_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: pbp example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("pbp example %s OK!", action[op]);
	return 0;
}

int bt_tmap_main(uint8_t role, uint8_t enable);
int atcmd_bt_tmap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "cg") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_CALL_GATEWAY_ROLE */
		role = 0x01;
		BTDEMO_AT_PRINTK("Set tmap Call Gateway");
	} else if (strcmp(argv[0], "ct") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_CALL_TERMINAL_ROLE */
		role = 0x02;
		BTDEMO_AT_PRINTK("Set tmap Call Terminal");
	} else if (strcmp(argv[0], "ums") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_UNICAST_MEDIA_SENDER_ROLE */
		role = 0x04;
		BTDEMO_AT_PRINTK("Set tmap Unicast Media Sender");
	} else if (strcmp(argv[0], "umr") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_UNICAST_MEDIA_RECEIVER_ROLE */
		role = 0x08;
		BTDEMO_AT_PRINTK("Set tmap Unicast Media Receiver");
	} else if (strcmp(argv[0], "bms") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_BROADCAST_MEDIA_SENDER_ROLE */
		role = 0x10;
		BTDEMO_AT_PRINTK("Set tmap Broadcast Media Sender");
	} else if (strcmp(argv[0], "bmr") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_BROADCAST_MEDIA_RECEIVER_ROLE */
		role = 0x20;
		BTDEMO_AT_PRINTK("Set tmap Broadcast Media Receiver");
	} else {
		BTDEMO_AT_PRINTK("invalid tmap role");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for tmap demo !", op);
		return -1;
	}
	if (bt_tmap_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: tmap example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("tmap example %s OK!", action[op]);
	return 0;
}

int bt_gmap_main(uint8_t role, uint8_t enable);
int atcmd_bt_gmap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "ugg") == 0) {
		/* RTK_BT_LE_AUDIO_GMAP_ROLE_UGG */
		role = 0x01;
		BTDEMO_AT_PRINTK("Set gmap UGG");
	} else if (strcmp(argv[0], "ugt") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_UGT */
		role = 0x02;
		BTDEMO_AT_PRINTK("Set gmap UGT");
	} else if (strcmp(argv[0], "bgs") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_BGS */
		role = 0x04;
		BTDEMO_AT_PRINTK("Set gmap BGS");
	} else if (strcmp(argv[0], "bgr") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_BGR */
		role = 0x08;
		BTDEMO_AT_PRINTK("Set gmap BGR");
	} else {
		BTDEMO_AT_PRINTK("invalid GMAP role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for gmap demo !", op);
		return -1;
	}
	if (bt_gmap_main(role, op)) {
		BTDEMO_AT_PRINTK("Error: gmap example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("gmap example %s OK!", action[op]);
	return 0;
}

int bt_config_main(uint8_t enable);
int atcmd_bt_config(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BTDEMO_AT_PRINTK("Error: wrong value (%d) for bt config!", op);
		return -1;
	}

	if (bt_config_main(op)) {
		BTDEMO_AT_PRINTK("Error: bt config example %s failed!", action[op]);
		return -1;
	}

	BTDEMO_AT_PRINTK("bt config example %s OK!", action[op]);
	return 0;
}

int bt_pts_main(uint8_t enable);
int atcmd_bt_pts(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (!strcmp("adv_data", argv[0])) {
		atcmd_bt_pts_cmd(argc, argv);
	} else if ((uint8_t)str_to_int(argv[0]) == 1 || (uint8_t)str_to_int(argv[0]) == 0) {
		op = (uint8_t)str_to_int(argv[0]) > 2 ? 2 : (uint8_t)str_to_int(argv[0]);
		if (bt_pts_main(op)) {
			BTDEMO_AT_PRINTK("Error: pts example %s failed!", action[op]);
			return -1;
		} else {
			BTDEMO_AT_PRINTK("pts example %s OK!", action[op]);
		}
	} else {
		BTDEMO_AT_PRINTK("Error: wrong pts atcmd!");
	}
	return 0;
}