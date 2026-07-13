/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>

#include "rtw_nl.h"

/* ---------------------------------------------------------------------------
 * Constants - from whc_host_netlink.h
 * ------------------------------------------------------------------------- */

/* WHC Netlink family name registered by the kernel driver */
#define WHC_CMD_GENL_NAME	"whc_demo"

/* ---------------------------------------------------------------------------
 * API IDs (enum whc_cmd_api_id) - used as WHC_ATTR_API_ID value
 * ------------------------------------------------------------------------- */
#define CMD_WIFI_SET_MAC	3
#define CMD_WIFI_NETIF_ON	4
#define CMD_WIFI_SEND_BUF	5
#define CMD_WIFI_INFO_INIT	6
#define CMD_WIFI_MP		10
#define CMD_WIFI_DBG		11

/* ---------------------------------------------------------------------------
 * Attribute types (enum whc_attr_type)
 * ------------------------------------------------------------------------- */
#define WHC_ATTR_API_ID		1
#define WHC_ATTR_WLAN_IDX	2
#define WHC_ATTR_STRING		3
#define WHC_ATTR_PAYLOAD	4

/* ---------------------------------------------------------------------------
 * WHC command types (enum whc_cmd_type)
 * ------------------------------------------------------------------------- */
#define WHC_CMD_ECHO		2	/* user -> kernel */
#define WHC_CMD_EVENT		3	/* kernel -> user (async event) */

/* ---------------------------------------------------------------------------
 * WHC_WIFI_TEST sub-commands (embedded in CMD_WIFI_SEND_BUF payload)
 * ------------------------------------------------------------------------- */
#define WHC_WIFI_TEST		0xffa5a5a5

#define WHC_WIFI_TEST_GET_MAC_ADDR	0x01
#define WHC_WIFI_TEST_GET_IP		0x02
#define WHC_WIFI_TEST_SET_READY		0x03
#define WHC_WIFI_TEST_SET_UNREADY	0x04
#define WHC_WIFI_TEST_SET_TICKPS_CMD	0x05
#define WHC_WIFI_TEST_CONNECT		0x06
#define WHC_WIFI_TEST_SCAN		0x07
#define WHC_WIFI_TEST_DHCP		0x08
#define WHC_WIFI_TEST_WIFION		0x09
#define WHC_WIFI_TEST_SCAN_RESULT	0x0A
#define WHC_WIFI_TEST_MP		0x0B
#define WHC_WIFI_TEST_OTA		0x10
#define WHC_WIFI_TEST_LOG_ENABLE	0x15
#define WHC_WIFI_TEST_LOG_DISABLE	0x16

/* ---------------------------------------------------------------------------
 * MP result fragmentation (from whc_def.h / whc_host_app.c):
 * the MP reply is split into WHC_MP_FRAG_NUM events, each carrying
 * [frag_idx(1)] + WHC_MP_FRAG_SIZE bytes; the host reassembles them.
 * ------------------------------------------------------------------------- */
#define WHC_MP_MSG_BUF_SIZE	4096
#define WHC_MP_FRAG_NUM		3
#define WHC_MP_FRAG_SIZE	((WHC_MP_MSG_BUF_SIZE + WHC_MP_FRAG_NUM - 1) / WHC_MP_FRAG_NUM)

/* ---------------------------------------------------------------------------
 * Netlink helper macros
 * ------------------------------------------------------------------------- */
#define GENLMSG_DATA(glh)	((void *)((char*)NLMSG_DATA(glh) + GENL_HDRLEN))
#define NLA_DATA(na)		((void *)((char*)(na) + NLA_HDRLEN))

#define MAX_MSG_SIZE		2048

struct msgtemplate {
	struct nlmsghdr n;
	struct genlmsghdr g;
	unsigned char buf[MAX_MSG_SIZE];
};

/* ---------------------------------------------------------------------------
 * NLA attribute writers (inline, to build netlink messages)
 * ------------------------------------------------------------------------- */

static inline int nla_attr_size(int payload)
{
	return NLA_HDRLEN + payload;
}

