#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rtw_nan_vender_ioctrl_struc.h"

u8 nan_addr[ETH_ALEN] = {0xde, 0xe9, 0x94, 0x4d, 0x3f, 0xc4};
u8 dst_addr[ETH_ALEN] = {0xde, 0xe9, 0x94, 0x4d, 0x3f, 0x5e};

struct dns_srvc_info {
	u8 service_name[MAX_SRVC_NAME_LEN];
	u8 service_instance[MAX_SRVC_INST_LEN];
	u8 text_info[MAX_SRVC_TEXT_LEN];
	u8 hostname[NAN_MAX_HOSTNAME_LEN];
	u16 interface_id[MAX_INTERFACE_ID_LEN];
	u16 port_number;
};

int mac_addr_a2n(unsigned char *mac_addr, char *arg)
{
	int i;

	for (i = 0; i < ETH_ALEN ; i++) {
		unsigned int temp;
		char *cp = strchr(arg, ':');
		if (cp) {
			*cp = 0;
			cp++;
		}
		if (sscanf(arg, "%x", &temp) != 1) {
			return -1;
		}
		if (temp > 255) {
			return -1;
		}

		mac_addr[i] = temp;
		if (!cp) {
			break;
		}
		arg = cp;
	}
	if (i < ETH_ALEN - 1) {
		return -1;
	}

	return 0;
}

int strhex_2_realhex(char *input, char *pattern, int pattern_len, char token)
{
	char *cp = NULL;
	int i = 0, num_byte = 0;

	for (i = 0; i < pattern_len ; i++) {
		unsigned int temp;
		char *cp = strchr(input, token);

		if (cp) {
			*cp = 0;
			cp++;
		}

		if (sscanf(input, "%x", &temp) != 1) {
			input = ++cp;
			continue;
		}

		printf("%d: pattern[%d] = %x\n", i, num_byte, temp);

		if (temp > 255) {
			input = ++cp;
			continue;
		}

		pattern[num_byte++] = temp;
		if (!cp) {
			break;
		}
		input = cp;
	}

	if (num_byte != pattern_len) {
		return -1;
	}

	return 0;
}

u16 construct_gen_srvc_info(struct dns_srvc_info *info, u8 *gen_srv_info);
u16 append_gen_sub_attr(u8 attr_id, u8 *pbuf, struct dns_srvc_info *info, u8 type);


void help_msg()
{
	printf("Main parameters\n\
ifname	<interface_name>\n\
type	<srvc_ext|data_req|data_rsp|data_end>\n\
For type srvc_ext:\n\
	[period <period>] \n\
	[event <\?\?\?>]\n\
	[proximity]\n\
	[data_path]\n\
	[awake_dw_int <awake_dw_int>]	Valid values are: 1, 2, 4, 8 and 16.\n\
	[further_srvc_disc]\n\
	[further_srvc_disc_func <followup|gas>]\n\
	[data_path_type <ndp|\?\?\?>]\n\
	[qos]\n\
	[sec_pmk <pmk>]\n\
	[type <ucast|\?>]\n\
	[publish_id <id>]\n\
	[mcast_addr <mac>]\n\
	[srvc_name <name>]\n\
	[name_of_srvc_inst <inst>]\n\
	[text_info <text>]\n\
For type data_req:\n\
	[req_type <ndp|\?\?\?>]\n\
	[rsp_nan_mac <mac>]\n\
	[sec]\n\
	[type <ucast|\?>]\n\
	[mc_id <id>]\n\
	[initiator_data_address <mac>]\n\
	[mcast_addr <mac>]\n\
	[min_duration <duration>]\n\
	[max_latency <latency>]\n\
	[sec_pmk <pmk>]\n\
	[srv_info]\n\
	[hostname <name>]\n\
	[port_num <num>]\n\
For type data_rsp:\n\
	[rsp_mode <peer|auto>]\n\
	[ndl_rsp <reject|accept|counter>]\n\
	[data_path_id <id>]\n\
	[publish_id <id>]\n\
	[m4_rsp_type <Accept|Reject|BadMic>]\n\
	[initiator_data_address <mac>]\n\
	[min_duration <duration>]\n\
	[max_latency <latency>]\n\
	[srv_info]\n\
	[hostname <name>]\n\
	[port_num <num>]\n\
For type data_end:\n\
	[ndp_id <id>]\n\
	[initiatorndi <mac>]\n\
	[type <ucast|\?>]\n\
	[status <status>]\n\
	[mc_id <id>]\n\
	[nmsg_id <id>]\n\
");
}

#ifdef NAN_CUSTOMER_NANDOW


void nandow_load_test_data(struct nan_customer_nandow *, char **argv, int argc);
void nandow_send_cmd(struct nan_customer_nandow *nandoow_cmd, void *input,
					 unsigned int input_len, char *tmp_file, char *cmdreply_file,
					 char *if_name, char *fname);
void nandow_parse_cmd_reply(struct nan_customer_nandow *, char *cmdreply_file);
void nandow_dump_test_data(struct nan_customer_nandow *nandow_test);

void nandow_pre_actions(struct nan_customer_nandow *nandow_test);
void nandow_post_actions(struct nan_customer_nandow *nandow_test);

#endif

int main(int argc, char *argv[])
{
	FILE *outfile;
	FILE *file;
	char if_name[20]  = {0};
	char tmp_file[20] = "nan_cfgvendor.dat";
	char cmdreply_file[20] = "cmdreply.dat";
	unsigned int cmd_id, no_input = 0xff;
	void *input = NULL;
	unsigned int input_len = 0;
	char fname[FILENAME_MAX], *lastSlash;
	char cmd[2 * FILENAME_MAX];

	struct rtw_phl_nan_srvc_ext_info ext_info;
	struct rtw_phl_nan_data_req_info req_info;
	struct rtw_phl_nan_data_rsp_info rsp_info;
	struct rtw_phl_nan_data_end_info end_info;
#ifdef CONFIG_NAN_PAIRING
	struct nan_vendor_tx_followup_info tx_info = {0};
	struct nan_pairing_start pasn_start_info;
#endif
#ifdef NAN_CUSTOMER_NANDOW
	struct nan_customer_nandow nandow_cmd;
#endif

	memcpy(fname, argv[0], strlen(argv[0]));
	lastSlash = strrchr(fname, '/');
	*(lastSlash + 1) = '\0';
	strcat(fname, "iw");

	argv++;
	argc--;
	if (argc == 0) {
		help_msg();
		return 0;
	}

	if (argc > 1 && strcmp(argv[0], "ifname") == 0) {
		argv++;
		argc--;
		if (strlen(argv[0]) < 20) {
			memcpy(if_name, argv[0], strlen(argv[0]));
		} else {
			printf("ifname too long!\n");
			return -1;
		}
		argv++;
		argc--;
	} else {
		printf("No ifname!\n");
		return -1;
	}

	if (argc > 1 && strcmp(argv[0], "type") == 0) {
		argv++;
		argc--;
		if (strcmp(argv[0], "srvc_ext") == 0) {
			cmd_id = NAN_SUBCMD_SRVC_EXT_INFO;
		} else if (strcmp(argv[0], "data_req") == 0) {
			cmd_id = NAN_SUBCMD_DATA_REQ;
		} else if (strcmp(argv[0], "data_rsp") == 0) {
			cmd_id = NAN_SUBCMD_DATA_RSP;
		} else if (strcmp(argv[0], "data_end") == 0) {
			cmd_id = NAN_SUBCMD_DATA_END;
		}
#ifdef NAN_CUSTOMER_NANDOW
		else if (strcmp(argv[0], "cus_nandow") == 0) {
			cmd_id = NAN_SUBCMD_CUSTOMER_NANDOW;
		}
#endif
#ifdef CONFIG_NAN_PAIRING
		else if (strcmp(argv[0], "follow_up") == 0) {
			cmd_id = NAN_SUBCMD_FOLLOWUP;
		} else if (strcmp(argv[0], "pasn_setpw") == 0) {
			cmd_id = NAN_SUBCMD_PAIRING_SET_PW;
		} else if (strcmp(argv[0], "pasn_start") == 0) {
			cmd_id = NAN_SUBCMD_PASN_START;
		}
#endif
		else {
			printf("not supported cmd(%s)!\n", argv[0]);
			return -1;
		}
		argv++;
		argc--;
	} else {
		printf("No type!\n");
		return -1;
	}

	/* Parsing parameters for srvc ext info */
	if (cmd_id == NAN_SUBCMD_SRVC_EXT_INFO) {
		struct dns_srvc_info dns_info;
		memset(&ext_info, 0, sizeof(struct rtw_phl_nan_srvc_ext_info));
		memset(&dns_info, 0, sizeof(struct dns_srvc_info));
		input_len = sizeof(struct rtw_phl_nan_srvc_ext_info);
		while (argc > 0) {
			if (argc > 1 && strcmp(argv[0], "period") == 0) {
				argv++;
				argc--;

				ext_info.period = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "event") == 0) {
				argv++;
				argc--;

				ext_info.event_condition = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 0 && strcmp(argv[0], "proximity") == 0) {
				argv++;
				argc--;

				ext_info.proximity_flag = 1;
			} else if (argc > 0 && strcmp(argv[0], "data_path") == 0) {
				argv++;
				argc--;

				ext_info.data_path_flag = 1;
			} else if (argc > 1 && strcmp(argv[0], "awake_dw_int") == 0) {
				argv++;
				argc--;

				ext_info.awake_dw_int = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 0 && strcmp(argv[0], "further_srvc_disc") == 0) {
				argv++;
				argc--;

				ext_info.further_srvc_disc = 1;
			} else if (argc > 1 && strcmp(argv[0], "further_srvc_disc_func") == 0) {
				argv++;
				argc--;

				if (strcmp(argv[0], "followup") == 0) {
					ext_info.further_srvc_disc_func = 0;
				} else if (strcmp(argv[0], "gas") == 0) {
					ext_info.further_srvc_disc_func = 1;
				} else {
					return -1;
				}
				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "data_path_type") == 0) {
				argv++;
				argc--;

				if (strcmp(argv[0], "ndp") == 0) {
					ext_info.data_path_type = 0;
				} else {
					return -1;
				}
				argv++;
				argc--;
			} else if (argc > 0 && strcmp(argv[0], "qos") == 0) {
				argv++;
				argc--;

				ext_info.qos = 1;
			} else if (argc > 1 && strcmp(argv[0], "sec_pmk") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) == 32) {
					ext_info.sec = 1;
					memcpy(ext_info.pmk_publish, argv[0], strlen(argv[0]));
				}

				argv++;
				argc--;
			}  else if (argc > 1 && strcmp(argv[0], "sec_password") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) <= MAX_PASSPHRASE_LEN) {
					ext_info.sec = 1;
					ext_info.passphrase_len = strlen(argv[0]);
					memcpy(ext_info.passphrase,
						   argv[0],
						   ext_info.passphrase_len);
				}

				argv++;
				argc--;
			} else if (argc > 0 && strcmp(argv[0], "gtk") == 0) {
				argv++;
				argc--;

				ext_info.gtk_protection = 1;
			} else if (argc > 1 && strcmp(argv[0], "srv_id") == 0) {
				argv++;
				argc--;

				if (strhex_2_realhex(argv[0], ext_info.service_id, NAN_SRVC_ID_LEN, ',')) {
					fprintf(stderr, "Invalid srv_id\n");
					return 2;
				}
				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "srv_name") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) > MAX_SRVC_NAME_LEN) {
					fprintf(stderr, "Invalid srv_name, len= %ld\n", strlen(argv[0]));
					return 2;
				}

				memcpy(dns_info.service_name, argv[0], strlen(argv[0]));

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "srv_inst") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) > MAX_SRVC_INST_LEN) {
					fprintf(stderr, "Invalid srvc name of inst, len= %ld\n", strlen(argv[0]));
					return 2;
				}
				memcpy(dns_info.service_instance, argv[0], strlen(argv[0]));

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "srv_text") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) > MAX_SRVC_TEXT_LEN) {
					fprintf(stderr, "Invalid srvc text info, len= %ld\n", strlen(argv[0]));
					return 2;
				}

				memcpy(dns_info.text_info, argv[0], strlen(argv[0]));

				argv++;
				argc--;
			}
