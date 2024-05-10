/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdlib.h>
#include <string.h>
#include <log_service.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>

_WEAK int bt_demo_main(void)
{
	AT_PRINTK("[ATBE] NO demo example!");
	return -1;
}

_WEAK int bt_audio_mp_test_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO bt audio mp test example!");
	return -1;
}

_WEAK int ble_central_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO central example!");
	return -1;
}

_WEAK int ble_peripheral_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO peripheral example!");
	return -1;
}

_WEAK int ble_scatternet_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO scatternet example!");
	return -1;
}

_WEAK int atcmd_bt_throughput_test(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO throughput example!");
	return -1;
}

_WEAK int ble_throughput_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO throughput example!");
	return -1;
}

_WEAK int ble_mesh_provisioner_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO mesh provisioner example!");
	return -1;
}

_WEAK int ble_mesh_device_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO mesh device example!");
	return -1;
}

_WEAK int ble_mesh_provisioner_scatternet_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO mesh provisioner scatternet example!");
	return -1;
}

_WEAK int ble_mesh_device_scatternet_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO mesh device scatternet example!");
	return -1;
}

_WEAK int ble_mesh_provisioner_test_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO mesh provisioner test example!");
	return -1;
}

_WEAK int ble_mesh_device_test_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO mesh device test example!");
	return -1;
}

_WEAK int atcmd_bt_mesh_performence_test(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO mesh provisioner test example!");
	return -1;
}

_WEAK int bt_config_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO bt config example!");
	return -1;
}

_WEAK int bt_a2dp_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO a2dp example!");
	return -1;
}

_WEAK int bt_a2dp_scatternet_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO a2dp scatternet example!");
	return -1;
}

_WEAK int bt_a2dp_provisioner_scatternet_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO a2dp provisioner scatternet example!");
	return -1;
}

_WEAK int bt_a2dp_sink_pbp_source_main(uint8_t enable)
{
	(void)enable;

	AT_PRINTK("[ATBE] NO a2dp sink pbp source example!");
	return -1;
}

_WEAK int bt_a2dp_sink_tmap_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO a2dp sink tmap example!");
	return -1;
}

_WEAK int bt_spp_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO spp example!");
	return -1;
}

_WEAK int bt_hid_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO hid example!");
	return -1;
}

_WEAK int bt_hfp_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO hfp example!");
	return -1;
}

_WEAK int bt_pts_main(uint8_t enable)
{
	(void)enable;
	AT_PRINTK("[ATBE] NO pts example!");
	return -1;
}

_WEAK int atcmd_bt_pts_cmd(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO pts example!");
	return -1;
}

_WEAK int bt_audio_a2dp_hfp_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO a2dp hfp example!");
	return -1;
}

_WEAK int bt_bap_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO bap example!");
	return -1;
}

_WEAK int bt_cap_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO cap example!");
	return -1;
}

_WEAK int bt_pbp_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO pbp example!");
	return -1;
}

_WEAK int bt_tmap_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO tmap example!");
	return -1;
}

_WEAK int bt_gmap_main(uint8_t role, uint8_t enable)
{
	(void)role;
	(void)enable;
	AT_PRINTK("[ATBE] NO gmap example!");
	return -1;
}

_WEAK int ble_cis_acceptor_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble cis acceptor example!");
	return -1;
}

_WEAK int ble_cis_initiator_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble cis initiator example!");
	return -1;
}

_WEAK int ble_bis_broadcaster_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble bis broadcaster example!");
	return -1;
}

_WEAK int ble_bis_receiver_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble bis receiver example!");
	return -1;
}

_WEAK int ble_audio_broadcast_sink_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio broadcast sink example!");
	return -1;
}

_WEAK int ble_audio_broadcast_source_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio broadcast source example!");
	return -1;
}
_WEAK int ble_audio_broadcast_assistant_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio broadcast assistant example!");
	return -1;
}

_WEAK int ble_audio_scan_delegator_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio scan delegator example!");
	return -1;
}
_WEAK int ble_audio_unicast_server_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio unicast server example!");
	return -1;
}

_WEAK int ble_audio_unicast_client_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio unicast client example!");
	return -1;
}

_WEAK int ble_audio_acceptor_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio acceptor example!");
	return -1;
}

_WEAK int ble_audio_initiator_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio initiator example!");
	return -1;
}

_WEAK int ble_audio_commander_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio commander example!");
	return -1;
}

_WEAK int ble_audio_tmap_ums_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio tmap ums example!");
	return -1;
}

