/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include "kv.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(settings, CONFIG_SETTINGS_LOG_LEVEL);

#define KV_NAME_LIST_BUF_SIZE  2048
#define KV_NAME_BUF_SIZE       64

static int settings_kv_load(struct settings_store *cs,
			     const struct settings_load_arg *arg);
static int settings_kv_save(struct settings_store *cs, const char *name,
			     const char *value, size_t val_len);

static const struct settings_store_itf settings_kv_itf = {
	.csi_load = settings_kv_load,
	.csi_save = settings_kv_save,
};

static struct settings_store default_settings_store_kv = {0};

static ssize_t settings_kv_read_fn(void *back_end, void *data, size_t len)
{
	ssize_t rc;
	char *name = (char *)back_end;

	rc = (ssize_t)rt_kv_get(name, data, len);

	LOG_DBG("KV read, key: %s, len: %d, ret: %d", name, len, rc);

	return rc;
}

static int key_name_char_replace(const char *src_name, char *dst_name, char from, char to)
{
	uint32_t i = 0;

	if (!src_name || !dst_name) {
		return -EINVAL;
	}

	memcpy(dst_name, src_name, strlen(src_name) + 1);
	while (dst_name[i] != 0) {
		if (dst_name[i] == from) {
			dst_name[i] = to;
		}
		i++;
	}

	return 0;
}

static int kv_get_settings_name(char *name_start, char *name, uint32_t len)
{
	uint32_t i = 0;

	while (name_start[i] != '\n' && name_start[i] != 0 && i < len) {
		if (' ' == name_start[i] && ':' == name_start[i + 1] && ' ' == name_start[i + 2]) {
			name[i] = 0;
			break;
		}
		name[i] = name_start[i];
		i++;
	}

	if (i == len) {
		return -ENOMEM;
	}

	return 0;
}

static int settings_kv_load(struct settings_store *cs,
			     			const struct settings_load_arg *arg)
{
	(void)cs;
	char *name_list = NULL;
	char *name_kv = NULL;
	char *name_settings = NULL;
	int ret = 0, i = 0, kv_size;
	bool ready = true;

	name_list = k_malloc(KV_NAME_LIST_BUF_SIZE);
	if (!name_list) {
		ret = -ENOMEM;
		goto exit;
	}

	name_kv = k_malloc(KV_NAME_BUF_SIZE);
	if (!name_kv) {
		ret = -ENOMEM;
		goto exit;
	}

	name_settings = k_malloc(KV_NAME_BUF_SIZE);
	if (!name_settings) {
		ret = -ENOMEM;
		goto exit;
	}

	memset(name_list, 0 , KV_NAME_LIST_BUF_SIZE);
	ret = rt_kv_list(name_list, KV_NAME_LIST_BUF_SIZE);
	if (ret < 0) {
		goto exit;
	}

	while (name_list[i] && i < KV_NAME_LIST_BUF_SIZE) {
		if (ready) {
			memset(name_kv, 0, KV_NAME_BUF_SIZE);
			ret = kv_get_settings_name(name_list + i, name_kv, KV_NAME_BUF_SIZE);
			if (ret) {
				goto exit;
			}
			LOG_INF("KV list each: %s", name_kv);

			memset(name_settings, 0, KV_NAME_BUF_SIZE);
			ret = key_name_char_replace(name_kv, name_settings, '#', '/');
			if (ret) {
				goto exit;
			}

			kv_size = rt_kv_size(name_kv);
			if (kv_size < 0) {
				ret = -ENOMEM;
				goto exit;
			}

			ret = settings_call_set_handler(name_settings, kv_size,
								settings_kv_read_fn, name_kv, (void *)arg);
			if (ret) {
				goto exit;
			}
		}

		ready = ('\n' == name_list[i]);
		i++;
	}

	if (KV_NAME_LIST_BUF_SIZE == i) {
		LOG_ERR("KV list over buf size");
		ret = -EINVAL;
	}

exit:
	if (name_list) {
		k_free(name_list);
	}
	if (name_kv) {
		k_free(name_kv);
	}
	if (name_settings) {
		k_free(name_settings);
	}
	return ret;
}

static int settings_kv_save(struct settings_store *cs, const char *name,
			     						const char *value, size_t val_len)
{
	(void)cs;
	char *name_kv;
	int ret;
	bool delete;

	if (!name) {
		return -EINVAL;
	}

	name_kv = k_malloc(strlen(name) + 1);
	if (!name_kv) {
		return -EINVAL;
	}
	memset(name_kv, 0, strlen(name) + 1);

	/* Find out if we are doing a delete */
	delete = ((value == NULL) || (val_len == 0));

	/* kv system cannot accept the key string with '/', so replace '/' with '#' */
	ret = key_name_char_replace(name, name_kv, '/', '#');
	if (ret) {
		goto exit;
	}

	if (delete) {
		ret = rt_kv_delete(name_kv);
		if (-ENOENT == ret) {
			LOG_WRN("Delete %s, but not found in kv", name_kv);
			/* If entry not exist, consider as delete success */
			ret = 0;
		}
	} else {
		ret = rt_kv_set(name_kv, value, val_len);
		if (ret == (int)val_len) {
			ret = 0;
		} else {
			ret = -EINVAL;
		}
	}

	LOG_DBG("KV %s, key: %s, len: %d, ret: %d",
			delete ? "delete" : "set", name_kv, val_len, ret);

exit:
	if (name_kv) {
		k_free(name_kv);
	}
	return ret;
}

int settings_backend_init(void)
{
	default_settings_store_kv.cs_itf = &settings_kv_itf;

	settings_src_register(&default_settings_store_kv);
	settings_dst_register(&default_settings_store_kv);

	return 0;
}