static inline int nla_total_size(int payload)
{
	return NLA_ALIGN(nla_attr_size(payload));
}

static inline void nla_put_u8(unsigned char **pbuf, int attrtype, uint8_t value)
{
	struct nlattr *nla = (struct nlattr *)(*pbuf);
	int size = sizeof(uint8_t);

	nla->nla_type = attrtype;
	nla->nla_len = nla_attr_size(size);
	memcpy(NLA_DATA(nla), &value, size);
	*pbuf += nla_total_size(size);
}

static inline void nla_put_u32(unsigned char **pbuf, int attrtype, uint32_t value)
{
	struct nlattr *nla = (struct nlattr *)(*pbuf);
	int size = sizeof(uint32_t);

	nla->nla_type = attrtype;
	nla->nla_len = nla_attr_size(size);
	memcpy(NLA_DATA(nla), &value, size);
	*pbuf += nla_total_size(size);
}

static inline void nla_put_string(unsigned char **pbuf, int attrtype, const char *str)
{
	struct nlattr *nla = (struct nlattr *)(*pbuf);
	int size = strlen(str) + 1;

	nla->nla_type = attrtype;
	nla->nla_len = nla_attr_size(size);
	memcpy(NLA_DATA(nla), str, size);
	*pbuf += nla_total_size(size);
}

static inline void nla_put_payload(unsigned char **pbuf, int attrtype,
								   const uint8_t *buf, uint32_t size)
{
	struct nlattr *nla = (struct nlattr *)(*pbuf);

	nla->nla_type = attrtype;
	/* payload attribute: 4-byte length prefix + actual data */
	nla->nla_len = nla_attr_size(size + 4);

	*(uint32_t *)(NLA_DATA(nla)) = size;
	memcpy(((char *)NLA_DATA(nla)) + 4, buf, size);

	*pbuf += nla_total_size(size + 4);
}

/*
 * Normalize MP reply text and print netlink format:
 *   Private Message: <msg>
 *   #
 * (ioctl path prints driver reply as-is without this prefix.)
 */
static void nl_trim_mp_msg(char *msg)
{
	size_t len;
	char *p;

	if (!msg || !*msg) {
		return;
	}

	p = msg;
	while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
		p++;
	}
	if (p != msg) {
		memmove(msg, p, strlen(p) + 1);
	}

	if (strncmp(msg, "Private Message:", 16) == 0) {
		p = msg + 16;
		while (*p == ' ') {
			p++;
		}
		memmove(msg, p, strlen(p) + 1);
	}

	len = strlen(msg);
	while (len > 0 && (msg[len - 1] == ' ' || msg[len - 1] == '\t' ||
					   msg[len - 1] == '\r' || msg[len - 1] == '\n')) {
		msg[--len] = '\0';
	}
	if (len > 0 && msg[len - 1] == '#') {
		msg[--len] = '\0';
	}
	while (len > 0 && (msg[len - 1] == ' ' || msg[len - 1] == '\t' ||
					   msg[len - 1] == '\r' || msg[len - 1] == '\n')) {
		msg[--len] = '\0';
	}
}

static void nl_print_mp_reply(const char *msg)
{
	char buf[WHC_MP_MSG_BUF_SIZE + 1];

	if (!msg || !*msg) {
		return;
	}

	strncpy(buf, msg, sizeof(buf) - 1);
	buf[sizeof(buf) - 1] = '\0';
	nl_trim_mp_msg(buf);
	printf("Private Message: %s\n#\n", buf);
}

static void whc_host_fill_nlhdr(struct msgtemplate *msg, uint16_t nlmsg_type,
								uint32_t nlmsg_pid, uint8_t genl_cmd)
{
	msg->n.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
	msg->n.nlmsg_type = nlmsg_type;
	msg->n.nlmsg_flags = NLM_F_REQUEST;
	msg->n.nlmsg_seq = 0;
	msg->n.nlmsg_pid = nlmsg_pid;
	msg->g.cmd = genl_cmd;
	msg->g.version = 1;
	msg->g.reserved = 0;
}