#ifdef CONFIG_NAN_PAIRING
			else if (argc > 1 && strcmp(argv[0], "csid") == 0) {
				argv++;
				argc--;

				ext_info.csid = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "bstrap_method") == 0) {
				argv++;
				argc--;

				ext_info.bstrap_method = atoi(argv[0]);
				ext_info.pairing_setup_enable = 1;
				ext_info.npk_nik_cache_enable = 1;

				argv++;
				argc--;
			} else if (argc > 0 && strcmp(argv[0], "npk_nik") == 0) {
				argv++;
				argc--;

				ext_info.npk_nik_cache_enable = 1;
			}
#endif
			else {
				printf(" srvc ext info: invalid param %s!\n", argv[0]);
				argv++;
				argc--;
			}
		}
		if (memcmp(&dns_info, &(struct dns_srvc_info) {
		0
	}, sizeof(dns_info))) {

			ext_info.srvc_info_len = construct_gen_srvc_info(&dns_info, ext_info.srvc_info);

		}
		input = &ext_info;
	}

	if (cmd_id == NAN_SUBCMD_DATA_REQ) {
		memset(&req_info, 0, sizeof(struct rtw_phl_nan_data_req_info));
		input_len = sizeof(struct rtw_phl_nan_data_req_info);
		struct dns_srvc_info dns_info;
		memset(&dns_info, 0, sizeof(struct dns_srvc_info));

		while (argc > 0) {
			if (argc > 1 && strcmp(argv[0], "req_type") == 0) {
				argv++;
				argc--;

				if (strcmp(argv[0], "ndp") == 0) {
					req_info.req_type = 0;
				} else {
					return -1;
				}
				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "rsp_nan_mac") == 0) {
				argv++;
				argc--;

				if (mac_addr_a2n(req_info.rsp_nan_mac, argv[0])) {
					fprintf(stderr, "Invalid MAC address\n");
					return 2;
				}

				argv++;
				argc--;
			} else if (argc > 0 && strcmp(argv[0], "sec") == 0) {
				argv++;
				argc--;

				req_info.sec = 1;
			} else if (argc > 1 && strcmp(argv[0], "type") == 0) {
				argv++;
				argc--;

				if (strcmp(argv[0], "ucast") == 0) {
					req_info.type = 0;
				} else {
					return -1;
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "publish_id") == 0) {
				argv++;
				argc--;

				req_info.publish_id = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "mcast_addr") == 0) {
				argv++;
				argc--;

				if (mac_addr_a2n(req_info.mcast_addr, argv[0])) {
					fprintf(stderr, "Invalid MAC address\n");
					return 2;
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "min_duration") == 0) {
				argv++;
				argc--;

				req_info.qos_req.min_duration = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "max_latency") == 0) {
				argv++;
				argc--;

				req_info.qos_req.max_latency = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "sec_pmk") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) == 32) {
					req_info.sec_type = NAN_PMK_SET_BY_USER_PMK;
					memcpy(req_info.sec_info, argv[0], strlen(argv[0]));
					req_info.sec_info_len = 32;
				} else if (strcmp(argv[0], "pairing") == 0) {
					req_info.sec_type = NAN_PMK_SET_BY_PAIRING;
				} else {
					printf("the length of sec_pmk is not 32!\n");
					return -1;
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "sec_password") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) <= MAX_PASSPHRASE_LEN) {
					req_info.sec_type =
						NAN_PMK_SET_BY_USER_PASSPRHRAE;
					req_info.sec_info_len = strlen(argv[0]);
					memcpy(req_info.sec_info,
						   argv[0],
						   req_info.sec_info_len);
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "mrole_sched_ratio") == 0) {
				argv++;
				argc--;

				req_info.mrole_sched_ratio = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "srv_info") == 0) {
				argv++;
				argc--;
				char *srv_info = argv[0];
				int i = 0, len_count = 0;
				for (i = 0 ; i < strlen(srv_info); i++) {
					if (srv_info[i] == ',') {
						len_count++;
					}
				}
				if (len_count) {
					len_count++;
				}
				printf("count = %d\n", len_count);
				req_info.srvc_info_len = len_count;
				if (strhex_2_realhex(srv_info, req_info.srvc_info,
									 req_info.srvc_info_len, ',')) {
					fprintf(stderr, "Invalid srv_info\n");
				}
				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "hostname") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) > MAX_SRVC_NAME_LEN) {
					fprintf(stderr, "Invalid hostname, len= %ld\n", strlen(argv[0]));
					return 2;
				}

				memcpy(dns_info.hostname, argv[0], strlen(argv[0]));

				argv++;
				argc--;

			} else if (argc > 1 && strcmp(argv[0], "port_num") == 0) {
				argv++;
				argc--;

				dns_info.port_number = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "interface_id") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) > MAX_INTERFACE_ID_LEN) {
					fprintf(stderr, "Invalid interface_id, len= %ld\n", strlen(argv[0]));
					return 2;
				}

				memcpy(dns_info.interface_id, argv[0], strlen(argv[0]));

				argv++;
				argc--;
			} else {
				printf(" DATA_RSP: invalid param %s!\n", argv[0]);
				argv++;
				argc--;
			}
		}
		if (memcmp(&dns_info, &(struct dns_srvc_info) {
		0
	}, sizeof(dns_info))) {

			req_info.srvc_info_len = construct_gen_srvc_info(&dns_info, req_info.srvc_info);

		}
		input = &req_info;
	}

	if (cmd_id == NAN_SUBCMD_DATA_RSP) {
		memset(&rsp_info, 0, sizeof(struct rtw_phl_nan_data_rsp_info));
		input_len = sizeof(struct rtw_phl_nan_data_rsp_info);
		struct dns_srvc_info dns_info;
		memset(&dns_info, 0, sizeof(struct dns_srvc_info));

		while (argc > 0) {
			if (argc > 1 && strcmp(argv[0], "rsp_mode") == 0) {
				argv++;
				argc--;

				if (strcmp(argv[0], "peer") == 0) {
					rsp_info.rsp_mode = 0;
				} else if (strcmp(argv[0], "auto") == 0) {
					rsp_info.rsp_mode = 1;
				} else {
					return -1;
				}
				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "ndl_rsp") == 0) {
				argv++;
				argc--;

				if (strcmp(argv[0], "reject") == 0) {
					rsp_info.ndl_rsp = 1;
				} else if (strcmp(argv[0], "accept") == 0) {
					rsp_info.ndl_rsp = 2;
				} else if (strcmp(argv[0], "counter") == 0) {
					rsp_info.ndl_rsp = 3;
				} else {
					return -1;
				}
				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "data_path_id") == 0) {
				argv++;
				argc--;

				rsp_info.data_path_id = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "publish_id") == 0) {
				argv++;
				argc--;

				rsp_info.publish_id = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "m4_rsp_type") == 0) {
				argv++;
				argc--;

				if (strcmp(argv[0], "Accept") == 0) {
					rsp_info.m4_rsp_type = 1;
				} else if (strcmp(argv[0], "Reject") == 0) {
					rsp_info.m4_rsp_type = 2;
				} else if (strcmp(argv[0], "BadMic") == 0) {
					rsp_info.m4_rsp_type = 3;
				} else {
					return -1;
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "type") == 0) {
				argv++;
				argc--;

				if (strcmp(argv[0], "ucast") == 0) {
					rsp_info.type = 0;
				} else {
					return -1;
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "mc_id") == 0) {
				argv++;
				argc--;

				rsp_info.mc_id = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "initiator_data_address") == 0) {
				argv++;
				argc--;

				if (mac_addr_a2n(rsp_info.initiator_data_address, argv[0])) {
					fprintf(stderr, "Invalid MAC address\n");
					return 2;
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "mcast_addr") == 0) {
				argv++;
				argc--;

				if (mac_addr_a2n(rsp_info.mcast_addr, argv[0])) {
					fprintf(stderr, "Invalid MAC address\n");
					return 2;
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "min_duration") == 0) {
				argv++;
				argc--;

				rsp_info.qos_req.min_duration = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "max_latency") == 0) {
				argv++;
				argc--;

				rsp_info.qos_req.max_latency = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "mrole_sched_ratio") == 0) {
				argv++;
				argc--;

				rsp_info.mrole_sched_ratio = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "srv_info") == 0) {
				argv++;
				argc--;
				char *srv_info = argv[0];
				int i = 0, len_count = 0;
				for (i = 0 ; i < strlen(srv_info); i++) {
					if (srv_info[i] == ',') {
						len_count++;
					}
				}
				if (len_count) {
					len_count++;
				}
				printf("count = %d\n", len_count);
				rsp_info.srvc_info_len = len_count;
				if (strhex_2_realhex(srv_info, rsp_info.srvc_info,
									 rsp_info.srvc_info_len, ',')) {
					fprintf(stderr, "Invalid srv_info\n");
				}
				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "hostname") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) > MAX_SRVC_NAME_LEN) {
					fprintf(stderr, "Invalid hostname, len= %ld\n", strlen(argv[0]));
					return 2;
				}

				memcpy(dns_info.hostname, argv[0], strlen(argv[0]));

				argv++;
				argc--;

			} else if (argc > 1 && strcmp(argv[0], "port_num") == 0) {
				argv++;
				argc--;

				dns_info.port_number = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "interface_id") == 0) {
				argv++;
				argc--;

				if (strlen(argv[0]) > MAX_INTERFACE_ID_LEN) {
					fprintf(stderr, "Invalid interface_id, len= %ld\n", strlen(argv[0]));
					return 2;
				}

				memcpy(dns_info.interface_id, argv[0], strlen(argv[0]));

				argv++;
				argc--;
			} else {
				printf(" DATA_RSP: invalid param %s!\n", argv[0]);
				argv++;
				argc--;
			}
		}
		if (memcmp(&dns_info, &(struct dns_srvc_info) {
		0
	}, sizeof(dns_info))) {

			rsp_info.srvc_info_len = construct_gen_srvc_info(&dns_info, rsp_info.srvc_info);

		}
		input = &rsp_info;
	}

	if (cmd_id == NAN_SUBCMD_DATA_END) {
		memset(&end_info, 0, sizeof(struct rtw_phl_nan_data_end_info));
		input_len = sizeof(struct rtw_phl_nan_data_end_info);

		if (argc > 1 && strcmp(argv[0], "ndp_id") == 0) {
			argv++;
			argc--;

			end_info.ndp_id = atoi(argv[0]);

			argv++;
			argc--;
		}

		if (argc > 1 && strcmp(argv[0], "initiatorndi") == 0) {
			argv++;
			argc--;

			if (mac_addr_a2n(end_info.initiatorndi, argv[0])) {
				fprintf(stderr, "Invalid MAC address\n");
				return 2;
			}

			argv++;
			argc--;
		}

		if (argc > 1 && strcmp(argv[0], "type") == 0) {
			argv++;
			argc--;

			if (strcmp(argv[0], "ucast") == 0) {
				end_info.type = 0;
			} else {
				return -1;
			}

			argv++;
			argc--;
		}

		if (argc > 1 && strcmp(argv[0], "status") == 0) {
			argv++;
			argc--;

			end_info.status = atoi(argv[0]);

			argv++;
			argc--;
		}

		if (argc > 1 && strcmp(argv[0], "mc_id") == 0) {
			argv++;
			argc--;

			end_info.mc_id = atoi(argv[0]);

			argv++;
			argc--;
		}

		if (argc > 1 && strcmp(argv[0], "nmsg_id") == 0) {
			argv++;
			argc--;

			end_info.nmsg_id = atoi(argv[0]);

			argv++;
			argc--;
		}

		input = &end_info;
	}

	if (cmd_id == NAN_SUBCMD_FOLLOWUP) {
#ifdef CONFIG_NAN_PAIRING

		input_len = sizeof(struct nan_vendor_tx_followup_info);
		while (argc > 0) {
			if (argc > 1 && strcmp(argv[0], "rmt_id") == 0) {
				argv++;
				argc--;

				tx_info.remote_inst_id = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "local_id") == 0) {
				argv++;
				argc--;

				tx_info.local_inst_id = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "rmt_mac") == 0) {
				argv++;
				argc--;
				if (mac_addr_a2n(tx_info.remote_mac, argv[0])) {
					fprintf(stderr, "Invalid MAC address\n");
					return 2;
				}
				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "type") == 0) {
				argv++;
				argc--;

				if (strcmp(argv[0], "bstrap") == 0) {
					tx_info.follow_up_type = NAN_TX_FOLLOW_UP_BSTRP;
				} else if (strcmp(argv[0], "request") == 0) {
					tx_info.follow_up_type = NAN_TX_FOLLOW_UP_REQ;
				} else if (strcmp(argv[0], "response") == 0) {
					tx_info.follow_up_type = NAN_TX_FOLLOW_UP_RSP;
				} else {
					printf("followup: type with invalid value!\n");
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "bstrap_type") == 0) {
				argv++;
				argc--;

				if (strcmp(argv[0], "rsp") == 0) {
					SET_BSTRAP_TYPE_RSP(tx_info.bstrap_type_status);
				} else if (strcmp(argv[0], "req") == 0) {
					SET_BSTRAP_TYPE_REQ(tx_info.bstrap_type_status);
				} else {
					printf("followup: bstrap_type with invalid value!\n");
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "bstrap_status") == 0) {
				argv++;
				argc--;

				if (atoi(argv[0]) > 2) {
					printf("followup: bstrap_status with invalid value!\n");
				} else {
					SET_BSTRAP_STATUS(tx_info.bstrap_type_status, atoi(argv[0]));
				}

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "bstrap_method") == 0) {
				argv++;
				argc--;

				tx_info.follow_up_type = NAN_TX_FOLLOW_UP_BSTRP;
				tx_info.bstrap_method = atoi(argv[0]);

				argv++;
				argc--;
			} else if (argc > 1 && strcmp(argv[0], "sec_key_type") == 0) {
				argv++;
				argc--;

				tx_info.sec_key_type = atoi(argv[0]);

				argv++;
				argc--;
			} else {
				printf("followup: invalid param!\n");
				argv++;
				argc--;
			}
		}
