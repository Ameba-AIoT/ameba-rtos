#ifndef _RTW_CTRL_API_H_
#define _RTW_CTRL_API_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>             /* offsetof */
#include <stdint.h>
#include <string.h>
#include <linux/genetlink.h>
#include <rtw_ctrl_wifi_types.h>

#define MAX_ARG_COUNT 5
#define MAX_ARG_LENGTH 128
#define MAX_INPUT_SIZE 640

#define GENLMSG_DATA(glh)	((void *)((char*)NLMSG_DATA(glh) + GENL_HDRLEN))
#define GENLMSG_PAYLOAD(glh)	(NLMSG_PAYLOAD(glh, 0) - GENL_HDRLEN)
#define NLA_DATA(na)		((void *)((char*)(na) + NLA_HDRLEN))
#define NLA_PAYLOAD(len)	(len - NLA_HDRLEN)

#define MAX_MSG_SIZE	2000
#define IFNAME "eth_sta0"
#define ifreq_offsetof(x)  offsetof(struct ifreq, x)

extern int num_of_ap;
extern int num_of_ap_index;
extern struct rtw_scan_result *scan_results;

extern int family_id;
extern int sock_fd;


#define MSG_EVENT_SCANING "CTRL-EVENT-SCANING"
#define MSG_EVENT_SCAN_COMPLETE "CTRL-EVENT-SCAN-PROCESS-COMPLETE "
#define MSG_EVENT_GET_SCAN_RESULT "CTRL-EVENT-GET-SCAN-RESULT "

#define MSG_EVENT_FAIL_BUSY "FAIL-BUSY"



//For Event handle

typedef void (*event_handler_t)(void);

struct event_func_t {
	u32 wifi_event;
	u8 *event_msg;
	event_handler_t handler;
};

//For Send CMD
struct whc_host_cli_cmd_t {
	u32 category;       // WHC_WIFI_TEST
	u8  cmd_id;         // WHC_WIFI_CMD_SCAN
	u8  idx;            // For Interface
	u8  TBD[7];
} __attribute__((packed));



struct whc_host_cli_send_string_t {
	u32 category;       // WHC_WIFI_TEST
	u8  cmd_id;         // WHC_WIFI_CMD_SCAN
	u32 msg_len;
	u8  message[256];
} __attribute__((packed));


typedef void (*cmd_handler_t)(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);

struct cmd_func_t {
	const char *cmd;
	cmd_handler_t handler;
	u8  api_id;         // For netlink category
	u32 cmd_category;
	u8 cmd_id;
};

/* API End */



/* Netlink */
struct msgtemplate {
	struct nlmsghdr n;
	struct genlmsghdr g;
	unsigned char buf[MAX_MSG_SIZE];
};

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

static inline void nla_put_u64(unsigned char **pbuf, int attrtype, uint64_t value)
{
	struct nlattr *nla = (struct nlattr *)(*pbuf);
	int size = sizeof(uint64_t);

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
	nla->nla_len = nla_attr_size(size); //include hdr

	memcpy(NLA_DATA(nla), str, size);

	*pbuf += nla_total_size(size); // include hdr and pad
}

static inline void whc_bridge_host_fill_nlhdr(struct msgtemplate *msg, uint16_t nlmsg_type, uint32_t nlmsg_pid, uint8_t genl_cmd)
{
	/* fill in header: nlmsghdr & genlmsghdr */
	msg->n.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
	msg->n.nlmsg_type = nlmsg_type;
	msg->n.nlmsg_flags = NLM_F_REQUEST;
	msg->n.nlmsg_seq = 0;
	msg->n.nlmsg_pid = nlmsg_pid;
	msg->g.cmd = genl_cmd;
	msg->g.version = 1;
	msg->g.reserved = 0;
}

static inline void nla_put_payload(unsigned char **pbuf, int attrtype, const char *buf, uint32_t size)
{
	struct nlattr *nla = (struct nlattr *)(*pbuf);

	nla->nla_type = attrtype;
	nla->nla_len = nla_attr_size(size + 4); //include hdr

	*(uint32_t *)(NLA_DATA(nla)) = size;
	memcpy(((char *)NLA_DATA(nla)) + 4, buf, size);

	*pbuf += nla_total_size(size + 4); // include hdr and pad
}

/**
 * nla_ok - check if the netlink attribute fits into the remaining bytes
 * @nla: netlink attribute
 * @remaining: number of bytes remaining in attribute stream
 */
static inline int nla_ok(const struct nlattr *nla, int remaining)
{
	return remaining >= (int) sizeof(*nla) &&
		   nla->nla_len >= sizeof(*nla) &&
		   nla->nla_len <= remaining;
}

/**
 * nla_next - next netlink attribute in attribute stream
 * @nla: netlink attribute
 * @remaining: number of bytes remaining in attribute stream
 *
 * Returns the next netlink attribute in the attribute stream and
 * decrements remaining by the size of the current attribute.
 */
static inline struct nlattr *nla_next(const struct nlattr *nla, int *remaining)
{
	unsigned int totlen = NLA_ALIGN(nla->nla_len);

	*remaining -= totlen;
	return (struct nlattr *)((char *) nla + totlen);
}

/**
 *  * nla_len - length of payload
 *   * @nla: netlink attribute
 *    */
static inline int nla_len(const struct nlattr *nla)
{
	return nla->nla_len - NLA_HDRLEN;
}

/* buf in user space send to kernel, final kernel to device */
int whc_bridge_host_api_send_nl_data(uint8_t whc_cmd, uint8_t *buf, uint32_t buf_len, uint8_t api_id);
int whc_bridge_host_api_send_nl_string(uint8_t whc_cmd, uint8_t *buf, uint32_t buf_len, uint8_t api_id);

int whc_bridge_host_api_create_nl_socket(int protocol, int pid);
int whc_bridge_host_api_send_to_kernel(int fd, char *buf, int buflen);
int whc_bridge_host_api_get_family_id(int fd, char *family_name);
void whc_bridge_host_cmd_scan(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);
void whc_bridge_host_cmd_scan_result(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);
void whc_bridge_host_cmd_getmac(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);
void whc_bridge_host_cmd_set_network(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);

void whc_host_cli_cmd_mp(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);
void whc_host_cli_cmd_dbg(int argc, char **argv, u8 api_id, u32 cmd_category, u8 cmd_id);

void print_scan_result(struct rtw_scan_result *record);
void whc_bridge_host_print_scan_result(char *pos, int chunk_index, char last_chunk);
void event_handler_scan_complete(void);
void whc_cmd_handle_rx_list_network(char *pos);

#endif