/* ---------------------------------------------------------------------------
 * Low-level netlink helpers
 * ------------------------------------------------------------------------- */

static int nl_create_socket(void)
{
	int fd;

	fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);
	if (fd < 0) {
		fprintf(stderr, "Failed to create NETLINK_GENERIC socket: %s\n",
				strerror(errno));
		return -1;
	}

	/* Set 10-second receive timeout to prevent hanging */
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		fprintf(stderr, "Failed to set socket receive timeout: %s\n",
				strerror(errno));
		close(fd);
		return -1;
	}

	struct sockaddr_nl local;
	memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();

	if (bind(fd, (struct sockaddr *)&local, sizeof(local)) < 0) {
		fprintf(stderr, "Failed to bind netlink socket: %s\n",
				strerror(errno));
		close(fd);
		return -1;
	}

	return fd;
}

static int nl_send_to_kernel(int fd, char *buf, int buflen)
{
	struct sockaddr_nl nladdr;
	int r;
	int retries = 0;

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;

	while ((r = sendto(fd, buf, buflen, 0,
					   (struct sockaddr *)&nladdr, sizeof(nladdr))) < buflen) {
		if (r > 0) {
			buf += r;
			buflen -= r;
		} else if (errno != EAGAIN) {
			return -1;
		} else if (++retries > 10) {
			fprintf(stderr, "sendto failed: kernel buffer full after %d retries\n",
					retries - 1);
			return -1;
		} else {
			usleep(1000); /* 1 ms back-off before retry */
		}
	}
	return 0;
}

/*
 * Resolve the Generic Netlink family ID for "whc_demo".
 */
static int nl_get_family_id(int fd)
{
	struct msgtemplate msg;
	struct msgtemplate ans;
	int rep_len;
	int id = 0;
	unsigned char *ptr = msg.buf;

	memset(&msg, 0, sizeof(msg));
	memset(&ans, 0, sizeof(ans));

	whc_host_fill_nlhdr(&msg, GENL_ID_CTRL, 0, CTRL_CMD_GETFAMILY);
	nla_put_string(&ptr, CTRL_ATTR_FAMILY_NAME, WHC_CMD_GENL_NAME);
	msg.n.nlmsg_len += ptr - msg.buf;

	if (nl_send_to_kernel(fd, (char *)&msg, msg.n.nlmsg_len) < 0) {
		fprintf(stderr, "Failed to send CTRL_CMD_GETFAMILY\n");
		return 0;
	}

	rep_len = recv(fd, &ans, sizeof(ans), 0);
	if (rep_len < 0) {
		fprintf(stderr, "Failed to receive family ID response: %s\n",
				strerror(errno));
		return 0;
	}

	if (ans.n.nlmsg_type == NLMSG_ERROR) {
		struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(&ans);
		fprintf(stderr, "CTRL_CMD_GETFAMILY error: %s\n",
				strerror(-err->error));
		return 0;
	}

	if (!NLMSG_OK((&ans.n), rep_len)) {
		fprintf(stderr, "Invalid response to CTRL_CMD_GETFAMILY\n");
		return 0;
	}

	/* Parse attributes, searching for CTRL_ATTR_FAMILY_ID */
	{
		struct nlattr *attr = (struct nlattr *)GENLMSG_DATA(&ans);
		int remaining = NLMSG_PAYLOAD(&ans.n, sizeof(struct genlmsghdr));

		while (remaining >= (int)sizeof(*attr)) {
			int len = attr->nla_len;

			if (len < (int)sizeof(*attr) || len > remaining) {
				break;
			}
			if (attr->nla_type == CTRL_ATTR_FAMILY_ID) {
				id = *(uint16_t *)NLA_DATA(attr);
				break;
			}
			remaining -= NLA_ALIGN(len);
			attr = (struct nlattr *)((char *)attr + NLA_ALIGN(len));
		}
	}

	return id;
}

/*
 * Send init (CMD_WIFI_INFO_INIT) to register this socket with the kernel.
 */
