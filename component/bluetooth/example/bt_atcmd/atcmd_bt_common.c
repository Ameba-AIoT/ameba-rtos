/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <osif.h>
#include <atcmd_service.h>
#include <atcmd_bt_impl.h>
#include <bt_utils.h>

static bool check_str_whitespace(char *str)
{
	uint32_t str_len = strlen(str);

	if (' ' == str[0] || ' ' == str[str_len - 1]) {
		BT_LOGE("[%s]Error: Atcmd param has 'whitespace' in string!!!\r\n", __func__);
		return TRUE;
	}
	return FALSE;
}

/* validate arg num and find cmd in cmd_table, then excute it */
int atcmd_bt_excute(int argc, char *argv[], const cmd_table_t *cmd_table, const char *tag)
{
	char *cmd_str = argv[0];
	int i = 0, j = 0;
	int ret = 0;

	if (!argc) {
		BT_LOGE("[%s]Error: Atcmd has no param!!!\r\n", __func__);
		ret = BT_AT_ERR_CMD_INVALID;
		goto exit;
	}

	for (j = 0; j < argc; j++) {
		if (NULL == argv[j]) {
			BT_LOGE("[%s]Error: Atcmd has NULL param!!!\r\n", __func__);
			ret = BT_AT_ERR_PARAM_INVALID;
			goto exit;
		}
		if (check_str_whitespace(argv[j])) {
			ret = BT_AT_ERR_PARAM_INVALID;
			goto exit;
		}
	}

	while (cmd_table[i].cmd_name) {
		if (0 == strcmp(cmd_str, cmd_table[i].cmd_name)) {
			if (argc < cmd_table[i].argc_min || argc > cmd_table[i].argc_max) {
				BT_LOGE("%s %s failed: wrong args number: %d, right range: [%d-%d]!\r\n",
						tag, cmd_str, argc, cmd_table[i].argc_min, cmd_table[i].argc_max);
				ret = BT_AT_ERR_PARAM_INVALID;
				goto exit;
			}

			ret = cmd_table[i].cmd_func(argc - 1, &argv[1]);
			goto exit;
		}
		i++;
	}

	BT_LOGE("%s Error: Cant find this cmd %s\r\n", tag, cmd_str);
	ret = BT_AT_ERR_CMD_INVALID;

exit:
	return ret;
}