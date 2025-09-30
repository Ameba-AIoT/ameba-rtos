#ifndef _WHC_HOST_APP_API_H_
#define _WHC_HOST_APP_API_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>             /* offsetof */
#include <stdint.h>
#include <string.h>
#include <linux/genetlink.h>

typedef int (*cmd_handler_t)(void);

struct whc_host_command_entry {
	char *cmd_name;
	cmd_handler_t handler;
};

#define GENLMSG_DATA(glh)	((void *)((char*)NLMSG_DATA(glh) + GENL_HDRLEN))
#define GENLMSG_PAYLOAD(glh)	(NLMSG_PAYLOAD(glh, 0) - GENL_HDRLEN)
#define NLA_DATA(na)		((void *)((char*)(na) + NLA_HDRLEN))
#define NLA_PAYLOAD(len)	(len - NLA_HDRLEN)

#define MAX_MSG_SIZE	2000
#define IFNAME "eth_sta0"
#define ifreq_offsetof(x)  offsetof(struct ifreq, x)

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

static inline void whc_host_fill_nlhdr(struct msgtemplate *msg, uint16_t nlmsg_type, uint32_t nlmsg_pid, uint8_t genl_cmd)
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

/* buf in user space send to kernel, final kernel to device */
int whc_host_api_send_nl_data(uint8_t *buf, uint32_t buf_len);

int whc_host_set_mac_internal(uint8_t idx, char *mac);
int whc_host_api_create_nl_socket(int protocol, int pid);
int whc_host_api_send_to_kernel(int fd, char *buf, int buflen);
int whc_host_api_get_family_id(int fd, char *family_name);


int whc_host_get_mac(void);
int whc_host_get_ip(void);
int whc_host_set_state(void);
int whc_host_set_tickps_cmd(void);
int whc_host_set_netif_on(void);
int whc_host_set_mac(void);
int whc_host_wifi_mp(void);
int whc_host_wifi_dbg(void);
int whc_host_set_netif_on(void);
int whc_host_wifi_connect(void);
int whc_host_wifi_scan(void);
int whc_host_wifi_on(void);
int whc_host_wifi_dhcp(void);
int whc_host_nl_init(void);


#endif