_WEAK int ble_audio_tmap_umr_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio tmap umr example!");
	return -1;
}

_WEAK int ble_audio_tmap_bms_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio tmap bms example!");
	return -1;
}

_WEAK int ble_audio_tmap_bmr_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio tmap bmr example!");
	return -1;
}

_WEAK int ble_audio_pbp_broadcast_source_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio pbp broadcast source example!");
	return -1;
}

_WEAK int ble_audio_pbp_broadcast_sink_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio pbp broadcast sink example!");
	return -1;
}

_WEAK int ble_audio_pbp_broadcast_assistant_main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	AT_PRINTK("[ATBE] NO ble audio pbp broadcast assistant example!");
	return -1;
}

int atcmd_bt_demo(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	if (bt_demo_main()) {
		AT_PRINTK("[ATBE] Error: demo example failed!");
		return -1;
	}

	AT_PRINTK("[ATBE] demo example OK!");
	return 0;
}

int atcmd_bt_audio_mp_test(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	uint8_t op;

	char *action[] = {"disable", "enable"};
	if ((op = (uint8_t)str_to_int(argv[0])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for bt audio mp test demo !", op);
		return -1;
	}
	if (bt_audio_mp_test_main(op)) {
		AT_PRINTK("[ATBE] Error: bt audio mp test example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] bt audio mp test example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_central(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	/* It's neccessary to judge if the args num(argc) is correct, otherwise it may
	    be hardfault because the address of argv[x] may be NULL */
	if (argc != 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for central!", op);
		return -1;
	}

	if (ble_central_main(op)) {
		AT_PRINTK("[ATBE] Error: central example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] central example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_peripheral(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc != 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for peripheral!", op);
		return -1;
	}

	if (ble_peripheral_main(op)) {
		AT_PRINTK("[ATBE] Error: peripheral example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] peripheral example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_scatternet(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc != 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}

	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for scatternet!", op);
		return -1;
	}

	if (ble_scatternet_main(op)) {
		AT_PRINTK("[ATBE] Error: scatternet example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] scatternet example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_throughput(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc < 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}

	if ((strcmp("conn", argv[0]) == 0) || (strcmp("test_start", argv[0]) == 0) ||
		(strcmp("test_stop", argv[0]) == 0)) {
		atcmd_bt_throughput_test(argc, argv);
	} else if ((strcmp("0", argv[0]) == 0) || (strcmp("1", argv[0]) == 0)) {
		op = (uint8_t)str_to_int(argv[0]);
		if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
			AT_PRINTK("[ATBE] Error: wrong value (%d) for throughput!", op);
			return -1;
		}

		if (ble_throughput_main(op)) {
			AT_PRINTK("[ATBE] Error: throughput example %s failed!", action[op]);
			return -1;
		}

		AT_PRINTK("[ATBE] throughput example %s OK!", action[op]);
	} else {
		AT_PRINTK("[ATBE] Input wrong parameters!!! ");
	}
	return 0;
}

int atcmd_bt_mesh_provisioner(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc != 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}
	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for mesh provisioner!", op);
		return -1;
	}

	if (ble_mesh_provisioner_main(op)) {
		AT_PRINTK("[ATBE] Error: mesh provisioner example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] mesh provisioner example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_mesh_device(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc != 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}
	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for mesh device!", op);
		return -1;
	}

	if (ble_mesh_device_main(op)) {
		AT_PRINTK("[ATBE] Error: mesh device example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] mesh device example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_mesh_provisioner_test(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc < 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}

	if (!strcmp("start", argv[0]) || !strcmp("result", argv[0]) || !strcmp("cmd", argv[0])) {
		atcmd_bt_mesh_performence_test(argc, argv);
	} else if ((uint8_t)str_to_int(argv[0]) == 0 || (uint8_t)str_to_int(argv[0]) == 1) {
		op = (uint8_t)str_to_int(argv[0]);
		if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
			AT_PRINTK("[ATBE] Error: wrong value (%d) for mesh provisioner!", op);
			return -1;
		}

		if (ble_mesh_provisioner_test_main(op)) {
			AT_PRINTK("[ATBE] Error: mesh provisioner example %s failed!", action[op]);
			return -1;
		}

		AT_PRINTK("[ATBE] mesh provisioner test example %s OK!", action[op]);
	} else {
		AT_PRINTK("Input wrong parameters!!! \r\n");
	}
	return 0;
}

int atcmd_bt_mesh_device_test(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc != 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}
	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for mesh device!", op);
		return -1;
	}

	if (ble_mesh_device_test_main(op)) {
		AT_PRINTK("[ATBE] Error: mesh device test example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] mesh device test example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_mesh_provisioner_scatternet(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc != 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}
	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for mesh provisioner scatternet!", op);
		return -1;
	}

	if (ble_mesh_provisioner_scatternet_main(op)) {
		AT_PRINTK("[ATBE] Error: mesh provisioner scatternet example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] mesh provisioner scatternet example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_mesh_device_scatternet(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc != 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}
	op = (uint8_t)str_to_int(argv[0]);
	if (op > 1) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for mesh device scatternet!", op);
		return -1;
	}

	if (ble_mesh_device_scatternet_main(op)) {
		AT_PRINTK("[ATBE] Error: mesh device scatternet example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] mesh device scatternet example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_a2dp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 0;
		AT_PRINTK("[ATBE] Set a2dp role (snk)");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 1;
		AT_PRINTK("[ATBE] Set a2dp role (src)");
	} else {
		AT_PRINTK("[ATBE] invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for a2dp demo !", op);
		return -1;
	}
	if (bt_a2dp_main(role, op)) {
		AT_PRINTK("[ATBE] Error: a2dp example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] a2dp example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_a2dp_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 0;
		AT_PRINTK("[ATBE] Set a2dp role (snk)");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 1;
		AT_PRINTK("[ATBE] Set a2dp role (src)");
	} else {
		AT_PRINTK("[ATBE] invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for a2dp demo !", op);
		return -1;
	}
	if (bt_a2dp_scatternet_main(role, op)) {
		AT_PRINTK("[ATBE] Error: a2dp scatternet example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] a2dp scatternet example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_a2dp_provisioner_scatternet(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 0;
		AT_PRINTK("[ATBE] Set a2dp role (snk)");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 1;
		AT_PRINTK("[ATBE] Set a2dp role (src)");
	} else {
		AT_PRINTK("[ATBE] invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for a2dp provisioner scatternet demo !", op);
		return -1;
	}
	if (bt_a2dp_provisioner_scatternet_main(role, op)) {
		AT_PRINTK("[ATBE] Error: a2dp provisioner scatternet example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] a2dp provisioner scatternet example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_a2dp_pbp(int argc, char *argv[])
{
	(void)argc;
	uint8_t enable = 0;
	char *action[] = {"disable", "enable"};

	if ((enable = (uint8_t)(str_to_int(argv[0]))) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for a2dp pbp demo !", enable);
		return -1;
	}

	if (bt_a2dp_sink_pbp_source_main(enable)) {
		AT_PRINTK("[ATBE] Error: example %s failed!", __func__);
		return -1;
	}

	AT_PRINTK("[ATBE] %s: %s OK!", __func__, action[enable]);
	return 0;
}

int atcmd_bt_a2dp_tmap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role = 0;
	uint8_t op = 0;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "ums") == 0) {
		role = 0x04;
		AT_PRINTK("[ATBE] Set a2dp sink ums role");
	} else if (strcmp(argv[0], "bms") == 0) {
		role = 0x10;
		AT_PRINTK("[ATBE] Set a2dp sink bms role");
	} else {
		AT_PRINTK("[ATBE] invalid role set");
		return -1;
	}

	if ((op = (uint8_t)(str_to_int(argv[1]))) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for a2dp tmap demo !", op);
		return -1;
	}

	if (bt_a2dp_sink_tmap_main(role, op)) {
		AT_PRINTK("[ATBE] Error: example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] a2dp sink tmap example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_hfp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "hf") == 0) {
		role = 1;
		AT_PRINTK("[ATBE] Set hfp role (hand free)");
	} else if (strcmp(argv[0], "ag") == 0) {
		role = 0;
		AT_PRINTK("[ATBE] Set hfp role (audio gate)");
	} else {
		AT_PRINTK("[ATBE] invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for hfp demo !", op);
		return -1;
	}
	if (bt_hfp_main(role, op)) {
		AT_PRINTK("[ATBE] Error: hfp example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] hfp example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_a2dp_hfp(int argc, char *argv[])
{
	(void)argc;
	uint8_t op;
	uint8_t role;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "snk") == 0) {
		role = 0;
		AT_PRINTK("[ATBE] Set a2dp role (snk) hfp role (hf)");
	} else if (strcmp(argv[0], "src") == 0) {
		role = 1;
		AT_PRINTK("[ATBE] Set a2dp role (src) hfp role (ag)");
	} else {
		AT_PRINTK("[ATBE] invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for a2dp hfp demo !", op);
		return -1;
	}
	if (bt_audio_a2dp_hfp_main(role, op)) {
		AT_PRINTK("[ATBE] Error: a2dp hfp example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] a2dp hfp example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_spp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "client") == 0) {
		role = 0;
		AT_PRINTK("[ATBE] Set spp role (client)");
	} else if (strcmp(argv[0], "server") == 0) {
		role = 1;
		AT_PRINTK("[ATBE] Set spp role (server)");
	} else {
		AT_PRINTK("[ATBE] invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for spp!", op);
		return -1;
	}
	if (bt_spp_main(role, op)) {
		AT_PRINTK("[ATBE] Error: spp example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] spp example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_hid(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (strcmp(argv[0], "gamepad") == 0) {
		role = 0;
		AT_PRINTK("[ATBE] Set hid role (gamepad)");
	} else if (strcmp(argv[0], "mouse") == 0) {
		role = 1;
		AT_PRINTK("[ATBE] Set hid role (mouse)");
	} else {
		AT_PRINTK("[ATBE] invalid role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for hid demo !", op);
		return -1;
	}
	if (bt_hid_main(role, op)) {
		AT_PRINTK("[ATBE] Error: hid example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] hid example %s OK!", action[op]);
	return 0;
}

int atcmd_ble_iso(int argc, char *argv[])
{
	int ret = 0;

	if (argc < 2) {
		AT_PRINTK("[ATBE] Error: wrong arg number (%d) for cis!", argc);
		return -1;
	}

	if (strcmp(argv[0], "acceptor") == 0) {
		ret =  ble_cis_acceptor_main(argc - 1, argv + 1); //{"disable", "enable"}
		if (ret != 0) {
			AT_PRINTK("[ATBE] Error: ble cis acceptor example %s failed! ret= %d", argv[1], ret);
		}

	} else if (strcmp(argv[0], "initiator") == 0) {
		ret =  ble_cis_initiator_main(argc - 1, argv + 1); //{"disable", "enable"}
		if (ret != 0) {
			AT_PRINTK("[ATBE] Error: ble cis initiator example %s failed! ret= %d", argv[1], ret);
		}

	} else if (strcmp(argv[0], "broadcaster") == 0) {
		ret =  ble_bis_broadcaster_main(argc - 1, argv + 1); //{"disable", "enable", "send"}
		if (ret != 0) {
			AT_PRINTK("[ATBE] Error: ble bis broadcaster example %s failed! ret= %d", argv[1], ret);
		}

	} else if (strcmp(argv[0], "receiver") == 0) {
		ret =  ble_bis_receiver_main(argc - 1, argv + 1); //{"disable", "enable"}
		if (ret != 0) {
			AT_PRINTK("[ATBE] Error: ble bis receiver example %s failed! ret= %d", argv[1], ret);
		}
	} else {
		AT_PRINTK("[ATBE] Error: wrong arg (%s) for ISO!", argv[0]);
		return -1;
	}

	return ret;
}

int atcmd_ble_audio(int argc, char *argv[])
{
	int ret = 0;

	if (argc < 2) {
		AT_PRINTK("[ATBE] Error: wrong arg number (%d) for le audio!", argc);
		return -1;
	}

	if (strcmp(argv[0], "usrv") == 0) {
		ret =  ble_audio_unicast_server_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "ucli") == 0) {
		ret =  ble_audio_unicast_client_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "bsnk") == 0) {
		ret =  ble_audio_broadcast_sink_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "bsrc") == 0) {
		ret =  ble_audio_broadcast_source_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "bass") == 0) {
		ret =  ble_audio_broadcast_assistant_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "sdel") == 0) {
		ret =  ble_audio_scan_delegator_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "acc") == 0) {
		ret =  ble_audio_acceptor_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "ini") == 0) {
		ret =  ble_audio_initiator_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "com") == 0) {
		ret =  ble_audio_commander_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "tmap_ums") == 0) {
		ret =  ble_audio_tmap_ums_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "tmap_umr") == 0) {
		ret =  ble_audio_tmap_umr_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "tmap_bms") == 0) {
		ret =  ble_audio_tmap_bms_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "tmap_bmr") == 0) {
		ret =  ble_audio_tmap_bmr_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "pbp_bsrc") == 0) {
		ret =  ble_audio_pbp_broadcast_source_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "pbp_bsink") == 0) {
		ret =  ble_audio_pbp_broadcast_sink_main(argc - 1, argv + 1);
	} else if (strcmp(argv[0], "pbp_assist") == 0) {
		ret =  ble_audio_pbp_broadcast_assistant_main(argc - 1, argv + 1);
	} else {
		AT_PRINTK("[ATBE] Error: wrong argv (%s) for le audio!", argv[0]);
		return -1;
	}

	if (ret != 0) {
		AT_PRINTK("[ATBE] Error: ble audio %s %s example failed! ret= %d", argv[0], argv[1], ret);
	}

	return ret;
}