#if 0
		if (IS_BSTRAP_COMEBACK_REQ(tx_info.bstrap_type_status)) {
			printf("followup: skip bstrap comeback req!\n");
			return 0;
		}
		if (IS_BSTRAP_TYPE_REQ(tx_info.bstrap_type_status) &&
			GET_BSTRAP_STATUS(tx_info.bstrap_type_status)) {
			printf("followup: reset bstrap_status to 0!\n");
			SET_BSTRAP_STATUS(tx_info.bstrap_type_status, 0);
		}
#endif
		input = &tx_info;
#endif
	}

#ifdef CONFIG_NAN_PAIRING

	if (cmd_id == NAN_SUBCMD_PASN_START) {
		memset(&pasn_start_info, 0, sizeof(struct nan_pairing_start));
		input_len = sizeof(struct nan_pairing_start);
		if (argc > 1 && strcmp(argv[0], "role") == 0) {
			argv++;
			argc--;

			pasn_start_info.role = atoi(argv[0]);

			argv++;
			argc--;
		}
		if (argc > 1 && strcmp(argv[0], "type") == 0) {
			argv++;
			argc--;

			pasn_start_info.type = atoi(argv[0]);

			argv++;
			argc--;
		}
		if (argc > 0 && strcmp(argv[0], "peer_nmi") == 0) {
			argv++;
			argc--;

			if (mac_addr_a2n(pasn_start_info.peer_nmi, argv[0])) {
				fprintf(stderr, "Invalid MAC address\n");
				return 2;
			}

			argv++;
			argc--;
		}
		if (argc > 0 && strcmp(argv[0], "auth_method") == 0) {
			argv++;
			argc--;

			pasn_start_info.auth_method = atoi(argv[0]);

			argv++;
			argc--;
		}
		if (argc > 1 && strcmp(argv[0], "nik_caching") == 0) {
			argv++;
			argc--;

			pasn_start_info.nik_caching = atoi(argv[0]);

			argv++;
			argc--;
		}

		input = &pasn_start_info;
	}
#endif /* CONFIG_NAN_PAIRING */

#ifdef NAN_CUSTOMER_NANDOW
	if (cmd_id == NAN_SUBCMD_CUSTOMER_NANDOW) {
		memset(&nandow_cmd, 0, sizeof(struct nan_customer_nandow));
		input_len = sizeof(struct nan_customer_nandow);
		fprintf(stderr, "input_len=%d\n", input_len);

		if (argc > 1 && strcmp(argv[0], "cmd_id") == 0) {
			argv++;
			argc--;

			nandow_cmd.cmd_id = atoi(argv[0]);

			argv++;
			argc--;
		}

		if (argc > 0 && strcmp(argv[0], "get") == 0) {
			argv++;
			argc--;

			nandow_cmd.cmd_type = NANDOW_CMD_TYPE_GET;
			nandow_pre_actions(&nandow_cmd);
			input = &nandow_cmd;
			nandow_send_cmd(&nandow_cmd, input, input_len, tmp_file, cmdreply_file, if_name, fname);
			nandow_parse_cmd_reply(&nandow_cmd, cmdreply_file);

		} else if (argc >= 1 && strcmp(argv[0], "set") == 0) {
			argv++;
			argc--;

			nandow_cmd.cmd_type = NANDOW_CMD_TYPE_SET;
			nandow_pre_actions(&nandow_cmd);
			nandow_load_test_data(&nandow_cmd, argv, argc);
			input = &nandow_cmd;
			nandow_send_cmd(&nandow_cmd, input, input_len, tmp_file, cmdreply_file, if_name, fname);
		}

		nandow_dump_test_data(&nandow_cmd);
		//nandow_post_actions(&nandow_cmd);
	} else
#endif
	{
		if (input == NULL) {
			input = &no_input;
			input_len = sizeof(unsigned int);
		}

		// open file for writing
		outfile = fopen(tmp_file, "w");
		if (outfile == NULL) {
			fprintf(stderr, "\nError opend file\n");
			exit(1);
		}

		// write struct to file
		fwrite(input, input_len, 1, outfile);

		if (fwrite != 0) {
			printf("contents to file written successfully !\n");
		} else {
			printf("error writing file !\n");
		}

		// close file
		fclose(outfile);

		if ((file = fopen(fname, "r"))) {
			sprintf(cmd, "%s %s vendor send 0x%x 0x%x %s", fname, if_name, OUI_REALTEK, cmd_id, tmp_file);
			printf("[rtw_cmd]%s\n", cmd);
			printf("Use iw in package\n");
			fclose(file);
		} else {
			sprintf(cmd, "iw %s vendor send 0x%x 0x%x %s", if_name, OUI_REALTEK, cmd_id, tmp_file);
			printf("Use iw in system\n");
		}

		system(cmd);
		remove(tmp_file);
	}

	return 0;
}

u16 cvert_text_info_str(u8 *txt_str, u8 *out_buf, u16 out_buf_len)
{
	char *pch = NULL;
	u16 txt_len = 0;

	if (NULL == txt_str || NULL == out_buf || 0 == out_buf_len) {
		printf("cvert text info err! input error\n");
		return 0;
	}

	pch = strtok(txt_str, " |");
	while (pch) {
		/* txt length */
		/* printf("length:%s\n", pch); */
		if ((txt_len + 1) > out_buf_len) {
			printf("cvert text info err! str is too long, len(%d) > out_buf_len(%d)\n",
				   txt_len + 1, out_buf_len);
			break;
		}
		if (sscanf(pch, "0x%hhx", (unsigned char *)(out_buf + txt_len)) != 1) {
			out_buf[txt_len] = atoi(pch);
		}
		txt_len++;
		pch = strtok(NULL, " |");
		if (!pch) {
			break;
		}

		/* txt data */
		/* printf("data:%s\n", pch); */
		if ((txt_len + strlen(pch)) > out_buf_len) {
			printf("cvert text info err! str is too long, len(%ld) > out_buf_len(%d)\n",
				   (txt_len + strlen(pch)), out_buf_len);
			break;
		}
		strcpy(out_buf + txt_len, pch);
		txt_len += strlen(pch);
		pch = strtok(NULL, " |");
	}

	return txt_len;
}

u16 append_gen_sub_attr(u8 attr_id, u8 *pbuf, struct dns_srvc_info *info, u8 type)
{
	u16 len = 0, attr_len = 0, i = 0;
	u8 text_info[MAX_SRVC_TEXT_LEN] = {0};

	switch (attr_id) {
	case NAN_GEN_SRVC_TRANSPORT_PORT:
		printf("append_gen_sub_attr NAN_GEN_SRVC_TRANSPORT_PORT[%d]\n",
			   attr_id);
		printf("port_number:%d\n", info->port_number);
		*pbuf = NAN_GEN_SRVC_TRANSPORT_PORT;
		attr_len = 2;
		memcpy(pbuf + 1, &attr_len, 2);
		memcpy(pbuf + 3, &info->port_number, attr_len);
		len = attr_len + 3;
		break;
	case NAN_GEN_SRVC_TRANSPORT_PROTOCOL:
		printf("append_gen_sub_attr NAN_GEN_SRVC_TRANSPORT_PROTOCOL[%d](skipped)\n", attr_id);
		break;
	case NAN_GEN_SRVC_SERVICE_NAME:
		printf("append_gen_sub_attr NAN_GEN_SRVC_SERVICE_NAME[%d]\n", attr_id);
		printf("service_name:%s\n", info->service_name);
		*pbuf = NAN_GEN_SRVC_SERVICE_NAME;
		attr_len = strlen(info->service_name);
		memcpy(pbuf + 1, &attr_len, 2);
		memcpy(pbuf + 3, info->service_name, attr_len);
		len = attr_len + 3;
		break;
	case NAN_GEN_SRVC_NAME_OF_SRVC_INST:
		printf("append_gen_sub_attr NAN_GEN_SRVC_NAME_OF_SRVC_INST[%d]\n", attr_id);
		*pbuf = NAN_GEN_SRVC_NAME_OF_SRVC_INST;
		if (type == NAN_WFA_SRVC_ATTR_SEDA) {
			printf("service inst:%s\n", info->service_instance);
			attr_len = strlen(info->service_instance);
			memcpy(pbuf + 1, &attr_len, 2);
			memcpy(pbuf + 3, info->service_instance, attr_len);
		} else {
			printf("hostname:%s\n", info->hostname);
			attr_len = strlen(info->hostname);
			memcpy(pbuf + 1, &attr_len, 2);
			memcpy(pbuf + 3, info->hostname, attr_len);
		}
		len = attr_len + 3;
		break;
	case NAN_GEN_SRVC_TEXT_INFO:
		printf("append_gen_sub_attr NAN_GEN_SRVC_TEXT_INFO[%d]\n", attr_id);
		printf("text info:%s\n", info->text_info);

		attr_len = cvert_text_info_str(info->text_info, text_info, MAX_SRVC_TEXT_LEN);
		if (!attr_len) {
			printf("attr_len = 0, skip\n");
			break;
		}
		*pbuf = NAN_GEN_SRVC_TEXT_INFO;
		memcpy(pbuf + 1, &attr_len, 2);
		memcpy(pbuf + 3, text_info, attr_len);
		len = attr_len + 3;
		break;
	case NAN_GEN_SRVC_UUID:
		printf("append_gen_sub_attr NAN_GEN_SRVC_UUID[%d] (skipped)\n", attr_id);
		break;
	case NAN_GEN_SRVC_BLOB:
		printf("append_gen_sub_attr NAN_GEN_SRVC_BLOB[%d] (skipped)\n", attr_id);
		break;
	case NAN_GEN_SRVC_RESERVD:
		printf("append_gen_sub_attr NAN_GEN_SRVC_RESERVD[%d] (skipped)\n", attr_id);
		break;
	case NAN_GEN_SRVC_VENDOR_SPEC_INFO:
		printf("append_gen_sub_attr NAN_GEN_SRVC_VENDOR_SPEC_INFO[%d] (skipped)\n",
			   attr_id);
		break;
	default:
		printf("append_gen_sub_attr unknow sub attribute id[%d]\n", attr_id);
		break;
	}

	printf("len = %d", len);
	for (i = 0; i < len; i++) {
		if (!(i % 16)) {
			printf("\n");
		}
		printf("%02x ", pbuf[i]);
	}
	printf("\n");

	return len;
}

