#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>             /* offsetof */
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>

#include <linux/genetlink.h>
#include <rtw_sdio_bridge_netlink.h>
#include <rtw_scan_res.h>

#define GENLMSG_DATA(glh)	((void *)((char*)NLMSG_DATA(glh) + GENL_HDRLEN))
#define GENLMSG_PAYLOAD(glh)	(NLMSG_PAYLOAD(glh, 0) - GENL_HDRLEN)
#define NLA_DATA(na)		((void *)((char*)(na) + NLA_HDRLEN))
#define NLA_PAYLOAD(len)	(len - NLA_HDRLEN)

#define MAX_MSG_SIZE	1024
#define IFNAME "eth0"
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

static inline void nla_put_u32(unsigned char **pbuf, int attrtype, __u32 value)
{
	struct nlattr *nla = (struct nlattr *)(*pbuf);
	int size = sizeof(__u32);

	nla->nla_type = attrtype;
	nla->nla_len = nla_attr_size(size);

	memcpy(NLA_DATA(nla), &value, size);

	*pbuf += nla_total_size(size);
}

static inline void nla_put_u64(unsigned char **pbuf, int attrtype, __u64 value)
{
	struct nlattr *nla = (struct nlattr *)(*pbuf);
	int size = sizeof(__u64);

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

/*
 * Create a raw netlink socket and bind
 */
static int _bridge_create_nl_socket(int protocol)
{
	int fd;
	struct sockaddr_nl local;

	/* gen socket */
	fd = socket(AF_NETLINK, SOCK_RAW, protocol);
	if (fd < 0) {
		return -1;
	}

	memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();

	if (bind(fd, (struct sockaddr *) &local, sizeof(local)) < 0) {
		goto error;
	}

	return fd;

error:
	close(fd);
	return -1;
}

static void _bridge_fill_nlhdr(struct msgtemplate *msg, __u16 nlmsg_type, __u32 nlmsg_pid, __u8 genl_cmd)
{
	/* fill in header: nlmsghdr & genlmsghdr */
	msg->n.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
	msg->n.nlmsg_type = nlmsg_type;
	msg->n.nlmsg_flags = NLM_F_REQUEST;
	msg->n.nlmsg_seq = 0;
	msg->n.nlmsg_pid = nlmsg_pid;
	msg->g.cmd = genl_cmd;
	msg->g.version = 1;
}

static int _bridge_send_buf(int fd, char *buf, int buflen)
{
	struct sockaddr_nl nladdr;
	int r;

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;

	/* send until finish */
	while ((r = sendto(fd, buf, buflen, 0, (struct sockaddr *) &nladdr,
					   sizeof(nladdr))) < buflen) {
		if (r > 0) {
			buf += r;
			buflen -= r;
		} else if (errno != EAGAIN) {
			return -1;
		}
	}

	return 0;
}
/*
 * Probe the controller in genetlink to find the family id
 * for the DEMO_GEN_CTRL family
 */
static int _bridge_get_family_id(int fd, char *family_name)
{
	struct msgtemplate ans;
	int id = 0;
	struct nlattr *na;
	int rep_len;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	/* find family id from family name */
	_bridge_fill_nlhdr(&msg, GENL_ID_CTRL, getpid(), CTRL_CMD_GETFAMILY);
	nla_put_string(&ptr, CTRL_ATTR_FAMILY_NAME, family_name);
	msg.n.nlmsg_len += ptr - msg.buf;
	_bridge_send_buf(fd, (char *)&msg, msg.n.nlmsg_len);

	/* recv message from kernel */
	rep_len = recv(fd, &ans, sizeof(ans), 0);
	if (ans.n.nlmsg_type == NLMSG_ERROR || (rep_len < 0) || !NLMSG_OK((&ans.n), rep_len)) {
		return 0;
	}

	/* parse message to get family id */
	na = (struct nlattr *) GENLMSG_DATA(&ans);
	na = (struct nlattr *)((char *) na + NLA_ALIGN(na->nla_len));
	if (na->nla_type == CTRL_ATTR_FAMILY_ID) {
		id = *(__u16 *) NLA_DATA(na);
	}

	return id;
}

static void _bridge_print_scan_result(struct rtw_scan_result *record)
{
	printf("%s\t ", (record->bss_type == RTW_BSS_TYPE_ADHOC) ? "Adhoc" : "Infra");
	printf("%02x:%02x:%02x:%02x:%02x:%02x,", record->BSSID.octet[0], record->BSSID.octet[1], record->BSSID.octet[2], record->BSSID.octet[3], record->BSSID.octet[4],
		   record->BSSID.octet[5]);
	/* cal complement for logs */
	printf(" %d\t ", record->signal_strength);
	printf(" %d\t  ", record->channel);
	printf("%s\t\t ", (record->security == RTW_SECURITY_OPEN) ? "Open               " :
		   (record->security == RTW_SECURITY_WEP_PSK) ? "WEP" :
		   (record->security == RTW_SECURITY_WPA_TKIP_PSK) ? "WPA TKIP" :
		   (record->security == RTW_SECURITY_WPA_AES_PSK) ? "WPA AES" :
		   (record->security == RTW_SECURITY_WPA_MIXED_PSK) ? "WPA Mixed" :
		   (record->security == RTW_SECURITY_WPA2_AES_PSK) ? "WPA2 AES" :
		   (record->security == RTW_SECURITY_WPA2_TKIP_PSK) ? "WPA2 TKIP" :
		   (record->security == RTW_SECURITY_WPA2_MIXED_PSK) ? "WPA2 Mixed" :
		   (record->security == RTW_SECURITY_WPA_WPA2_TKIP_PSK) ? "WPA/WPA2 TKIP" :
		   (record->security == RTW_SECURITY_WPA_WPA2_AES_PSK) ? "WPA/WPA2 AES" :
		   (record->security == RTW_SECURITY_WPA_WPA2_MIXED_PSK) ? "WPA/WPA2 Mixed" :
		   (record->security == (RTW_SECURITY_WPA_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA TKIP Enterprise" :
		   (record->security == (RTW_SECURITY_WPA_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA AES Enterprise" :
		   (record->security == (RTW_SECURITY_WPA_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA Mixed Enterprise" :
		   (record->security == (RTW_SECURITY_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA2 TKIP Enterprise" :
		   (record->security == (RTW_SECURITY_WPA2_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA2 AES Enterprise" :
		   (record->security == (RTW_SECURITY_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA2 Mixed Enterprise" :
		   (record->security == (RTW_SECURITY_WPA_WPA2_TKIP_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 TKIP Enterprise" :
		   (record->security == (RTW_SECURITY_WPA_WPA2_AES_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 AES Enterprise" :
		   (record->security == (RTW_SECURITY_WPA_WPA2_MIXED_PSK | ENTERPRISE_ENABLED)) ? "WPA/WPA2 Mixed Enterprise" :

		   (record->security == RTW_SECURITY_WPA3_AES_PSK) ? "WPA3-SAE AES" :
		   (record->security == RTW_SECURITY_WPA2_WPA3_MIXED) ? "WPA2/WPA3-SAE AES" :
		   (record->security == (WPA3_SECURITY | ENTERPRISE_ENABLED)) ? "WPA3 Enterprise" :
		   "Unknown            ");

	printf(" %s ", record->SSID.val);
	printf("\r\n");
}

static int _bridge_get_device_ip(int family_id, int *ip_addr)
{
	int ret = 0;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;
	__u32 ip = 0;
	struct msgtemplate ans;
	struct nlattr *na;
	int rep_len;
	__u8 iptab[4];
	int nl_fd;

	/* initialize socket */
	nl_fd = _bridge_create_nl_socket(NETLINK_GENERIC);
	if (nl_fd < 0) {
		fprintf(stderr, "failed to create netlink socket\n");
		return 0;
	}

	_bridge_fill_nlhdr(&msg, family_id, getpid(), BRIDGE_CMD_ECHO);
	nla_put_u32(&ptr, BRIDGE_ATTR_API_ID, CMD_GET_IP);

	msg.n.nlmsg_len += ptr - msg.buf;
	ret = _bridge_send_buf(nl_fd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	/*wait response*/
	rep_len = recv(nl_fd, &ans, sizeof(ans), 0);
	if (ans.n.nlmsg_type == NLMSG_ERROR || (rep_len < 0) || !NLMSG_OK((&ans.n), rep_len)) {
		return 0;
	}

	/* parse message to get ip */
	if (ans.g.cmd == BRIDGE_CMD_REPLY) {
		na = (struct nlattr *) GENLMSG_DATA(&ans);
		ip = *(__u32 *) NLA_DATA(na);
		iptab[0] = (__u8)(ip >> 24);
		iptab[1] = (__u8)(ip >> 16);
		iptab[2] = (__u8)(ip >> 8);
		iptab[3] = (__u8)(ip);
		memcpy(ip_addr, &ip, 4);
		printf("IP Address : %d.%d.%d.%d\n", iptab[3], iptab[2], iptab[1], iptab[0]);
	}
}

static int _bridge_config_host_ip(int *ip_addr)
{
	struct ifreq ifr;
	struct sockaddr_in sai;
	int sockfd;                     /* socket fd we use to manipulate stuff with */
	int selector;
	unsigned char mask;
	char *p;

	/* Create a channel to the NET kernel. */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	/* get interface name */
	strncpy(ifr.ifr_name, IFNAME, IFNAMSIZ);

	memset(&sai, 0, sizeof(struct sockaddr));
	sai.sin_family = AF_INET;
	sai.sin_port = 0;

	sai.sin_addr.s_addr = (__u32)(*ip_addr);

	p = (char *) &sai;
	memcpy((((char *)&ifr + ifreq_offsetof(ifr_addr))),
		   p, sizeof(struct sockaddr));

	ioctl(sockfd, SIOCSIFADDR, &ifr);
	ioctl(sockfd, SIOCGIFFLAGS, &ifr);

	ifr.ifr_flags |= IFF_UP | IFF_RUNNING;

	ioctl(sockfd, SIOCSIFFLAGS, &ifr);
	close(sockfd);
}

int bridge_wifi_connect(char *ssid, char *pwd)
{
	int ret = 0;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;
	struct msgtemplate ans;
	struct nlattr *na;
	int rep_len;
	int nl_fd;
	int nl_family_id = 0;
	int ip_addr = 0;

	/* initialize socket */
	nl_fd = _bridge_create_nl_socket(NETLINK_GENERIC);
	if (nl_fd < 0) {
		fprintf(stderr, "failed to create netlink socket\n");
		return 0;
	}

	/* get family id */
	nl_family_id = _bridge_get_family_id(nl_fd, SDIO_BRIDGE_GENL_NAME);
	if (!nl_family_id) {
		fprintf(stderr, "Failed to get family id, errno %d\n", errno);
		close(nl_fd);
		return -1;
	}

	_bridge_fill_nlhdr(&msg, nl_family_id, getpid(), BRIDGE_CMD_ECHO);
	nla_put_u32(&ptr, BRIDGE_ATTR_API_ID, CMD_WIFI_CONNECT);
	nla_put_string(&ptr, BRIDGE_ATTR_SSID, ssid);
	if (pwd) {
		nla_put_string(&ptr, BRIDGE_ATTR_PWD, pwd);
	}
	msg.n.nlmsg_len += ptr - msg.buf;
	ret = _bridge_send_buf(nl_fd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	/*wait response*/
	rep_len = recv(nl_fd, &ans, sizeof(ans), 0);
	if (ans.n.nlmsg_type == NLMSG_ERROR || (rep_len < 0) || !NLMSG_OK((&ans.n), rep_len)) {
		close(nl_fd);
		return 0;
	}

	/* parse message to get connect result */
	if (ans.g.cmd == BRIDGE_CMD_REPLY) {
		na = (struct nlattr *) GENLMSG_DATA(&ans);
		if (na->nla_type == BRIDGE_ATTR_API_RET) {
			ret = *(__s32 *) NLA_DATA(na);
			if (ret < 0) {
				printf("wifi connect fail\n");
			} else {
				printf("wifi connect success\n");
				//config ip address here
				close(nl_fd);
				_bridge_get_device_ip(nl_family_id, &ip_addr);
				_bridge_config_host_ip(&ip_addr);
				return 0;
			}
		}
	}
	close(nl_fd);
}

int bridge_wifi_disconnect(void)
{
	int ret = 0;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;
	int nl_fd;
	int nl_family_id = 0;

	/* initialize socket */
	nl_fd = _bridge_create_nl_socket(NETLINK_GENERIC);
	if (nl_fd < 0) {
		fprintf(stderr, "failed to create netlink socket\n");
		return 0;
	}

	/* get family id */
	nl_family_id = _bridge_get_family_id(nl_fd, SDIO_BRIDGE_GENL_NAME);
	if (!nl_family_id) {
		fprintf(stderr, "Failed to get family id, errno %d\n", errno);
		goto out;
	}

	_bridge_fill_nlhdr(&msg, nl_family_id, getpid(), BRIDGE_CMD_ECHO);
	nla_put_u32(&ptr, BRIDGE_ATTR_API_ID, CMD_WIFI_DISCONNECT);

	msg.n.nlmsg_len += ptr - msg.buf;
	ret = _bridge_send_buf(nl_fd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}
out:
	close(nl_fd);
}

int bridge_wifi_scan(void)
{
	int ret = 0;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;
	__u32 ip = 0;
	struct msgtemplate ans;
	struct nlattr *na;
	int rep_len;
	int nl_fd;
	int nl_family_id = 0;

	/* initialize socket */
	nl_fd = _bridge_create_nl_socket(NETLINK_GENERIC);
	if (nl_fd < 0) {
		fprintf(stderr, "failed to create netlink socket\n");
		return 0;
	}

	/* get family id */
	nl_family_id = _bridge_get_family_id(nl_fd, SDIO_BRIDGE_GENL_NAME);
	if (!nl_family_id) {
		fprintf(stderr, "Failed to get family id, errno %d\n", errno);
		goto out;
	}

	_bridge_fill_nlhdr(&msg, nl_family_id, getpid(), BRIDGE_CMD_ECHO);
	nla_put_u32(&ptr, BRIDGE_ATTR_API_ID, CMD_WIFI_SCAN);

	msg.n.nlmsg_len += ptr - msg.buf;
	ret = _bridge_send_buf(nl_fd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}
out:
	close(nl_fd);
}


int bridge_wifi_get_scanres(void)
{
	int ret = 0;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;
	struct msgtemplate ans;
	struct nlattr *na;
	int rep_len;
	__u8 *scanbuf;
	__u32 ap_num;
	struct rtw_scan_result *scanned_AP_info;
	int i = 0;
	int nl_fd;
	int nl_family_id = 0;

	/* initialize socket */
	nl_fd = _bridge_create_nl_socket(NETLINK_GENERIC);
	if (nl_fd < 0) {
		fprintf(stderr, "failed to create netlink socket\n");
		return 0;
	}

	/* get family id */
	nl_family_id = _bridge_get_family_id(nl_fd, SDIO_BRIDGE_GENL_NAME);
	if (!nl_family_id) {
		fprintf(stderr, "Failed to get family id, errno %d\n", errno);
		goto out;
	}

	scanbuf = (__u8 *)malloc(64 * sizeof(struct rtw_scan_result));
	memset(scanbuf, 0, 64 * sizeof(struct rtw_scan_result));

	_bridge_fill_nlhdr(&msg, nl_family_id, getpid(), BRIDGE_CMD_ECHO);
	nla_put_u32(&ptr, BRIDGE_ATTR_API_ID, CMD_GET_SCAN_RES);
	nla_put_u64(&ptr, BRIDGE_ATTR_SCAN_RES_ADDR, (__u64)(scanbuf));

	msg.n.nlmsg_len += ptr - msg.buf;
	ret = _bridge_send_buf(nl_fd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	/*wait response to get actual scanned ap num*/
	rep_len = recv(nl_fd, &ans, sizeof(ans), 0);
	if (ans.n.nlmsg_type == NLMSG_ERROR || (rep_len < 0) || !NLMSG_OK((&ans.n), rep_len)) {
		return 0;
	}

	if (ans.g.cmd == BRIDGE_CMD_REPLY) {
		na = (struct nlattr *) GENLMSG_DATA(&ans);
		ap_num = *(__u32 *) NLA_DATA(na);
		/* print scan res */
		if (ap_num > 0) {
			for (i = 0; i < ap_num; i++) {
				printf("%d\t ", (i + 1));
				scanned_AP_info = (struct rtw_scan_result *)(scanbuf + i * sizeof(struct rtw_scan_result));
				scanned_AP_info->SSID.val[scanned_AP_info->SSID.len] = 0; /* Ensure the SSID is null terminated */
				_bridge_print_scan_result(scanned_AP_info);
			}
		}
	}
	free(scanbuf);
out:
	close(nl_fd);
}