static int nl_send_init(int fd, int family_id)
{
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	memset(&msg, 0, sizeof(msg));
	whc_host_fill_nlhdr(&msg, family_id, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_INFO_INIT);
	msg.n.nlmsg_len += ptr - msg.buf;

	if (nl_send_to_kernel(fd, (char *)&msg, msg.n.nlmsg_len) < 0) {
		fprintf(stderr, "Failed to send init via Netlink: %s\n",
				strerror(errno));
		return -1;
	}
	return 0;
}

/*
 * Generic one-shot Netlink transaction:
 *   1. create socket  2. resolve family  3. send init
 *   Then caller fills &msg with the command, then this sends + optionally recvs.
 *
 * @msg:   partially filled message (nlmsg_type will be overwritten with family_id)
 * @recv_reply: reply mode (RTW_NL_NO_REPLY / RTW_NL_REPLY_ONCE / RTW_NL_REPLY_SCAN)
 * @expect_sub: WHC_WIFI_TEST sub-command to wait for in single-reply mode.
 *              >= 0 : discard unrelated events and keep waiting until this
 *                     sub-command arrives (or the recv times out).
 *              <  0 : accept the first event (legacy behaviour).
 * Returns 0 on success, -1 on error.
 */
static int nl_do_transaction(struct msgtemplate *msg, int recv_reply, int expect_sub,
							 char *mp_outbuf, size_t mp_outlen)
{
	int fd = -1;
	int family_id = 0;
	int ret = -1;

	fd = nl_create_socket();
	if (fd < 0) {
		return -1;
	}

	family_id = nl_get_family_id(fd);
	if (family_id == 0) {
		fprintf(stderr,
				"Failed to resolve Netlink family '%s'. "
				"Is the WiFi driver loaded?\n",
				WHC_CMD_GENL_NAME);
		close(fd);
		return -1;
	}

	/* Send CMD_WIFI_INFO_INIT so the kernel saves our port for replies */
	if (nl_send_init(fd, family_id) < 0) {
		close(fd);
		return -1;
	}

	/* Set the resolved family ID and send the command */
	msg->n.nlmsg_type = family_id;
	if (nl_send_to_kernel(fd, (char *)msg, msg->n.nlmsg_len) < 0) {
		fprintf(stderr, "Failed to send command via Netlink: %s\n",
				strerror(errno));
		close(fd);
		return -1;
	}

	/* Optionally wait for one or more WHC_CMD_EVENT replies.
	 *
	 * RTW_NL_REPLY_ONCE: read a single reply (getmac/getip/connect/...).
	 * RTW_NL_REPLY_SCAN: keep reading SCAN_RESULT events until the kernel
	 *                    sends the end-of-scan marker (idx == 0) or the
	 *                    receive times out, mirroring the persistent
	 *                    whc_host_app poll() loop. */
	if (recv_reply) {
		int looping = 1;
		/* MP reply reassembly buffer (RTW_NL_REPLY_MP only) */
		uint8_t mpfrag_buf[WHC_MP_FRAG_NUM * WHC_MP_FRAG_SIZE + 1];
		int mpfrag_received = 0;

		memset(mpfrag_buf, 0, sizeof(mpfrag_buf));

		while (looping) {
			struct msgtemplate ans;
			int rep_len;

			memset(&ans, 0, sizeof(ans));
			rep_len = recv(fd, &ans, sizeof(ans), 0);
			if (rep_len < 0) {
				if (errno == EAGAIN || errno == EWOULDBLOCK) {
					/* For streaming modes (scan/MP) a timeout
					 * just ends the stream and is not fatal. */
					if (recv_reply == RTW_NL_REPLY_SCAN
						|| recv_reply == RTW_NL_REPLY_MP) {
						break;
					}
					fprintf(stderr,
							"Netlink command timed out (10s).\n");
				} else {
					fprintf(stderr,
							"Failed to receive Netlink reply: %s\n",
							strerror(errno));
				}
				close(fd);
				return -1;
			}

			if (ans.n.nlmsg_type == NLMSG_ERROR) {
				struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(&ans);
				fprintf(stderr, "Netlink error: %s\n",
						strerror(-err->error));
				close(fd);
				return -1;
			}

			/* Parse WHC_CMD_EVENT response.
			 * Layout: [magic(4)][sub_cmd(1)][data...], so
			 * data[4] = sub_cmd and payload = data + 5. */
			if (ans.g.cmd == WHC_CMD_EVENT
				&& rep_len > (int)(NLMSG_HDRLEN + GENL_HDRLEN)) {
				struct nlattr *na = (struct nlattr *)GENLMSG_DATA(&ans);
				uint8_t *data = (uint8_t *)NLA_DATA(na);
				uint32_t whc_event = *(uint32_t *)data;

				if (whc_event == WHC_WIFI_TEST) {
					uint8_t sub_cmd = data[4];
					uint8_t *payload = data + 5;

					/* In single-reply mode with a specific expected
					 * sub-command, ignore unrelated events that may
					 * already be queued and keep waiting for ours. */
					if (expect_sub >= 0
						&& recv_reply != RTW_NL_REPLY_SCAN
						&& sub_cmd != (uint8_t)expect_sub) {
						continue;
					}

					switch (sub_cmd) {
					case WHC_WIFI_TEST_GET_MAC_ADDR:
						/* payload: [idx(1)][mac(6)] */
						printf("MAC ADDR %02x:%02x:%02x:%02x:%02x:%02x\n",
							   payload[1], payload[2], payload[3],
							   payload[4], payload[5], payload[6]);
						break;
					case WHC_WIFI_TEST_GET_IP:
						/* payload: [ip(4)][gw(4)] */
						printf("IP ADDR %d.%d.%d.%d GW %d %d %d %d\n",
							   payload[0], payload[1], payload[2], payload[3],
							   payload[4], payload[5], payload[6], payload[7]);
						break;
					case WHC_WIFI_TEST_SCAN_RESULT:
						/* whc_host_scan_result(): idx at payload[0],
						 * AP info string at payload[3]. idx == 0 marks
						 * the end of the scan result stream. */
						if (payload[0] == 0) {
							looping = 0;
						} else {
							printf("%s", (char *)(payload + 3));
						}
						continue;  /* scan stream controls its own loop */
					case WHC_WIFI_TEST_MP: {
						/* MP reply is fragmented:
						 * payload = [frag_idx(1)][chunk(WHC_MP_FRAG_SIZE)].
						 * Reassemble WHC_MP_FRAG_NUM fragments, then print. */
						uint8_t frag_idx = payload[0];

						if (frag_idx < WHC_MP_FRAG_NUM) {
							memcpy(mpfrag_buf + frag_idx * WHC_MP_FRAG_SIZE,
								   payload + 1, WHC_MP_FRAG_SIZE);
							if (++mpfrag_received >= WHC_MP_FRAG_NUM) {
								if (mp_outbuf && mp_outlen > 0) {
									strncpy(mp_outbuf, (char *)mpfrag_buf, mp_outlen - 1);
									mp_outbuf[mp_outlen - 1] = '\0';
								} else {
									nl_print_mp_reply((char *)mpfrag_buf);
								}
								looping = 0;
							}
						}
						continue;
					}
					default:
						/* Print raw hex dump for unknown sub-commands */
					{
						int i;
						int dlen = rep_len - (int)(NLMSG_HDRLEN + GENL_HDRLEN + NLA_HDRLEN + 5);
						printf("Event 0x%02x (%d bytes):", sub_cmd, dlen);
						for (i = 0; i < dlen && i < 32; i++) {
							printf(" %02x", payload[i]);
						}
						printf("\n");
					}
					break;
					}
				} else {
					/* Plain string response (e.g. dbg result).
					 * When filtering for a specific WHC sub-command,
					 * this isn't it: keep waiting. */
					if (expect_sub >= 0
						&& recv_reply != RTW_NL_REPLY_SCAN) {
						continue;
					}
					printf("%s\n", (char *)data);
				}
			} else {
				/* Unusable message; keep waiting if we're filtering. */
				if (expect_sub >= 0
					&& recv_reply != RTW_NL_REPLY_SCAN) {
					continue;
				}
			}

			/* Single-reply mode stops after one (matching) reply.
			 * Streaming modes (SCAN, MP) keep looping until their
			 * own end condition or a receive timeout. */
			if (recv_reply == RTW_NL_REPLY_ONCE) {
				looping = 0;
			}
		}
	}

	ret = 0;
	close(fd);
	return ret;
}