u16 construct_gen_srvc_info(struct dns_srvc_info *info, u8 *gen_srv_info)
{
	u16 len = 0;
	u8 temp_srv_info[NAN_MAX_SERVICE_INFO_LEN] = {0};
	u16 idx = 0, total_len = 0;
	int i = 0;

	/* Wi-Fi Aware specificcation v3.2 9.5.4.1*/
	/* WFA OUI = 0x50, 0x6F, 0x9A */
	memcpy(&temp_srv_info[0], WFA_OUI, 3);
	idx += 3;

	/* Service Protocol Type */
	temp_srv_info[idx] = NAN_SRVC_PROTOCOL_GENIC;
	idx += 1;

	/* Generic Service Protocol sub-attribute */
	if (info->service_name && strlen(info->service_name)) {
		len = append_gen_sub_attr(NAN_GEN_SRVC_SERVICE_NAME,
								  &temp_srv_info[idx],
								  info,
								  NAN_WFA_SRVC_ATTR_SEDA);
		idx += len;
	}
	if (info->service_instance && strlen(info->service_instance)) {
		len = append_gen_sub_attr(NAN_GEN_SRVC_NAME_OF_SRVC_INST,
								  &temp_srv_info[idx],
								  info,
								  NAN_WFA_SRVC_ATTR_SEDA);
		idx += len;
	}
	if (info->text_info && strlen(info->text_info)) {
		len = append_gen_sub_attr(NAN_GEN_SRVC_TEXT_INFO,
								  &temp_srv_info[idx],
								  info,
								  NAN_WFA_SRVC_ATTR_SEDA);
		idx += len;
	}
	if (info->hostname && strlen(info->hostname)) {
		len = append_gen_sub_attr(NAN_GEN_SRVC_NAME_OF_SRVC_INST,
								  &temp_srv_info[idx],
								  info,
								  NAN_WFA_SRVC_ATTR_NDPE);
		idx += len;
	}
	if (info->port_number) {
		len = append_gen_sub_attr(NAN_GEN_SRVC_TRANSPORT_PORT,
								  &temp_srv_info[idx],
								  info,
								  NAN_WFA_SRVC_ATTR_NDPE);
		idx += len;
	}

	total_len = idx;
	if (total_len > NAN_MAX_SERVICE_INFO_LEN) {
		printf("service info length(%d) is out of limit\n", total_len);
		return 0;
	}

	memcpy(gen_srv_info, temp_srv_info, total_len);

	printf("generic service info (len=%d)", total_len);
	for (i = 0; i < total_len; i++) {
		if (!(i % 16)) {
			printf("\n");
		}
		printf("%02x ", gen_srv_info[i]);
	}
	printf("\n");

	return total_len;
}


#ifdef NAN_CUSTOMER_NANDOW

/* sub function prototype */

void nandow_load_test_data_nan_init(struct nan_customer_nandow *nandow_test);
void nandow_dump_test_data_nan_init(struct nan_customer_nandow *nandow_test);

void nandow_load_test_data_get_drv_cap(struct nan_customer_nandow *nandow_test);
void nandow_dump_test_data_get_drv_cap(struct nan_customer_nandow *nandow_test);

void nandow_load_test_data_nan_enabled(struct nan_customer_nandow *nandow_test, char **argv, int argc);
void nandow_dump_test_data_nan_enabled(struct nan_customer_nandow *nandow_test);

void nandow_load_data_cluster_id(struct nan_customer_nandow *nandow_test,
								 char **argv, int argc);
void nandow_dump_data_cluster_id(struct nan_customer_nandow *nandow_test);

void nandow_load_data_disc_period(struct nan_customer_nandow *nandow_test,
								  char **argv,
								  int argc);
void nandow_dump_data_disc_period(struct nan_customer_nandow *nandow_test);

void nandow_load_forced_bcn_trans(struct nan_customer_nandow *nandow_test, char **argv, int argc);
void nandow_dump_data_forced_bcn_trans(struct nan_customer_nandow *nandow_test);

void nandow_load_data_master_pref(struct nan_customer_nandow *nandow_test,
								  char **argv, int argc);
void nandow_dump_data_master_pref(struct nan_customer_nandow *nandow_test);

void nandow_load_data_random_factor(struct nan_customer_nandow *nandow_test,
									char **argv, int argc);
void nandow_dump_data_random_factor(struct nan_customer_nandow *nandow_test);

void nandow_load_data_dw_awake_period(struct nan_customer_nandow *nandow_test,
									  char **argv,
									  int argc);
void nandow_dump_data_dw_awake_period(struct nan_customer_nandow *nandow_test);

void nandow_load_data_primary_master_channel(struct nan_customer_nandow *nandow_test,
		char **argv,
		int argc);
void nandow_dump_data_primary_master_channel(struct nan_customer_nandow *nandow_test);
void nandow_load_data_secondary_master_channel(struct nan_customer_nandow *nandow_test,
		char **argv,
		int argc);
void nandow_dump_data_secondary_master_channel(struct nan_customer_nandow *nandow_test);


void nandow_load_test_data_subscribe(struct nan_customer_nandow *nandow_test, char **argv, int argc);
void nandow_dump_test_data_subscribe(struct nan_customer_nandow *nandow_test);

void nandow_load_test_data_publish(struct nan_customer_nandow *nandow_test, char **argv, int argc);
void nandow_dump_test_data_publish(struct nan_customer_nandow *nandow_test);

void nandow_load_test_data_cancel_publish(struct nan_customer_nandow *nandow_test);
void nandow_dump_test_data_cancel_publish(struct nan_customer_nandow *nandow_test);

void nandow_load_test_data_cancel_subscribe(struct nan_customer_nandow *nandow_test);
void nandow_dump_test_data_cancel_subscribe(struct nan_customer_nandow *nandow_test);

void nandow_load_test_data_follow_up_transmit(struct nan_customer_nandow *nandow_test);
void nandow_dump_test_data_follow_up_transmit(struct nan_customer_nandow *nandow_test);

void nandow_load_test_data_req(struct nan_customer_nandow *nandow_test, char **argv, int argc);
void nandow_dump_test_data_req(struct nan_customer_nandow *nandow_test);

void nandow_load_test_data_rsp(struct nan_customer_nandow *nandow_test, char **argv, int argc);
void nandow_dump_test_data_rsp(struct nan_customer_nandow *nandow_test);

void nandow_load_test_data_confirm(struct nan_customer_nandow *nandow_test, char **argv, int argc);
void nandow_dump_test_data_confirm(struct nan_customer_nandow *nandow_test);

void nandow_load_test_data_end(struct nan_customer_nandow *nandow_test);
void nandow_dump_test_data_end(struct nan_customer_nandow *nandow_test);

void nandow_load_test_ndc_avail(struct nan_customer_nandow *nandow_test, char **argv, int argc);
void nandow_dump_test_ndc_avail(struct nan_customer_nandow *nandow_test);

void nandow_load_test_set_scan_control(struct nan_customer_nandow *nandow_test);
void nandow_dump_test_set_scan_control(struct nan_customer_nandow *nandow_test);

void nandow_load_test_country_code(struct nan_customer_nandow *nandow_test);
void nandow_dump_test_country_code(struct nan_customer_nandow *nandow_test);


/* main function definition */

void _dump_service_info(struct rtw_nan_service_info *srv_info)
{

	printf("\t service_specific_info.len = %d \n", srv_info->length);
	printf("\t service_specific_info.info = \n");
	for (int i = 0 ; i < srv_info->length;) {

		printf("\t\t [0x%X] %02X %02X %02X %02X %02X %02X %02X %02X \n",
			   i,
			   srv_info->info[i],
			   srv_info->info[i + 1],
			   srv_info->info[i + 2],
			   srv_info->info[i + 3],
			   srv_info->info[i + 4],
			   srv_info->info[i + 5],
			   srv_info->info[i + 6],
			   srv_info->info[i + 7]);
		i = i + 8;
	}

}


