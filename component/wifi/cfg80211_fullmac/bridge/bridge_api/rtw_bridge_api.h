#ifndef _RTW_SDIO_BRIDGE_API_H_
#define _RTW_SDIO_BRIDGE_API_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>             /* offsetof */
#include <stdint.h>
#include <string.h>
#include <linux/genetlink.h>

#define GENLMSG_DATA(glh)	((void *)((char*)NLMSG_DATA(glh) + GENL_HDRLEN))
#define GENLMSG_PAYLOAD(glh)	(NLMSG_PAYLOAD(glh, 0) - GENL_HDRLEN)
#define NLA_DATA(na)		((void *)((char*)(na) + NLA_HDRLEN))
#define NLA_PAYLOAD(len)	(len - NLA_HDRLEN)

#define MAX_MSG_SIZE	512
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

static inline void _bridge_fill_nlhdr(struct msgtemplate *msg, uint16_t nlmsg_type, uint32_t nlmsg_pid, uint8_t genl_cmd)
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

int bridge_wifi_get_ip(void);
int bridge_wifi_get_mac_addr(void);
int bridge_wifi_set_host_state(void);
int bridge_wifi_set_mac(char *mac);
#endif
