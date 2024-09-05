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
		BT_LOGE("Error: demo example failed!\r\n");
		return -1;
	}

	BT_LOGA("demo example OK!\r\n");
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
		BT_LOGE("Error: wrong value (%d) for bt audio mp test example!\r\n", op);
		return -1;
	}
	if (bt_audio_mp_test_main(op)) {
		BT_LOGE("Error: bt audio mp test example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("bt audio mp test example %s OK!\r\n", action[op]);
	return 0;
}

int ble_central_main(uint8_t enable);
int atcmd_bt_central(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for central example!\r\n", op);
		return -1;
	}

	if (ble_central_main(op)) {
		BT_LOGE("Error: central example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("central example %s OK!\r\n", action[op]);
	return 0;
}

int ble_peripheral_main(uint8_t enable);
int atcmd_bt_peripheral(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for peripheral example!\r\n", op);
		return -1;
	}

	if (ble_peripheral_main(op)) {
		BT_LOGE("Error: peripheral example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("peripheral example %s OK!\r\n", action[op]);
	return 0;
}

int ble_scatternet_main(uint8_t enable);
int atcmd_bt_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for scatternet example!\r\n", op);
		return -1;
	}

	if (ble_scatternet_main(op)) {
		BT_LOGE("Error: scatternet example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("scatternet example %s OK!\r\n", action[op]);
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
			BT_LOGE("Error: wrong value (%d) for throughput example!\r\n", op);
			return -1;
		}

		if (ble_throughput_main(op)) {
			BT_LOGE("Error: throughput example %s failed!\r\n", action[op]);
			return -1;
		}

		BT_LOGA("throughput example %s OK!\r\n", action[op]);
	} else {
		BT_LOGE("Input wrong parameters!!!\r\n");
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
		BT_LOGE("Error: wrong value (%d) for mesh provisioner example!\r\n", op);
		return -1;
	}

	if (ble_mesh_provisioner_main(op)) {
		BT_LOGE("Error: mesh provisioner example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("mesh provisioner example %s OK!\r\n", action[op]);
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
		BT_LOGE("Error: wrong value (%d) for mesh device example!\r\n", op);
		return -1;
	}

	if (ble_mesh_device_main(op)) {
		BT_LOGE("Error: mesh device example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("mesh device example %s OK!\r\n", action[op]);
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
			BT_LOGE("Error: wrong value (%d) for mesh provisioner example!\r\n", op);
			return -1;
		}

		if (ble_mesh_provisioner_test_main(op)) {
			BT_LOGE("Error: mesh provisioner example %s failed!\r\n", action[op]);
			return -1;
		}

		BT_LOGA("mesh provisioner test example %s OK!\r\n", action[op]);
	} else {
		BT_LOGE("Input wrong parameters!!!\r\n");
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
		BT_LOGE("Error: wrong value (%d) for mesh device example!\r\n", op);
		return -1;
	}

	if (ble_mesh_device_test_main(op)) {
		BT_LOGE("Error: mesh device test example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("mesh device test example %s OK!\r\n", action[op]);
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
		BT_LOGE("Error: wrong value (%d) for mesh provisioner scatternet example!\r\n", op);
		return -1;
	}

	if (ble_mesh_provisioner_scatternet_main(op)) {
		BT_LOGE("Error: mesh provisioner scatternet example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("mesh provisioner scatternet example %s OK!\r\n", action[op]);
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
		BT_LOGE("Error: wrong value (%d) for mesh device scatternet example!\r\n", op);
		return -1;
	}

	if (ble_mesh_device_scatternet_main(op)) {
		BT_LOGE("Error: mesh device scatternet example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("mesh device scatternet example %s OK!\r\n", action[op]);
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
	if (bt_a2dp_main(role, op)) {
		BT_LOGE("Error: a2dp example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("a2dp example %s OK!\r\n", action[op]);
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
	if (bt_a2dp_scatternet_main(role, op)) {
		BT_LOGE("Error: a2dp scatternet example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("a2dp scatternet example %s OK!\r\n", action[op]);
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
		BT_LOGA("Set a2dp role (snk)\r\n");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 0;
		BT_LOGA("Set a2dp role (src)\r\n");
	} else {
		BT_LOGE("Invalid role set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		BT_LOGE("Error: wrong value (%d) for a2dp provisioner scatternet example!\r\n", op);
		return -1;
	}
	if (bt_a2dp_provisioner_scatternet_main(role, op)) {
		BT_LOGE("Error: a2dp provisioner scatternet example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("a2dp provisioner scatternet example %s OK!\r\n", action[op]);
	return 0;
}

int bt_a2dp_sink_pbp_source_main(uint8_t enable);
int atcmd_bt_a2dp_pbp(int argc, char *argv[])
{
	(void)argc;
	uint8_t op = 0;
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)(str_to_int(argv[0]))) > 2) {
		BT_LOGE("Error: wrong value (%d) for a2dp pbp example!\r\n", op);
		return -1;
	}

	if (bt_a2dp_sink_pbp_source_main(op)) {
		BT_LOGE("Error: a2dp pbp example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("a2dp pbp example %s OK!\r\n", action[op]);
	return 0;
}

int bt_a2dp_sink_tmap_main(uint8_t role, uint8_t enable);
int bt_tmap_a2dp_source_main(uint8_t role, uint8_t enable);
int atcmd_bt_a2dp_tmap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role = 0;
	uint8_t op = 0;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "ums") == 0) {
		role = 0x04;
		BT_LOGA("Set a2dp sink ums role\r\n");
	} else if (strcmp(argv[0], "bms") == 0) {
		role = 0x10;
		BT_LOGA("Set a2dp sink bms role\r\n");
	} else if (strcmp(argv[0], "umr") == 0) {
		role = 0x08;
		BT_LOGA("Set tmap umr a2dp source role\r\n");
	} else if (strcmp(argv[0], "bmr") == 0) {
		role = 0x20;
		BT_LOGA("Set tmap bmr a2dp source role\r\n");
	} else {
		BT_LOGE("Invalid role set\r\n");
		return -1;
	}

	if ((op = (uint8_t)(str_to_int(argv[1]))) > 2) {
		BT_LOGE("Error: wrong value (%d) for a2dp tmap example!\r\n", op);
		return -1;
	}

	if (role == 0x04 || role == 0x10) {
		if (bt_a2dp_sink_tmap_main(role, op)) {
			BT_LOGE("Error: a2dp sink tmap example %s failed!\r\n", action[op]);
			return -1;
		}
		BT_LOGA("a2dp sink tmap example %s OK!\r\n", action[op]);
	} else if (role == 0x08 || role == 0x20) {
		if (bt_tmap_a2dp_source_main(role, op)) {
			BT_LOGE("Error: tmap a2dp source example %s failed!\r\n", action[op]);
			return -1;
		}
		BT_LOGA("tmap a2dp source example %s OK!\r\n", action[op]);
	}

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
	if (bt_hfp_main(role, op)) {
		BT_LOGE("Error: hfp example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("hfp example %s OK!\r\n", action[op]);
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
	if (bt_audio_a2dp_hfp_main(role, op)) {
		BT_LOGE("Error: a2dp hfp example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("a2dp hfp example %s OK!\r\n", action[op]);
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
	if (bt_spp_main(role, op)) {
		BT_LOGE("Error: spp example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("spp example %s OK!\r\n", action[op]);
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
	if (bt_hid_main(role, op)) {
		BT_LOGE("Error: hid example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("hid example %s OK!\r\n", action[op]);
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
	if (bt_le_iso_main(role, op)) {
		BT_LOGE("Error: iso example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("iso example %s OK!\r\n", action[op]);
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
			BT_LOGA("Set bap broadcast source\r\n");
		} else if (strcmp(argv[1], "sink") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK */
			role = 0x02;
			BT_LOGA("Set bap broadcast sink\r\n");
		} else if (strcmp(argv[1], "assistant") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI */
			role = 0x04;
			BT_LOGA("Set bap broadcast assistant\r\n");
		} else if (strcmp(argv[1], "delegate") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_SCAN_DELE */
			role = 0x08;
			BT_LOGA("Set bap scan delegate\r\n");
		} else {
			BT_LOGE("Invalid broadcast role set\r\n");
			return -1;
		}
	} else if (strcmp(argv[0], "unicast") == 0) {
		if (strcmp(argv[1], "client") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI */
			role = 0x20;
			BT_LOGA("Set bap unicast client\r\n");
		} else if (strcmp(argv[1], "server") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER */
			role = 0x10;
			BT_LOGA("Set bap unicast server\r\n");
		} else {
			BT_LOGE("Invalid unicast role set\r\n");
			return -1;
		}
	} else {
		BT_LOGE("Invalid bap broadcast / unicast set\r\n");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[2])) > 2) {
		BT_LOGE("Error: wrong value (%d) for bap example!\r\n", op);
		return -1;
	}
	if (bt_bap_main(role, op)) {
		BT_LOGE("Error: bap example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("bap example %s OK!\r\n", action[op]);
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
		BT_LOGA("Set cap initiator\r\n");
	} else if (strcmp(argv[0], "acceptor") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR */
		role = 0x02;
		BT_LOGA("Set cap acceptor\r\n");
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
	if (bt_cap_main(role, op)) {
		BT_LOGE("Error: cap example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("cap example %s OK!\r\n", action[op]);
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
	if (bt_pbp_main(role, op)) {
		BT_LOGE("Error: pbp example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("pbp example %s OK!\r\n", action[op]);
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
	if (bt_tmap_main(role, op)) {
		BT_LOGE("Error: tmap example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("tmap example %s OK!\r\n", action[op]);
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
	if (bt_gmap_main(role, op)) {
		BT_LOGE("Error: gmap example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("gmap example %s OK!\r\n", action[op]);
	return 0;
}

int bt_config_main(uint8_t enable);
int atcmd_bt_config(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		BT_LOGE("Error: wrong value (%d) for bt config!\r\n", op);
		return -1;
	}

	if (bt_config_main(op)) {
		BT_LOGE("Error: bt config example %s failed!\r\n", action[op]);
		return -1;
	}

	BT_LOGA("bt config example %s OK!\r\n", action[op]);
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
			BT_LOGE("Error: pts example %s failed!\r\n", action[op]);
			return -1;
		} else {
			BT_LOGA("pts example %s OK!\r\n", action[op]);
		}
	} else {
		BT_LOGE("Error: wrong pts atcmd!\r\n");
	}
	return 0;
}

int ble_transfer_module_main(uint8_t enable);
int atcmd_bt_transfer_module(int argc, char *argv[])
{
	int ret = 0;
	if ((strcmp("0", argv[0]) == 0) || (strcmp("1", argv[0]) == 0)) {
		uint8_t op = (uint8_t)str_to_int(argv[0]);
		if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
			BT_LOGE("Error: wrong parameter (%d) for transfer module example!\r\n", op);
			return -1;
		}

		if (ble_transfer_module_main(op)) {
			BT_LOGE("Error: transfer module example %s failed!\r\n", (op == 1) ? "enable" : "disable");
			return -1;
		}

		BT_LOGA("transfer module example %s OK!\r\n", (op == 1) ? "enable" : "disable");
	} else {
		ret = atcmd_bt_transfer_module_cmd(argc, argv);
	}
	return ret;
}