void nandow_load_test_data(struct nan_customer_nandow *nandow_test, char **argv, int argc)
{

	switch (nandow_test->cmd_id) {
	case RTW_NAN_CMD_INIT:
		nandow_load_test_data_nan_init(nandow_test);
		break;
	case RTW_NAN_CMD_WIFI_DRV_CAP:
		nandow_load_test_data_get_drv_cap(nandow_test);
		break;
	case RTW_NAN_CMD_NAN_ENABLE:
		nandow_load_test_data_nan_enabled(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_CLUSTER_ID:
		nandow_load_data_cluster_id(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_DISC_BCN_PERIOD:
		nandow_load_data_disc_period(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_FORCED_DISC_BCN:
		nandow_load_forced_bcn_trans(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_MASTER_PREF:
		nandow_load_data_master_pref(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_RANDOM_FACTOR:
		nandow_load_data_random_factor(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_DW_AWAKE_PERIOD:
		nandow_load_data_dw_awake_period(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_PRI_MASTER_CH:
		nandow_load_data_primary_master_channel(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_SEC_MASTER_CH:
		nandow_load_data_secondary_master_channel(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_SUBSCRIBE:
		nandow_load_test_data_subscribe(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_PUBLISH:
		nandow_load_test_data_publish(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_CANCEL_SUBSCRIBE:
		nandow_load_test_data_cancel_subscribe(nandow_test);
		break;
	case RTW_NAN_CMD_CANCEL_PUBLISH:
		nandow_load_test_data_cancel_publish(nandow_test);
		break;
	case RTW_NAN_CMD_FOLLOWUP_TX:
		nandow_load_test_data_follow_up_transmit(nandow_test);
		break;
	case RTW_NAN_CMD_DATAPATH_REQ:
		nandow_load_test_data_req(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_DATAPATH_RSP:
		nandow_load_test_data_rsp(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_DATAPATH_CONFIRM:
		nandow_load_test_data_confirm(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_DATAPATH_END:
		nandow_load_test_data_end(nandow_test);
		break;
	case RTW_NAN_CMD_NDC_AVAIL:
		nandow_load_test_ndc_avail(nandow_test, argv, argc);
		break;
	case RTW_NAN_CMD_SCAN_CONTROL:
		nandow_load_test_set_scan_control(nandow_test);
		break;
	case RTW_NAN_CMD_COUNTRY_CODE:
		nandow_load_test_country_code(nandow_test);
		break;
	default:
		break;
	}
}

void nandow_pre_actions(struct nan_customer_nandow *nandow_test)
{
	switch (nandow_test->cmd_id) {
	case RTW_NAN_CMD_INIT:
		nandow_test->para_len = sizeof(struct rtw_nan_device_capability);
		break;
	case RTW_NAN_CMD_WIFI_DRV_CAP:
		nandow_test->para_len = sizeof(struct rtw_nan_driver_capabilities);
		break;
	case RTW_NAN_CMD_NAN_ENABLE:
		nandow_test->para_len = sizeof(struct rtw_nan_enable);
		break;
	case RTW_NAN_CMD_CLUSTER_ID:
		nandow_test->para_len = sizeof(struct rtw_nan_cluster_id);
		break;
	case RTW_NAN_CMD_MASTER_PREF:
		nandow_test->para_len = sizeof(struct rtw_nan_master_preference);
		break;
	case RTW_NAN_CMD_RANDOM_FACTOR:
		nandow_test->para_len = sizeof(struct rtw_nan_random_factor);
		break;
	case RTW_NAN_CMD_DW_AWAKE_PERIOD:
		nandow_test->para_len = sizeof(struct rtw_nan_discovery_window_awake_period);
		break;
	case RTW_NAN_CMD_PRI_MASTER_CH:
		nandow_test->para_len = sizeof(struct rtw_nan_primary_master_channel);
		break;
	case RTW_NAN_CMD_SEC_MASTER_CH:
		nandow_test->para_len = sizeof(struct rtw_nan_secondary_master_channel);
		break;
	case RTW_NAN_CMD_DISC_BCN_PERIOD:
		nandow_test->para_len = sizeof(struct rtw_nan_discovery_beacon_period);
		break;
	case RTW_NAN_CMD_FORCED_DISC_BCN:
		nandow_test->para_len = sizeof(struct rtw_nan_forced_discovery_beacon_transmission);
		break;
	case RTW_NAN_CMD_SUBSCRIBE:
		nandow_test->para_len = sizeof(struct rtw_nan_subscribe_data);
		break;
	case RTW_NAN_CMD_PUBLISH:
		nandow_test->para_len = sizeof(struct rtw_nan_publish_data);
		break;
	case RTW_NAN_CMD_CANCEL_SUBSCRIBE:
		nandow_test->para_len = sizeof(struct rtw_nan_cancel_subscribe_data);
		break;
	case RTW_NAN_CMD_CANCEL_PUBLISH:
		nandow_test->para_len = sizeof(struct rtw_nan_cancel_publish_data);
		break;
	case RTW_NAN_CMD_FOLLOWUP_TX:
		nandow_test->para_len = sizeof(struct rtw_nan_follow_up_transmit_data);
		break;
	case RTW_NAN_CMD_DATAPATH_REQ:
		nandow_test->para_len = sizeof(struct rtw_nan_datapath_request);
		break;
	case RTW_NAN_CMD_DATAPATH_RSP:
		nandow_test->para_len = sizeof(struct rtw_nan_datapath_response);
		break;
	case RTW_NAN_CMD_DATAPATH_CONFIRM:
		nandow_test->para_len = sizeof(struct rtw_nan_datapath_confirm);
		break;
	case RTW_NAN_CMD_DATAPATH_END:
		nandow_test->para_len = sizeof(struct rtw_nan_datapath_end);
		break;
	case RTW_NAN_CMD_NDC_AVAIL:
		nandow_test->para_len = sizeof(struct rtw_nan_data_cluster_availability);
		break;
	case RTW_NAN_CMD_SCAN_CONTROL:
		nandow_test->para_len = sizeof(struct rtw_nan_set_scan_control);
		break;
	case RTW_NAN_CMD_COUNTRY_CODE:
		nandow_test->para_len = sizeof(struct rtw_nan_country_code_data);
		break;
	default:
		break;
	}
	memset(&nandow_test->para, 0, sizeof(union nandow_para));
	printf("nandow_pre_actions : id(%d), op(%s), len(%d)\n",
		   nandow_test->cmd_id,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set",
		   nandow_test->para_len);
}

void nandow_post_actions(struct nan_customer_nandow *nandow_test)
{
	/* 	if (nandow_test->cmd_id == RTW_NAN_CMD_INIT) {
		} else if(nandow_test->cmd_id == RTW_NAN_CMD_WIFI_DRV_CAP) {
		} else if(nandow_test->cmd_id == RTW_NAN_CMD_NAN_ENABLE) {
		} else if(nandow_test->cmd_id == RTW_NAN_CMD_CLUSTER_ID) {
		} else if(nandow_test->cmd_id == RTW_NAN_CMD_MASTER_PREF) {
		} else if(nandow_test->cmd_id == RTW_NAN_CMD_RANDOM_FACTOR) {
		} else if(nandow_test->cmd_id == RTW_NAN_CMD_PUBLISH) {
		} else if(nandow_test->cmd_id == RTW_NAN_CMD_CANCEL_PUBLISH) {
		} */
}

void nandow_send_cmd(
	struct nan_customer_nandow *nandoow_cmd,
	void *input,
	unsigned int input_len,
	char *tmp_file,
	char *cmdreply_file,
	char *if_name,
	char *fname)
{
	FILE *outfile;
	FILE *file;
	char cmd[256];
	unsigned int no_input = 0xff, cmd_id = NAN_SUBCMD_CUSTOMER_NANDOW;

	if (input == NULL) {
		input = &no_input;
		input_len = sizeof(unsigned int);
	}

	// open file for writing
	outfile = fopen(tmp_file, "w");
	if (outfile == NULL) {
		fprintf(stderr, "\nError opend file\n");
		exit(1);
	}

	// write struct to file
	fwrite(input, input_len, 1, outfile);

	if (fwrite != 0) {
		printf("contents to file written successfully !\n");
	} else {
		printf("error writing file !\n");
	}

	// close file
	fclose(outfile);

	if ((file = fopen(fname, "r"))) {
		snprintf(cmd, sizeof(cmd), "%s %s vendor recv 0x%x 0x%x %s > %s", fname, if_name, OUI_REALTEK, cmd_id, tmp_file, cmdreply_file);
		printf("[rtw_cmd]%s\n", cmd);
		printf("Use iw in package\n");
		fclose(file);
		/* printf("cmd(%s)\n", cmd); */
	} else {
		snprintf(cmd, sizeof(cmd), "iw %s vendor send 0x%x 0x%x %s", if_name, OUI_REALTEK, cmd_id, tmp_file);
		printf("Use iw in system\n");
	}

	system(cmd);
	remove(tmp_file);
}

void nandow_dump_test_data(struct nan_customer_nandow *nandow_test)
{
	switch (nandow_test->cmd_id) {
	case RTW_NAN_CMD_INIT:
		nandow_dump_test_data_nan_init(nandow_test);
		break;
	case RTW_NAN_CMD_WIFI_DRV_CAP:
		nandow_dump_test_data_get_drv_cap(nandow_test);
	case RTW_NAN_CMD_NAN_ENABLE:
		nandow_dump_test_data_nan_enabled(nandow_test);
		break;
	case RTW_NAN_CMD_CLUSTER_ID:
		nandow_dump_data_cluster_id(nandow_test);
		break;
	case RTW_NAN_CMD_MASTER_PREF:
		nandow_dump_data_master_pref(nandow_test);
		break;
	case RTW_NAN_CMD_RANDOM_FACTOR:
		nandow_dump_data_random_factor(nandow_test);
		break;
	case RTW_NAN_CMD_DW_AWAKE_PERIOD:
		nandow_dump_data_dw_awake_period(nandow_test);
		break;
	case RTW_NAN_CMD_PRI_MASTER_CH:
		nandow_dump_data_primary_master_channel(nandow_test);
		break;
	case RTW_NAN_CMD_SEC_MASTER_CH:
		nandow_dump_data_secondary_master_channel(nandow_test);
		break;
	case RTW_NAN_CMD_DISC_BCN_PERIOD:
		nandow_dump_data_disc_period(nandow_test);
		break;
	case RTW_NAN_CMD_FORCED_DISC_BCN:
		nandow_dump_data_forced_bcn_trans(nandow_test);
		break;
	case RTW_NAN_CMD_SUBSCRIBE:
		nandow_dump_test_data_subscribe(nandow_test);
		break;
	case RTW_NAN_CMD_PUBLISH:
		nandow_dump_test_data_publish(nandow_test);
		break;
	case RTW_NAN_CMD_CANCEL_PUBLISH:
		nandow_dump_test_data_cancel_publish(nandow_test);
		break;
	case RTW_NAN_CMD_CANCEL_SUBSCRIBE:
		nandow_dump_test_data_cancel_subscribe(nandow_test);
		break;
	case RTW_NAN_CMD_FOLLOWUP_TX:
		nandow_dump_test_data_follow_up_transmit(nandow_test);
		break;
	case RTW_NAN_CMD_DATAPATH_REQ:
		nandow_dump_test_data_req(nandow_test);
		break;
	case RTW_NAN_CMD_DATAPATH_RSP:
		nandow_dump_test_data_rsp(nandow_test);
		break;
	case RTW_NAN_CMD_DATAPATH_CONFIRM:
		nandow_dump_test_data_confirm(nandow_test);
		break;
	case RTW_NAN_CMD_DATAPATH_END:
		nandow_dump_test_data_end(nandow_test);
		break;
	case RTW_NAN_CMD_NDC_AVAIL:
		nandow_dump_test_ndc_avail(nandow_test);
		break;
	case RTW_NAN_CMD_SCAN_CONTROL:
		nandow_dump_test_set_scan_control(nandow_test);
		break;
	case RTW_NAN_CMD_COUNTRY_CODE:
		nandow_dump_test_country_code(nandow_test);
		break;
	default:
		break;
	}
}

void nandow_parse_cmd_reply(struct nan_customer_nandow *nandow_test, char *cmdreply_file)
{

	char cmd[MAX_NANDOW_PARA_LEN] = {0}, input[MAX_NANDOW_PARA_LEN] = {0};
	FILE *rawfile;

	/* remove 'vendor response:' */
	sprintf(cmd, "sed -e \"s/vendor response://g\" -i %s", cmdreply_file);
	system(cmd);

	//sprintf(cmd, "cat %s", cmdreply_file);
	//system(cmd);

	/* read as hex and fill in nandow_test */
	rawfile = fopen(cmdreply_file, "r");
	if (rawfile == NULL) {
		fprintf(stderr, "\nError opend file\n");
		exit(1);
	}

	fread(input, MAX_NANDOW_PARA_LEN, 1, rawfile);
	strhex_2_realhex(input, (char *)nandow_test, MAX_NANDOW_REPLY_LEN, ' ');

	/*
		printf("nandow_test->para_len(%d): ", nandow_test->para_len);
		for(int i=0; i< nandow_test->para_len; i++)
			printf("%2x ", nandow_test->para[i]);
		printf("\n");
	*/

	fclose(rawfile);
	remove(cmdreply_file);
}

/* sub function definition */

// RTW_NAN_CMD_INIT
void nandow_load_test_data_nan_init(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_device_capability dev_cap;

	/* printf("nandow cmd RTW_NAN_CMD_INIT(%x)\n", RTW_NAN_CMD_INIT); */
	/* no set op, skip this step */
}

void nandow_dump_test_data_nan_init(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_device_capability *dev_cap = &nandow_test->para.dev_cap;

	printf("\nrtw_nan_dev_cap(%x), type(%s)\n", RTW_NAN_CMD_INIT,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\tsupported_bands(%x)\n", dev_cap->supported_bands);
	printf("\tsupported_cipher_suites(%x)\n", dev_cap->supported_cipher_suites);
	printf("\toperation_mode_ht_vht(%x)\n", dev_cap->operation_mode_ht_vht);
	printf("\tnumber_of_antennas(%x)\n", dev_cap->number_of_antennas);
	printf("\tmax_channel_switch_time(%x)\n", dev_cap->max_channel_switch_time);
	printf("\tmax_publishers(%x)\n", dev_cap->max_publishers);
	printf("\tmax_subscribers(%x)\n", dev_cap->max_subscribers);
	printf("\tmax_peer_entries(%x)\n", dev_cap->max_peer_entries);
	printf("\tmax_datapath_sessions(%x)\n", dev_cap->max_datapath_sessions);
	printf("\tmax_avail_maps(%x)\n", dev_cap->max_avail_maps);
	printf("\tmax_nan_interfaces(%x)\n", dev_cap->max_nan_interfaces);
	printf("\tunicast_insecure_datapath_restore_supported(%x)\n", dev_cap->unicast_insecure_datapath_restore_supported);
}

// RTW_NAN_CMD_WIFI_DRV_CAP
void nandow_load_test_data_get_drv_cap(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_driver_capabilities drv_cap;

	/* printf("nandow cmd RTW_NAN_CMD_WIFI_DRV_CAP(%x)\n", RTW_NAN_CMD_WIFI_DRV_CAP); */
	/* no set op, do nothing */
}

void nandow_dump_test_data_get_drv_cap(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_driver_capabilities *drv_cap = &nandow_test->para.drv_cap;

	printf("\nrtw_nan_drv_cap(%x), type(%s)\n", RTW_NAN_CMD_WIFI_DRV_CAP,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\tcapabilities(%x)\n", drv_cap->capabilities);
}

// RTW_NAN_CMD_NAN_ENABLE
void nandow_load_test_data_nan_enabled(struct nan_customer_nandow *nandow_test, char **argv, int argc)
{
	struct rtw_nan_enable *nan_enabled = &nandow_test->para.nan_enabled;

	if (argv == NULL || argc == 0) {
		printf("Error to load nan_enabled\n");
	}

	if (argc > 0 && strcmp(argv[0], "enabled") == 0) {
		argv ++;
		argc --;

		nan_enabled->enable = atoi(argv[0]);
	}

	printf("\tenable(%x)\n", nan_enabled->enable);
}

void nandow_dump_test_data_nan_enabled(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_enable *nan_enabled = &nandow_test->para.nan_enabled;

	printf("\nrtw_nan_enable(%x), type(%s)\n", RTW_NAN_CMD_NAN_ENABLE,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\tenable(%x)\n", nan_enabled->enable);
}
// RTW_NAN_CMD_CLUSTER_ID
void nandow_load_data_cluster_id(struct nan_customer_nandow *nandow_test, char **argv, int argc)
{
	struct rtw_nan_cluster_id *cluster_id = &nandow_test->para.cluster_id;

	if (argv == NULL || argc == 0) {
		printf("Error to load master_pref\n");
	}

	if (argc > 0 && strcmp(argv[0], "cluster_id") == 0) {
		argv ++;
		argc --;

		mac_addr_a2n((unsigned char *) & (cluster_id->cluster_id), argv[0]);
	}

	for (int i = 0 ; i < ETH_ALEN ; i++) {
		printf("\tload cluster_id[%d]= %d\n", i, nandow_test->para.cluster_id.cluster_id.ether_addr_octet[i]);
	}
}

void nandow_dump_data_cluster_id(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_cluster_id *drv_cluster_id = &nandow_test->para.cluster_id;

	printf("\nrtw_nan_cluster_id(%x), type(%s)\n", RTW_NAN_CMD_CLUSTER_ID,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	for (int i = 0 ; i < ETH_ALEN ; i++) {
		printf("\t cluster_id[%d]= %x\n", i, drv_cluster_id->cluster_id.ether_addr_octet[i]);
	}
}

void nandow_load_data_disc_period(struct nan_customer_nandow *nandow_test,
								  char **argv,
								  int argc)
{
	struct rtw_nan_discovery_beacon_period *disc_bcn = &nandow_test->para.disc_bcn_period;

	if (argv == NULL || argc == 0) {
		printf("Error to load disc_bcn_period\n");
	}

	memset(disc_bcn, 0, sizeof(struct rtw_nan_discovery_beacon_period));
	if (argc > 0 && strcmp(argv[0], "period") == 0) {
		argv ++;
		argc --;
		disc_bcn->period = atoi(argv[0]);
	}

	printf("\tload disc bcn period(%d) length(%d)\n", disc_bcn->period, nandow_test->para_len);
}
void nandow_dump_data_disc_period(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_discovery_beacon_period *disc_bcn = &nandow_test->para.disc_bcn_period;

	printf("\nrtw_nan_disc_bcn_period((%x), type(%s)\n",
		   RTW_NAN_CMD_DISC_BCN_PERIOD,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\tperiod(%d)\n", disc_bcn->period);
}

void nandow_load_forced_bcn_trans(struct nan_customer_nandow *nandow_test,
								  char **argv,
								  int argc)
{
	struct rtw_nan_forced_discovery_beacon_transmission *forced_bcn =
			&nandow_test->para.forced_disc_bcn;
	u8 test_pattern = 0;

	if (argv == NULL || argc == 0) {
		printf("Error to load forced_disc_bcn\n");
	}


	memset(forced_bcn, 0, sizeof(struct rtw_nan_forced_discovery_beacon_transmission));
	if (argc > 0 && strcmp(argv[0], "interval") == 0) {
		argv ++;
		argc --;
		forced_bcn->enable = 1;
		forced_bcn->beacon_interval = atoi(argv[0]);
		forced_bcn->reason = 0;
		printf("\tload forced disc bcn: intvl(%d) length(%d)\n",
			   forced_bcn->beacon_interval,
			   nandow_test->para_len);
	} else if (argc > 0 && strcmp(argv[0], "avail") == 0) {
		argv ++;
		argc --;
		test_pattern = atoi(argv[0]);

		forced_bcn->enable = 1;
		forced_bcn->availability.num_map_ids = 1;

		/* bit duration: 0 = 16 TUs, 1 = 32 TUs, 2 = 64 TUs, 3 = 128 TUs */
		if (test_pattern == 1) {
			forced_bcn->availability.slots[0].map_id = 0;
			forced_bcn->availability.slots[0].time_bitmap.offset = 0;
			forced_bcn->availability.slots[0].time_bitmap.period = 3;
			forced_bcn->availability.slots[0].time_bitmap.bit_duration = 0;
			forced_bcn->availability.slots[0].time_bitmap.time_bitmap_length = 4;
			forced_bcn->availability.slots[0].time_bitmap.time_bitmap[0] = 0x14;
			forced_bcn->availability.slots[0].time_bitmap.time_bitmap[1] = 0x40;
			forced_bcn->availability.slots[0].time_bitmap.time_bitmap[2] = 0x10;
			forced_bcn->availability.slots[0].time_bitmap.time_bitmap[3] = 0x04;
		} else if (test_pattern == 2) {
			forced_bcn->availability.slots[0].map_id = 0;
			forced_bcn->availability.slots[0].time_bitmap.offset = 16;
			forced_bcn->availability.slots[0].time_bitmap.period = 2;
			forced_bcn->availability.slots[0].time_bitmap.bit_duration = 0;
			forced_bcn->availability.slots[0].time_bitmap.time_bitmap_length = 1;
			forced_bcn->availability.slots[0].time_bitmap.time_bitmap[0] = 0x14;
		} else {
			forced_bcn->availability.slots[0].map_id = 0;
			forced_bcn->availability.slots[0].time_bitmap.offset = 0;
			forced_bcn->availability.slots[0].time_bitmap.period = 2;
			forced_bcn->availability.slots[0].time_bitmap.bit_duration = 2;
			forced_bcn->availability.slots[0].time_bitmap.time_bitmap_length = 1;
			forced_bcn->availability.slots[0].time_bitmap.time_bitmap[0] = 0xA;
		}

		forced_bcn->reason = 0;

		printf("\tload forced disc bcn: avail, length(%d)\n",
			   nandow_test->para_len);
	} else if (argc > 0 && strcmp(argv[0], "disable") == 0) {
		// argv ++;
		argc --;
		forced_bcn->enable = 0;
		forced_bcn->reason = 0;

		printf("\tload forced disc bcn: enable(%d) length(%d)\n",
			   forced_bcn->enable,
			   nandow_test->para_len);
	}
}
void nandow_dump_data_forced_bcn_trans(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_forced_discovery_beacon_transmission *forced_bcn =
			&nandow_test->para.forced_disc_bcn;
	u8 i = 0;
	u8 j = 0;

	printf("\nrtw_nan_forced_disc_bcn((%x), type(%s)\n",
		   RTW_NAN_CMD_FORCED_DISC_BCN,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");

	if (!forced_bcn->enable) {
		/* disable */
		printf("\tenable(%d) reason(%d)\n", forced_bcn->enable, forced_bcn->reason);
	} else if (forced_bcn->beacon_interval) {
		/* interval */
		printf("\tenable(%d) intvl(%d) reason(%d)\n",
			   forced_bcn->enable,
			   forced_bcn->beacon_interval,
			   forced_bcn->reason);
	} else {
		printf("\tenable(%d) reason(%d)\n", forced_bcn->enable, forced_bcn->reason);

		for (i = 0; i < forced_bcn->availability.num_map_ids; i++) {
			printf("\tmap_id(%d)\n", forced_bcn->availability.slots[i].map_id);
			printf("\time_bitmap: offset(%d)period(%d)bitDuration(%d)length(%d)\n",
				   forced_bcn->availability.slots[i].time_bitmap.offset,
				   forced_bcn->availability.slots[i].time_bitmap.period,
				   forced_bcn->availability.slots[i].time_bitmap.bit_duration,
				   forced_bcn->availability.slots[i].time_bitmap.time_bitmap_length);
			for (j = 0; j < forced_bcn->availability.slots[i].time_bitmap.time_bitmap_length; j++) {
				printf("\ttime_bitmap: 0x%x\n",
					   forced_bcn->availability.slots[i].time_bitmap.time_bitmap[j]);
			}
		}
	}
}

// RTW_NAN_CMD_MASTER_PREF
void nandow_load_data_master_pref(struct nan_customer_nandow *nandow_test, char **argv, int argc)
{
	struct rtw_nan_master_preference *master_pref
			= &nandow_test->para.master_pref;

	if (argv == NULL || argc == 0) {
		printf("Error to load master_pref\n");
	}

	if (argc > 0 && strcmp(argv[0], "master_pref") == 0) {
		argv ++;
		argc --;

		master_pref->master_preference = atoi(argv[0]);
	}

	printf("\tload master_pref(%d) length(%d)\n",
		   nandow_test->para.master_pref.master_preference,
		   nandow_test->para_len);
}

void nandow_dump_data_master_pref(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_master_preference *master_pref = &nandow_test->para.master_pref;

	printf("\nrtw_nan_master_pref((%x), type(%s)\n",
		   RTW_NAN_CMD_MASTER_PREF,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\tmaster_pref(%d)\n", master_pref->master_preference);
}

// RTW_NAN_CMD_RANDOM_FACTOR
void nandow_load_data_random_factor(struct nan_customer_nandow *nandow_test, char **argv, int argc)
{
	struct rtw_nan_random_factor *random_factor
			= &nandow_test->para.random_factor;

	if (argv == NULL || argc == 0) {
		printf("Error to load master_pref\n");
	}

	if (argc > 0 && strcmp(argv[0], "random_factor") == 0) {
		argv ++;
		argc --;

		random_factor->random_factor = atoi(argv[0]);
	}

	printf("\tload random_factor(%d) length(%d)\n",
		   nandow_test->para.random_factor.random_factor, nandow_test->para_len);
}

void nandow_dump_data_random_factor(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_random_factor *drv_random_factor =
			&nandow_test->para.random_factor;

	printf("\nrtw_nan_random_factor(%x), type(%s)\n", RTW_NAN_CMD_RANDOM_FACTOR,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\trandom_factor(%d)\n", drv_random_factor->random_factor);
}

void nandow_load_data_dw_awake_period(struct nan_customer_nandow *nandow_test,
									  char **argv,
									  int argc)
{
	struct rtw_nan_discovery_window_awake_period *dw_awake = &nandow_test->para.dw_awake_period;

	if (argv == NULL || argc < 2) {
		printf("Error to load dw_awake_period\n");
	}

	/* Avail value for period: 0 (not avail for 24G DW), 1, 2, 4 */
	memset(dw_awake, 0, sizeof(struct rtw_nan_discovery_window_awake_period));
	if (argc > 1) {
		dw_awake->period_2_4g = atoi(argv[0]);
		dw_awake->period_5g = atoi(argv[1]);
		// argv += 2;
		argc -= 2;
	}

	printf("\tload dw_awake 24G(%d)5G(%d) length(%d)\n",
		   dw_awake->period_2_4g,
		   dw_awake->period_5g,
		   nandow_test->para_len);
	printf("\tNote for avail val: 0 (not avail for 2g), 1, 2, 4\n");
}

void nandow_dump_data_dw_awake_period(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_discovery_window_awake_period *dw_awake = &nandow_test->para.dw_awake_period;

	printf("\nrtw_nan_dw_awake_period((%x), type(%s)\n",
		   RTW_NAN_CMD_DW_AWAKE_PERIOD,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\t24g(%d)5g(%d)\n", dw_awake->period_2_4g, dw_awake->period_5g);
}

void nandow_load_data_primary_master_channel(struct nan_customer_nandow *nandow_test,
		char **argv,
		int argc)
{
	struct rtw_nan_primary_master_channel *pri_ch = &nandow_test->para.pri_master_ch;

	if (argv == NULL || argc < 1) {
		printf("Error to load primary_master_channel\n");
	}

	memset(pri_ch, 0, sizeof(struct rtw_nan_primary_master_channel));

	pri_ch->channel = atoi(argv[0]);

	printf("\tload primary_master_channel=(%d) length(%d)\n",
		   pri_ch->channel,
		   nandow_test->para_len);
}

void nandow_dump_data_primary_master_channel(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_primary_master_channel *pri_ch = &nandow_test->para.pri_master_ch;

	printf("\nrtw_nan_pri_master_ch((%x), type(%s)\n",
		   RTW_NAN_CMD_PRI_MASTER_CH,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\t primary channel (%d)\n", pri_ch->channel);
}

void nandow_load_data_secondary_master_channel(struct nan_customer_nandow *nandow_test,
		char **argv,
		int argc)
{
	struct rtw_nan_secondary_master_channel *second_ch = &nandow_test->para.second_master_ch;

	if (argv == NULL || argc < 1) {
		printf("Error to load secondmary_master_channel\n");
	}

	memset(second_ch, 0, sizeof(struct rtw_nan_secondary_master_channel));

	second_ch->channel = atoi(argv[0]);

	printf("\tload secondary_master_channel=(%d) length(%d)\n",
		   second_ch->channel,
		   nandow_test->para_len);
}

void nandow_dump_data_secondary_master_channel(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_secondary_master_channel *second_ch = &nandow_test->para.second_master_ch;

	printf("\nrtw_nan_pri_master_ch((%x), type(%s)\n",
		   RTW_NAN_CMD_SEC_MASTER_CH,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\t secondary channel (%d)\n", second_ch->channel);
}



// RTW_NAN_CMD_PUBLISH : rtw_nan_subscribe_data_t
void nandow_load_test_data_subscribe(
	struct nan_customer_nandow *nandow_test,
	char **argv,
	int argc)
{
	struct rtw_nan_subscribe_data *subscribe_srv
			= &nandow_test->para.subscribe_srv;
	char tmp_arry[512] = {0};
	u8 gen_srv_info[] = {0x50, 0x6f, 0x9a, 0x02, //WFA_OUI,Type
						 0x01, 0x01, 0x00, 0x06, //Transport Protocol(TCP 0X06)
						 0x02, 0x07, 0x00, 0x72, 0x65, 0x61, 0x6c, 0x74, 0x65, 0x6b
						};
	//0x02: Service Name
	u16 gen_srv_info_len = sizeof(gen_srv_info) / sizeof(gen_srv_info[0]);
	u8 srv_hash[RTW_NAN_SERVICE_NAME_HASH_SIZE] = {0xb4, 0x26, 0x6b, 0xab, 0x05, 0x07};
	//printf("nandow cmd WFPAL_NAN_SUBSRCIBE(%x)\n", WFPAL_NAN_SUBSRCIBE);

	if (argv == NULL || argc == 0) {
		printf("Error to load subscribe\n");
	}

	if (argc > 0 && strcmp(argv[0], "actsub") == 0) {
		argv ++;
		argc --;
		subscribe_srv->subscribe_type =
			RTW_NAN_SUBSCRIBE_TYPE_ACTIVE;

		subscribe_srv->matching_filter_tx_count = 1;
		sprintf(tmp_arry, "%s", "bytes used for matching");
		memcpy(subscribe_srv->matching_filter_tx[0].filter, tmp_arry, sizeof("bytes used for matching"));
		subscribe_srv->matching_filter_tx[0].length = sizeof("bytes used for matching") - 1;
	} else if (argc > 0 && strcmp(argv[0], "passub") == 0) {
		// argv ++;
		argc --;
		subscribe_srv->subscribe_type =
			RTW_NAN_SUBSCRIBE_TYPE_PASSIVE;

		subscribe_srv->matching_filter_rx_count = 1;
		sprintf(tmp_arry, "%s", "bytes used for matching");
		memcpy(subscribe_srv->matching_filter_rx[0].filter, tmp_arry, sizeof("bytes used for matching"));
		subscribe_srv->matching_filter_rx[0].length = sizeof("bytes used for matching") - 1;
	}

	/* start initialize parameters here */
	memcpy(subscribe_srv->service_hash, srv_hash, RTW_NAN_SERVICE_NAME_HASH_SIZE);

	sprintf(tmp_arry, "%s", "realtek");
	memcpy(subscribe_srv->name.name, tmp_arry, sizeof("realtek"));

	subscribe_srv->subscribe_id = 1;
	subscribe_srv->awake_discovery_window_interval = 1;
	subscribe_srv->ttl = 1024;
	subscribe_srv->flags = RTW_NAN_PUBLISH_FLAG_ADD_DEVICE_CAPABILITY_ATTRIBUTE;
	subscribe_srv->control =
		(RTW_NAN_SDE_CTL_DATAPATH_REQ_REQUIRED ||
		 RTW_NAN_SDE_CTL_DATAPATH_TYPE ||
		 /* RTW_NAN_SDE_CTL_SECURITY_REQUIRED || */
		 RTW_NAN_SDE_CTL_SERVICE_INFO_PRESENT);

	memcpy(subscribe_srv->service_specific_info.info, gen_srv_info,
		   gen_srv_info_len);
	subscribe_srv->service_specific_info.length = gen_srv_info_len;
	/* for google CTS
	sprintf(tmp_arry, "%s", "Extra bytes in the publisher discovery");
	memcpy(subscribe_srv->service_specific_info.info, tmp_arry,
		sizeof("Extra bytes in the publisher discovery")-1);
	subscribe_srv->service_specific_info.length =
		sizeof("Extra bytes in the publisher discovery")-1;
	*/

}

void nandow_dump_test_data_subscribe(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_subscribe_data *subscribe_srv
			= &nandow_test->para.subscribe_srv;

	printf("\nrtw_nan_subscribe_data(%x), type(%s)\n", RTW_NAN_CMD_SUBSCRIBE,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\t subscribe_type(%x)\n", subscribe_srv->subscribe_type);
	printf("\t subscribe_id(%x)\n", subscribe_srv->subscribe_id);
	printf("\t name(%s)\n", subscribe_srv->name.name);
	_dump_service_info(&subscribe_srv->service_specific_info);
}

// RTW_NAN_CMD_PUBLISH : rtw_nan_publish_data_t
void nandow_load_test_data_publish(struct nan_customer_nandow *nandow_test,
								   char **argv,
								   int argc)
{
	struct rtw_nan_publish_data *publish_srv
			= &nandow_test->para.publish_srv;
	char tmp_arry[512] = {0};
	u8 gen_srv_info[] = {0x50, 0x6f, 0x9a, 0x02, //WFA_OUI,Type
						 0x01, 0x01, 0x00, 0x06, //0x01: Transport Protocol(TCP 0X06)
						 0x02, 0x07, 0x00, 0x72, 0x65, 0x61, 0x6c, 0x74, 0x65, 0x6b
						};
	//0x02: Service Name
	u16 gen_srv_info_len = sizeof(gen_srv_info) / sizeof(gen_srv_info[0]);
	u8 srv_hash[RTW_NAN_SERVICE_NAME_HASH_SIZE] = {0xb4, 0x26, 0x6b, 0xab, 0x05, 0x07};

	//printf("nandow cmd RTW_NAN_CMD_PUBLISH(%x)\n", RTW_NAN_CMD_PUBLISH);

	if (argv == NULL || argc == 0) {
		printf("Error to load publish\n");
	}

	if (argc > 0 && strcmp(argv[0], "unsopub") == 0) {
		argv ++;
		argc --;
		publish_srv->publish_type =
			RTW_NAN_PUBLISH_TYPE_UNSOLICITED;

		publish_srv->matching_filter_tx_count = 1;
		sprintf(tmp_arry, "%s", "bytes used for matching");
		memcpy(publish_srv->matching_filter_tx[0].filter, tmp_arry, sizeof("bytes used for matching") - 1);
		publish_srv->matching_filter_tx[0].length = sizeof("bytes used for matching") - 1;
	} else if (argc > 0 && strcmp(argv[0], "sopub") == 0) {
		// argv ++;
		argc --;
		publish_srv->publish_type =
			RTW_NAN_PUBLISH_TYPE_SOLICITED;

		publish_srv->matching_filter_rx_count = 1;
		sprintf(tmp_arry, "%s", "bytes used for matching");
		memcpy(publish_srv->matching_filter_rx[0].filter, tmp_arry, sizeof("bytes used for matching"));
		publish_srv->matching_filter_rx[0].length = sizeof("bytes used for matching") - 1;
	}

	/* start initialize parameters here */
	memcpy(publish_srv->service_hash, srv_hash, RTW_NAN_SERVICE_NAME_HASH_SIZE);

	sprintf(tmp_arry, "%s", "realtek");
	memcpy(publish_srv->name.name, tmp_arry, sizeof("realtek"));
	publish_srv->publish_id = 1;
	publish_srv->awake_discovery_window_interval = 1;
	publish_srv->ttl = 1024;
	publish_srv->flags = RTW_NAN_PUBLISH_FLAG_ADD_DEVICE_CAPABILITY_ATTRIBUTE;
	publish_srv->control =
		(RTW_NAN_SDE_CTL_DATAPATH_REQ_REQUIRED ||
		 RTW_NAN_SDE_CTL_DATAPATH_TYPE ||
		 /* RTW_NAN_SDE_CTL_SECURITY_REQUIRED || */
		 RTW_NAN_SDE_CTL_SERVICE_INFO_PRESENT);


	memcpy(publish_srv->service_specific_info.info, gen_srv_info,
		   gen_srv_info_len);
	publish_srv->service_specific_info.length = gen_srv_info_len;

	/* for google CTS
	sprintf(tmp_arry, "%s", "Extra bytes in the publisher discovery");
	memcpy(publish_srv->service_specific_info.info, tmp_arry,
		sizeof("Extra bytes in the publisher discovery")-1);
	publish_srv->service_specific_info.length =
		sizeof("Extra bytes in the publisher discovery")-1;
	*/

	//publish_srv->cipher_suite_id = RTW_NAN_CIPHER_ID_NCS_SK_128;

	//sprintf(tmp_arry, "%s", "123456789abcdef0123456789abcdef0");
	//memcpy(publish_srv->key, tmp_arry, RTW_NAN_PMK_SIZE);
}

void nandow_dump_test_data_publish(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_publish_data *publish_srv
			= &nandow_test->para.publish_srv;

	printf("\nrtw_nan_publish_data(%x), type(%s)\n", RTW_NAN_CMD_PUBLISH,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\t publish_id(%x)\n", publish_srv->publish_id);
	printf("\t name(%s)\n", publish_srv->name.name);
	_dump_service_info(&publish_srv->service_specific_info);

}

// RTW_NAN_CMD_CANCEL_PUBLISH : rtw_nan_cancel_publish_data_t
void nandow_load_test_data_cancel_publish(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_cancel_publish_data *cancel_publish_srv
			= &nandow_test->para.cancel_publish_srv;

	/* start initialize parameters here */
	cancel_publish_srv->publish_id = 1;
}

void nandow_dump_test_data_cancel_publish(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_cancel_publish_data *cancel_publish_srv
			= &nandow_test->para.cancel_publish_srv;

	printf("\nrtw_nan_cancel_publish_data(%x), type(%s)\n", RTW_NAN_CMD_CANCEL_PUBLISH,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\t publish_id(%x)\n", cancel_publish_srv->publish_id);
}

// RTW_NAN_CMD_CANCEL_PUBLISH : rtw_nan_cancel_subscribe_data_t
void nandow_load_test_data_cancel_subscribe(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_cancel_subscribe_data *cancel_subscribe_srv
			= &nandow_test->para.cancel_sub_srv;

	/* start initialize parameters here */
	cancel_subscribe_srv->subscribe_id = 1;
}

void nandow_dump_test_data_cancel_subscribe(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_cancel_subscribe_data *cancel_subscribe_srv
			= &nandow_test->para.cancel_sub_srv;

	printf("\nrtw_nan_cancel_subscribe_data(%x), type(%s)\n", RTW_NAN_CMD_CANCEL_SUBSCRIBE,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");
	printf("\t subscribe_id(%x)\n", cancel_subscribe_srv->subscribe_id);
}

// RTW_NAN_CMD_FOLLOWUP_TXT_DATA : rtw_nan_followup_tx_data_t
void nandow_load_test_data_follow_up_transmit(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_follow_up_transmit_data *followup_data
			= &nandow_test->para.followup_srv;
#if(1)
	u8 srvc_info[7] = {0x12, 0x34, 0x56, 0x78, 0xab, 0xcd, 0xef};
#endif
#if(0)
	u8 nan_addr[ETH_ALEN] = {0xde, 0xe9, 0x94, 0x4d, 0x3f, 0xc4};
	u8 dst_addr[ETH_ALEN] = {0xde, 0xe9, 0x94, 0x4d, 0x3f, 0x5e};
#endif

	/* start initialize parameters here */
	followup_data->local_service_id = 1;
	followup_data->requestor_instance_id = 1;                        // Requestor Instance ID
	followup_data->follow_up_token = 0;                              // Return value
	memcpy(followup_data->destination_address.ether_addr_octet, dst_addr, ETH_ALEN);
	followup_data->service_specific_info.length = ETH_ALEN;
	memcpy(followup_data->service_specific_info.info, srvc_info, 7);

}

void nandow_dump_test_data_follow_up_transmit(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_follow_up_transmit_data *followup_data
			= &nandow_test->para.followup_srv;
	int i = 0;

	printf("\nrtw_nan_followup_tx_data(%x)\n", RTW_NAN_CMD_FOLLOWUP_TX);
	printf("\t local_service_id(%x)\n", followup_data->local_service_id);
	printf("\t requestor_instance_id(%x)\n", followup_data->requestor_instance_id);
	for (int i = 0 ; i < ETH_ALEN ; i++) {
		printf("\t destination_address[%d]= %x\n", i, followup_data->destination_address.ether_addr_octet[i]);
	}

	_dump_service_info(&followup_data->service_specific_info);
}

void nandow_load_test_data_req(struct nan_customer_nandow *nandow_test,
							   char **argv,
							   int argc)
{
	struct rtw_nan_datapath_request *datapath_req_para
			= &nandow_test->para.datapath_req;
	u8 gen_srv_info[] = {0x50, 0x6f, 0x9a, 0x02, //WFA_OUI,Type
						 0x01, 0x01, 0x00, 0x06, //0x01: Transport Protocol(TCP 0X06)
						 0x02, 0x07, 0x00, 0x72, 0x65, 0x61, 0x6c, 0x74, 0x65, 0x6b
						};
	//0x02: Service Name
	u16 gen_srv_info_len = sizeof(gen_srv_info) / sizeof(gen_srv_info[0]);

	datapath_req_para->datapath_id = 1;
	datapath_req_para->responder_publish_id = 1;
	memcpy(datapath_req_para->responder_nan_addr.ether_addr_octet, dst_addr, ETH_ALEN);
	memcpy(datapath_req_para->service_specific_info.info, gen_srv_info,
		   gen_srv_info_len);
	datapath_req_para->service_specific_info.length = gen_srv_info_len;

}

void nandow_dump_test_data_req(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_datapath_request *datapath_req_para
			= &nandow_test->para.datapath_req;
	printf("\nrtw_nan_datapath_req(%x)\n", RTW_NAN_CMD_DATAPATH_REQ);
	printf("\t datapath_id(%x)\n", datapath_req_para->datapath_id);
	printf("\t responder_publish_id(%x)\n", datapath_req_para->responder_publish_id);
	for (int i = 0 ; i < ETH_ALEN ; i++)
		printf("\t destination_address[%d]= %x\n", i,
			   datapath_req_para->responder_nan_addr.ether_addr_octet[i]);
	_dump_service_info(&datapath_req_para->service_specific_info);
}

void nandow_load_test_data_rsp(struct nan_customer_nandow *nandow_test,
							   char **argv,
							   int argc)
{
	struct rtw_nan_datapath_response *datapath_rsp_para
			= &nandow_test->para.datapath_rsp;
	u8 gen_srv_info[] = {0x50, 0x6f, 0x9a, 0x02, //WFA_OUI,Type
						 0x01, 0x01, 0x00, 0x06, //0x01: Transport Protocol(TCP 0X06)
						 0x02, 0x07, 0x00, 0x72, 0x65, 0x61, 0x6c, 0x74, 0x65, 0x6b
						};
	//0x02: Service Name
	u16 gen_srv_info_len = sizeof(gen_srv_info) / sizeof(gen_srv_info[0]);

	datapath_rsp_para->datapath_id = 1;
	datapath_rsp_para->publish_id = 1;
	datapath_rsp_para->status = RTW_NAN_DATAPATH_ACCEPTED;
	memcpy(datapath_rsp_para->initiator_management_address.ether_addr_octet, dst_addr, ETH_ALEN);
	memcpy(datapath_rsp_para->service_specific_info.info, gen_srv_info,
		   gen_srv_info_len);
	datapath_rsp_para->service_specific_info.length = gen_srv_info_len;

}

void nandow_dump_test_data_rsp(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_datapath_response *datapath_rsp_para
			= &nandow_test->para.datapath_rsp;
	printf("\nrtw_nan_datapath_rsp(%x)\n", RTW_NAN_CMD_DATAPATH_RSP);
	printf("\t datapath_id(%x)\n", datapath_rsp_para->datapath_id);
	printf("\t publishId(%x)\n", datapath_rsp_para->publish_id);
	printf("\t status(%x)\n", datapath_rsp_para->status);
	for (int i = 0 ; i < ETH_ALEN ; i++)
		printf("\t initiator_management_address[%d]= %x\n", i,
			   datapath_rsp_para->initiator_management_address.ether_addr_octet[i]);
	_dump_service_info(&datapath_rsp_para->service_specific_info);
}


void nandow_load_test_data_confirm(struct nan_customer_nandow *nandow_test,
								   char **argv,
								   int argc)
{
	struct rtw_nan_datapath_confirm *datapath_confirm_para
			= &nandow_test->para.datapath_confirm;
	datapath_confirm_para->datapath_id = 1;
	datapath_confirm_para->status = 0;
	memcpy(datapath_confirm_para->initiator_data_address.ether_addr_octet, dst_addr, ETH_ALEN);

}

void nandow_dump_test_data_confirm(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_datapath_confirm *datapath_confirm_para
			= &nandow_test->para.datapath_confirm;
	printf("\nrtw_nan_datapath_confirm(%x)\n", RTW_NAN_CMD_DATAPATH_CONFIRM);
	printf("\t datapath_id(%x)\n", datapath_confirm_para->datapath_id);
	printf("\t status(%x)\n", datapath_confirm_para->status);
	for (int i = 0 ; i < ETH_ALEN ; i++)
		printf("\t initiator_data_address[%d]= %x\n", i,
			   datapath_confirm_para->initiator_data_address.ether_addr_octet[i]);
}

// RTW_NAN_EVT_DATAPATH_END
void nandow_load_test_data_end(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_datapath_end *ndp_end
			= &nandow_test->para.datapath_end;

	/* start initialize parameters here */
	ndp_end->datapath_id = 1;
	ndp_end->reason_code = RTW_NAN_DATAPATH_END_REASON_USER_INITIATED;
}

void nandow_dump_test_data_end(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_datapath_end *ndp_end
			= &nandow_test->para.datapath_end;

	printf("\trtw_nan_datapath_end(%x), type(%s)\n", RTW_NAN_EVT_DATAPATH_END,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");

	printf("\t datapath_id(%d)\n", ndp_end->datapath_id);
	printf("\t dialog_token(%d)\n", ndp_end->dialog_token);
	printf("\t reason_code(%d)\n", ndp_end->reason_code);
	printf("\t initiator_data_address[%x-%x-%x-%x-%x-%x]\n",
		   ndp_end->initiator_data_address.ether_addr_octet[0],
		   ndp_end->initiator_data_address.ether_addr_octet[1],
		   ndp_end->initiator_data_address.ether_addr_octet[2],
		   ndp_end->initiator_data_address.ether_addr_octet[3],
		   ndp_end->initiator_data_address.ether_addr_octet[4],
		   ndp_end->initiator_data_address.ether_addr_octet[5]);
	printf("\t peer_address[%x-%x-%x-%x-%x-%x]\n",
		   ndp_end->peer_address.ether_addr_octet[0],
		   ndp_end->peer_address.ether_addr_octet[1],
		   ndp_end->peer_address.ether_addr_octet[2],
		   ndp_end->peer_address.ether_addr_octet[3],
		   ndp_end->peer_address.ether_addr_octet[4],
		   ndp_end->peer_address.ether_addr_octet[5]);
}

void nandow_load_test_ndc_avail(struct nan_customer_nandow *nandow_test,
								char **argv,
								int argc)
{
	struct rtw_nan_data_cluster_availability *ndc_avail_para = &nandow_test->para.avail_ndc;
	u8 ndc[4] = {0x00, 0x03, 0x00, 0x00};

	ndc_avail_para->num_maps_ids = 1;
	//ndc_avail_para->availability_params[0].data_cluster_id = {}; /* parsing from iw event? */
	ndc_avail_para->availability_params[0].map_id = 0;
	ndc_avail_para->availability_params[0].selected = 1;
	ndc_avail_para->availability_params[0].time_bitmap.offset = 0;
	ndc_avail_para->availability_params[0].time_bitmap.period = 3;
	ndc_avail_para->availability_params[0].time_bitmap.bit_duration = 0;
	ndc_avail_para->availability_params[0].time_bitmap.time_bitmap_length = 4;
	memcpy(ndc_avail_para->availability_params[0].time_bitmap.time_bitmap, ndc, 4);

}

void nandow_dump_test_ndc_avail(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_data_cluster_availability *ndc_avail_para = &nandow_test->para.avail_ndc;

	printf("\nrtw_nan_ndc_avail(%x)\n", RTW_NAN_CMD_NDC_AVAIL);
}

// RTW_NAN_CMD_SCAN_CONTROL
void nandow_load_test_set_scan_control(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_set_scan_control *scan_ctl
			= &nandow_test->para.scan_ctl;

	/* start initialize parameters here */
	scan_ctl->client = 2;
	scan_ctl->params.roam_scan_disable = 1;
}

void nandow_dump_test_set_scan_control(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_set_scan_control *scan_ctl
			= &nandow_test->para.scan_ctl;

	printf("\trtw_nan_set_scan_control(%x), type(%s)\n", RTW_NAN_CMD_SCAN_CONTROL,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");

	printf("\t client(%d)\n", scan_ctl->client);
	printf("\t is_clear_request(%d)\n", scan_ctl->is_clear_request);
	printf("\t abort_scan(%d)\n", scan_ctl->params.abort_scan);
	printf("\t epno_scan_disable(%d)\n", scan_ctl->params.epno_scan_disable);
	printf("\t roam_scan_disable(%d)\n", scan_ctl->params.roam_scan_disable);
	printf("\t limited_roam_enable(%d)\n", scan_ctl->params.limited_roam_enable);

}

// RTW_NAN_CMD_COUNTRY_CODE
void nandow_load_test_country_code(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_country_code_data *country_code
			= &nandow_test->para.country_code;

	/* start initialize parameters here */
	/* TODO */
}

void nandow_dump_test_country_code(struct nan_customer_nandow *nandow_test)
{
	struct rtw_nan_country_code_data *country_code
			= &nandow_test->para.country_code;

	printf("\trtw_nan_country_code(%x), type(%s)\n", RTW_NAN_CMD_COUNTRY_CODE,
		   (nandow_test->cmd_type == NANDOW_CMD_TYPE_GET) ? "get" : "set");

}


#endif /* NAN_CUSTOMER_NANDOW */