/* ---------------------------------------------------------------------------
 * Public API - rtw_nl_send_cmd (MP path via CMD_WIFI_SEND_BUF)
 *
 * The driver moved the "iwpriv-style" MP commands off the dedicated
 * CMD_WIFI_MP netlink command and onto CMD_WIFI_SEND_BUF (Pattern A).
 * The payload mirrors whc_host_wifi_mp() in the driver reference app:
 *   [WHC_WIFI_TEST(4)] [WHC_WIFI_TEST_MP(1)] [show_msg(1)] [cmd string + '\0']
 * The reply comes back as WHC_MP_FRAG_NUM fragmented WHC_WIFI_TEST_MP events,
 * which nl_do_transaction() reassembles (RTW_NL_REPLY_MP).
 * ------------------------------------------------------------------------- */

int rtw_nl_send_cmd(const char *cmd)
{
	uint8_t payload[6 + 1024];
	uint32_t cmd_len;
	uint32_t len;

	if (!cmd || !*cmd) {
		fprintf(stderr, "Empty command\n");
		return -1;
	}

	cmd_len = (uint32_t)strlen(cmd) + 1;	/* include trailing '\0' */
	if (cmd_len > sizeof(payload) - 6) {
		fprintf(stderr, "MP command too long\n");
		return -1;
	}

	*(uint32_t *)payload = WHC_WIFI_TEST;
	payload[4] = WHC_WIFI_TEST_MP;
	payload[5] = 1;				/* show_msg */
	memcpy(payload + 6, cmd, cmd_len);
	len = 6 + cmd_len;

	return rtw_nl_send_buf(payload, len, RTW_NL_REPLY_MP);
}

