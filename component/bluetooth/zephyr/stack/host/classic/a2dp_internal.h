/** @file
 * @brief Advance Audio Distribution Profile Internal header.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* To be called when first SEP is being registered */
int bt_a2dp_init(void);

#ifdef ZEPHYR_FIX_CODE
static int a2dp_get_all_capabilities_ind(struct bt_avdtp *session,
			struct bt_avdtp_sep *sep, struct net_buf *rsp_buf, uint8_t *errcode);

static int bt_a2dp_suspend_cb(struct bt_avdtp_req *req);

static void a2dp_stream_connected(struct bt_avdtp_sep *sep);

static void a2dp_stream_disconnected(struct bt_avdtp_sep *sep);

static void stream_recv(struct bt_a2dp_stream *stream,
		struct net_buf *buf, uint16_t seq_num, uint32_t ts);
#endif