int atcmd_bt_bap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc < 3) {
		AT_PRINTK("[ATBE] Error: wrong arg number (%d) for basic audio profile!", argc);
		return -1;
	}

	if (strcmp(argv[0], "broadcast") == 0) {
		if (strcmp(argv[1], "source") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SOUR */
			role = 0x01;
			AT_PRINTK("[ATBE] Set bap broadcast source");
		} else if (strcmp(argv[1], "sink") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_SINK */
			role = 0x02;
			AT_PRINTK("[ATBE] Set bap broadcast sink");
		} else if (strcmp(argv[1], "assistant") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_BRO_ASSI */
			role = 0x04;
			AT_PRINTK("[ATBE] Set bap broadcast assistant");
		} else if (strcmp(argv[1], "delegate") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_SCAN_DELE */
			role = 0x08;
			AT_PRINTK("[ATBE] Set bap scan delegate");
		} else {
			AT_PRINTK("[ATBE] invalid broadcast role set");
			return -1;
		}
	} else if (strcmp(argv[0], "unicast") == 0) {
		if (strcmp(argv[1], "client") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_UNI_CLI */
			role = 0x20;
			AT_PRINTK("[ATBE] Set bap unicast client");
		} else if (strcmp(argv[1], "server") == 0) {
			/* RTK_BT_LE_AUDIO_BAP_ROLE_UNI_SER */
			role = 0x10;
			AT_PRINTK("[ATBE] Set bap unicast server");
		} else {
			AT_PRINTK("[ATBE] invalid unicast role set");
			return -1;
		}
	} else {
		AT_PRINTK("[ATBE] invalid bap broadcast / unicast set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[2])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for bap demo !", op);
		return -1;
	}
	if (bt_bap_main(role, op)) {
		AT_PRINTK("[ATBE] Error: bap example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] bap example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_cap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc < 2) {
		AT_PRINTK("[ATBE] Error: wrong arg number (%d) for basic audio profile!", argc);
		return -1;
	}

	if (strcmp(argv[0], "initiator") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_INITIATOR */
		role = 0x01;
		AT_PRINTK("[ATBE] Set cap initiator");
	} else if (strcmp(argv[0], "acceptor") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_ACCEPTOR */
		role = 0x02;
		AT_PRINTK("[ATBE] Set cap acceptor");
	} else if (strcmp(argv[0], "commander") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_COMMANDER */
		role = 0x04;
		AT_PRINTK("[ATBE] Set cap commander");
	} else {
		AT_PRINTK("[ATBE] invalid unicast role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for cap demo !", op);
		return -1;
	}
	if (bt_cap_main(role, op)) {
		AT_PRINTK("[ATBE] Error: cap example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] cap example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_pbp(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc < 2) {
		AT_PRINTK("[ATBE] Error: wrong arg number (%d) for pulic broadcast profile!", argc);
		return -1;
	}

	if (strcmp(argv[0], "source") == 0) {
		/* RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SOURCE */
		role = 0x01;
		AT_PRINTK("[ATBE] Set pbp broadcast source");
	} else if (strcmp(argv[0], "sink") == 0) {
		/* RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_SINK */
		role = 0x02;
		AT_PRINTK("[ATBE] Set pbp broadcast sink");
	} else if (strcmp(argv[0], "assistant") == 0) {
		/* RTK_BT_LE_AUDIO_PBP_ROLE_BROADCAST_ASSISTANT */
		role = 0x04;
		AT_PRINTK("[ATBE] Set pbp broadcast assistant");
	} else {
		AT_PRINTK("[ATBE] invalid pbp role");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for pbp demo !", op);
		return -1;
	}
	if (bt_pbp_main(role, op)) {
		AT_PRINTK("[ATBE] Error: pbp example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] pbp example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_tmap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc < 2) {
		AT_PRINTK("[ATBE] Error: wrong arg number (%d) for pulic broadcast profile!", argc);
		return -1;
	}

	if (strcmp(argv[0], "cg") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_CALL_GATEWAY_ROLE */
		role = 0x01;
		AT_PRINTK("[ATBE] Set tmap Call Gateway");
	} else if (strcmp(argv[0], "ct") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_CALL_TERMINAL_ROLE */
		role = 0x02;
		AT_PRINTK("[ATBE] Set tmap Call Terminal");
	} else if (strcmp(argv[0], "ums") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_UNICAST_MEDIA_SENDER_ROLE */
		role = 0x04;
		AT_PRINTK("[ATBE] Set tmap Unicast Media Sender");
	} else if (strcmp(argv[0], "umr") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_UNICAST_MEDIA_RECEIVER_ROLE */
		role = 0x08;
		AT_PRINTK("[ATBE] Set tmap Unicast Media Receiver");
	} else if (strcmp(argv[0], "bms") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_BROADCAST_MEDIA_SENDER_ROLE */
		role = 0x10;
		AT_PRINTK("[ATBE] Set tmap Broadcast Media Sender");
	} else if (strcmp(argv[0], "bmr") == 0) {
		/* RTK_BT_LE_AUDIO_TMAP_BROADCAST_MEDIA_RECEIVER_ROLE */
		role = 0x20;
		AT_PRINTK("[ATBE] Set tmap Broadcast Media Receiver");
	} else {
		AT_PRINTK("[ATBE] invalid tmap role");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for tmap demo !", op);
		return -1;
	}
	if (bt_tmap_main(role, op)) {
		AT_PRINTK("[ATBE] Error: tmap example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] tmap example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_gmap(int argc, char *argv[])
{
	(void)argc;
	uint8_t role;
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc < 2) {
		AT_PRINTK("[ATBE] Error: wrong arg number (%d) for gaming audio profile!", argc);
		return -1;
	}

	if (strcmp(argv[0], "ugg") == 0) {
		/* RTK_BT_LE_AUDIO_GMAP_ROLE_UGG */
		role = 0x01;
		AT_PRINTK("[ATBE] Set gmap UGG");
	} else if (strcmp(argv[0], "ugt") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_UGT */
		role = 0x02;
		AT_PRINTK("[ATBE] Set gmap UGT");
	} else if (strcmp(argv[0], "bgs") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_BGS */
		role = 0x04;
		AT_PRINTK("[ATBE] Set gmap BGS");
	} else if (strcmp(argv[0], "bgr") == 0) {
		/* RTK_BT_LE_AUDIO_CAP_ROLE_BGR */
		role = 0x08;
		AT_PRINTK("[ATBE] Set gmap BGR");
	} else {
		AT_PRINTK("[ATBE] invalid GMAP role set");
		return -1;
	}
	if ((op = (uint8_t)str_to_int(argv[1])) > 2) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for gmap demo !", op);
		return -1;
	}
	if (bt_gmap_main(role, op)) {
		AT_PRINTK("[ATBE] Error: gmap example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] gmap example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_config(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc != 1) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}


	if ((op = (uint8_t)str_to_int(argv[0])) > 1) {
		AT_PRINTK("[ATBE] Error: wrong value (%d) for bt config!", op);
		return -1;
	}

	if (bt_config_main(op)) {
		AT_PRINTK("[ATBE] Error: bt config example %s failed!", action[op]);
		return -1;
	}

	AT_PRINTK("[ATBE] bt config example %s OK!", action[op]);
	return 0;
}

int atcmd_bt_pts(int argc, char *argv[])
{
	uint8_t op;
	char *action[] = {"disable", "enable"};

	if (argc < 1 || argc > 3) {
		AT_PRINTK("%s, wrong args num: %d", __func__, argc);
		return -1;
	}

	if (!strcmp("adv_data", argv[0])) {
		atcmd_bt_pts_cmd(argc, argv);
	} else if ((uint8_t)str_to_int(argv[0]) == 1 || (uint8_t)str_to_int(argv[0]) == 0) {
		op = (uint8_t)str_to_int(argv[0]) > 2 ? 2 : (uint8_t)str_to_int(argv[0]);
		if (bt_pts_main(op)) {
			AT_PRINTK("[ATBE] Error: pts example %s failed!", action[op]);
			return -1;
		} else {
			AT_PRINTK("[ATBE] pts example %s OK!", action[op]);
		}
	} else {
		AT_PRINTK("[ATBE] Error: wrong pts atcmd!");
	}
	return 0;
}