int rtw_nl_send_cmd_buf(const char *cmd, char *outbuf, size_t outlen)
{
	uint8_t payload[6 + 1024];
	uint32_t cmd_len;
	uint32_t len;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	if (!cmd || !*cmd) {
		fprintf(stderr, "Empty command\n");
		return -1;
	}
	if (!outbuf || outlen == 0) {
		fprintf(stderr, "Invalid output buffer\n");
		return -1;
	}

	outbuf[0] = '\0';
	cmd_len = (uint32_t)strlen(cmd) + 1;	/* include trailing '\0' */
	if (cmd_len > sizeof(payload) - 6) {
		fprintf(stderr, "MP command too long\n");
		return -1;
	}

	*(uint32_t *)payload = WHC_WIFI_TEST;
	payload[4] = WHC_WIFI_TEST_MP;
	payload[5] = 1;				/* show_msg */
	memcpy(payload + 6, cmd, cmd_len);
	len = 6 + cmd_len;

	memset(&msg, 0, sizeof(msg));
	whc_host_fill_nlhdr(&msg, 0, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_SEND_BUF);
	nla_put_payload(&ptr, WHC_ATTR_PAYLOAD, payload, len);
	msg.n.nlmsg_len += ptr - msg.buf;

	return nl_do_transaction(&msg, RTW_NL_REPLY_MP, WHC_WIFI_TEST_MP,
							 outbuf, outlen);
}

/* ---------------------------------------------------------------------------
 * Public API - rtw_nl_send_buf (Pattern A: CMD_WIFI_SEND_BUF)
 *
 * Sends a raw buffer to the device via CMD_WIFI_SEND_BUF.
 * The payload should contain WHC_WIFI_TEST + sub-command + params.
 * If expect_reply is set, waits for a WHC_CMD_EVENT response.
 * ------------------------------------------------------------------------- */

