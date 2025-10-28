#include <errno.h>
#include <netinet/in.h> // for AF_NETLINK
#include <unistd.h>  // for close

#include <whc_host_netlink.h>
#include <whc_host_app_api.h>

extern struct whc_netlink whc_netlink_info;
/*
 * Create a raw netlink socket and bind
 */
int whc_host_api_create_nl_socket(int protocol, int pid)
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
	local.nl_pid = pid;

	if (bind(fd, (struct sockaddr *) &local, sizeof(local)) < 0) {
		close(fd);
		return -1;
	}

	return fd;
}


/*
 * Probe the controller in genetlink to find the family id
 * for the DEMO_GEN_CTRL family
 */
int whc_host_api_get_family_id(int fd, char *family_name)
{
	struct msgtemplate ans;
	int id = 0;
	struct nlattr *na;
	int rep_len;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	/* find family id from family name */
	whc_host_fill_nlhdr(&msg, GENL_ID_CTRL, 0, CTRL_CMD_GETFAMILY);
	nla_put_string(&ptr, CTRL_ATTR_FAMILY_NAME, family_name);
	msg.n.nlmsg_len += ptr - msg.buf;
	whc_host_api_send_to_kernel(fd, (char *)&msg, msg.n.nlmsg_len);

	/* recv message from kernel */
	rep_len = recv(fd, &ans, sizeof(ans), 0);
	if (ans.n.nlmsg_type == NLMSG_ERROR || (rep_len < 0) || !NLMSG_OK((&ans.n), rep_len)) {
		return 0;
	}

	/* parse message to get family id */
	na = (struct nlattr *) GENLMSG_DATA(&ans);
	na = (struct nlattr *)((char *) na + NLA_ALIGN(na->nla_len));
	if (na->nla_type == CTRL_ATTR_FAMILY_ID) {
		id = *(uint16_t *) NLA_DATA(na);
	}

	return id;
}

int whc_host_api_send_to_kernel(int fd, char *buf, int buflen)
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


// WHC_ATTR_PAYLOAD: size(4B) + payload
int whc_host_api_send_nl_data(uint8_t *buf, uint32_t buf_len)
{
	int ret = 0;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	whc_host_fill_nlhdr(&msg, whc_netlink_info.family_id, 0, WHC_CMD_ECHO);
	nla_put_u32(&ptr, WHC_ATTR_API_ID, CMD_WIFI_SEND_BUF);
	nla_put_payload(&ptr, WHC_ATTR_PAYLOAD, buf, buf_len);
	msg.n.nlmsg_len += ptr - msg.buf;
	ret = whc_host_api_send_to_kernel(whc_netlink_info.sockfd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	return ret;

}

void whc_host_set_ps_cmd(struct whc_dev_ps_cmd *pcmd, char *cmd_arg)
{
	if (strcmp(cmd_arg, "r") == 0) {
		pcmd->type = WHC_CMD_TICKPS_R;
	} else if (strcmp(cmd_arg, "a") == 0) {
		pcmd->type = WHC_CMD_TICKPS_A;
	} else if (strcmp(cmd_arg, "cg") == 0) {
		pcmd->type = WHC_CMD_TICKPS_TYPE_CG;
	} else if (strcmp(cmd_arg, "pg") == 0) {
		pcmd->type = WHC_CMD_TICKPS_TYPE_PG;
	}

	return;
}