int rtw_nl_send_buf(const uint8_t *payload, uint32_t payload_len, int expect_reply)
{
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;
	int expect_sub = -1;  /* -1 = accept the first event (legacy behaviour) */

	if (!payload || payload_len == 0) {
		fprintf(stderr, "Empty payload\n");
		return -1;
	}

	/* For request/response-symmetric commands the reply carries the same
	 * sub-command as the request, so we can wait specifically for it and
	 * discard unrelated events that may already be queued on the socket.
	 * (connect etc. keep expect_sub = -1, preserving their behaviour.) */
	if (expect_reply == RTW_NL_REPLY_SCAN) {
		expect_sub = WHC_WIFI_TEST_SCAN_RESULT;
	} else if (expect_reply == RTW_NL_REPLY_MP) {
		expect_sub = WHC_WIFI_TEST_MP;
	} else if (expect_reply == RTW_NL_REPLY_ONCE && payload_len >= 5) {
		uint8_t req_sub = payload[4];
		if (req_sub == WHC_WIFI_TEST_GET_MAC_ADDR ||
			req_sub == WHC_WIFI_TEST_GET_IP) {
			expect_sub = req_sub;
		}
	}

	memset(&msg, 0, sizeof(msg));

	whc_host_fill_nlhdr(&msg, 0, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_SEND_BUF);
	nla_put_payload(&ptr, WHC_ATTR_PAYLOAD, payload, payload_len);
	msg.n.nlmsg_len += ptr - msg.buf;

	return nl_do_transaction(&msg, expect_reply, expect_sub, NULL, 0);
}

/* ---------------------------------------------------------------------------
 * Public API - rtw_nl_send_dbg (Pattern B: CMD_WIFI_DBG)
 * ------------------------------------------------------------------------- */

int rtw_nl_send_dbg(const char *dbg_cmd)
{
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	if (!dbg_cmd || !*dbg_cmd) {
		fprintf(stderr, "Empty dbg command\n");
		return -1;
	}

	memset(&msg, 0, sizeof(msg));

	whc_host_fill_nlhdr(&msg, 0, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_DBG);
	nla_put_string(&ptr, WHC_ATTR_STRING, dbg_cmd);
	msg.n.nlmsg_len += ptr - msg.buf;

	return nl_do_transaction(&msg, RTW_NL_REPLY_ONCE, -1, NULL, 0);
}

/* ---------------------------------------------------------------------------
 * Public API - rtw_nl_set_mac (Pattern B: CMD_WIFI_SET_MAC)
 * ------------------------------------------------------------------------- */

int rtw_nl_set_mac(uint8_t idx, const char *mac_str)
{
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	if (!mac_str || !*mac_str) {
		fprintf(stderr, "Empty MAC address\n");
		return -1;
	}

	memset(&msg, 0, sizeof(msg));

	whc_host_fill_nlhdr(&msg, 0, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_SET_MAC);
	nla_put_u8(&ptr, WHC_ATTR_WLAN_IDX, idx);
	nla_put_string(&ptr, WHC_ATTR_STRING, mac_str);
	msg.n.nlmsg_len += ptr - msg.buf;

	return nl_do_transaction(&msg, RTW_NL_NO_REPLY, -1, NULL, 0);
}

/* ---------------------------------------------------------------------------
 * Public API - rtw_nl_netif_on (Pattern B: CMD_WIFI_NETIF_ON)
 * ------------------------------------------------------------------------- */

int rtw_nl_netif_on(uint8_t idx)
{
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	memset(&msg, 0, sizeof(msg));

	whc_host_fill_nlhdr(&msg, 0, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_NETIF_ON);
	nla_put_u8(&ptr, WHC_ATTR_WLAN_IDX, idx);
	msg.n.nlmsg_len += ptr - msg.buf;

	return nl_do_transaction(&msg, RTW_NL_NO_REPLY, -1, NULL, 0);
}
