#include <errno.h>
#include <netinet/in.h> // for AF_NETLINK
#include <unistd.h>  // for close

#include <rtw_bridge_netlink.h>
#include <rtw_bridge_api.h>

/*
 * Create a raw netlink socket and bind
 */
static int _bridge_create_nl_socket(int protocol, int pid)
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
	_bridge_fill_nlhdr(&msg, GENL_ID_CTRL, 0, CTRL_CMD_GETFAMILY);
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
		id = *(uint16_t *) NLA_DATA(na);
	}

	return id;
}

int bridge_wifi_get_mac_addr(void)
{
	int nl_fd;
	int nl_family_id = 0;
	int ret = 0;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	/* initialize socket */
	nl_fd = _bridge_create_nl_socket(NETLINK_GENERIC, getpid());
	if (nl_fd < 0) {
		fprintf(stderr, "failed to create netlink socket\n");
		return 0;
	}

	/* get family id */
	nl_family_id = _bridge_get_family_id(nl_fd, BRIDGE_GENL_NAME);
	if (!nl_family_id) {
		fprintf(stderr, "Failed to get family id, errno %d\n", errno);
		close(nl_fd);
		return -1;
	}

	_bridge_fill_nlhdr(&msg, nl_family_id, 0, BRIDGE_CMD_ECHO);
	nla_put_u32(&ptr, BRIDGE_ATTR_API_ID, CMD_GET_MAC_ADDR);

	msg.n.nlmsg_len += ptr - msg.buf;
	ret = _bridge_send_buf(nl_fd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	close(nl_fd);

	return ret;

}

int bridge_wifi_get_ip(void)
{
	int nl_fd;
	int nl_family_id = 0;
	int ret = 0;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;

	/* initialize socket */
	nl_fd = _bridge_create_nl_socket(NETLINK_GENERIC, getpid());
	if (nl_fd < 0) {
		fprintf(stderr, "failed to create netlink socket\n");
		return 0;
	}

	/* get family id */
	nl_family_id = _bridge_get_family_id(nl_fd, BRIDGE_GENL_NAME);
	if (!nl_family_id) {
		fprintf(stderr, "Failed to get family id, errno %d\n", errno);
		close(nl_fd);
		return -1;
	}

	_bridge_fill_nlhdr(&msg, nl_family_id, 0, BRIDGE_CMD_ECHO);
	nla_put_u32(&ptr, BRIDGE_ATTR_API_ID, CMD_GET_IP);

	msg.n.nlmsg_len += ptr - msg.buf;
	ret = _bridge_send_buf(nl_fd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	close(nl_fd);

	return ret;
}


int bridge_wifi_set_host_state(void)
{
	int nl_fd;
	int nl_family_id = 0;
	struct msgtemplate msg;
	unsigned char *ptr = msg.buf;
	int ret = 0;

	/* initialize socket */
	nl_fd = _bridge_create_nl_socket(NETLINK_GENERIC, getpid());
	if (nl_fd < 0) {
		fprintf(stderr, "failed to create netlink socket\n");
		return 0;
	}

	/* get family id */
	nl_family_id = _bridge_get_family_id(nl_fd, BRIDGE_GENL_NAME);
	if (!nl_family_id) {
		fprintf(stderr, "Failed to get family id, errno %d\n", errno);
		close(nl_fd);
		return -1;
	}

	_bridge_fill_nlhdr(&msg, nl_family_id, 0, BRIDGE_CMD_ECHO);
	nla_put_u32(&ptr, BRIDGE_ATTR_API_ID, CMD_SET_HOST_STATE);

	msg.n.nlmsg_len += ptr - msg.buf;
	ret = _bridge_send_buf(nl_fd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	close(nl_fd);

	return ret;
}

int bridge_wifi_set_mac(char *mac)
{
	int nl_fd;
	int nl_family_id = 0;
	struct msgtemplate msg;
	int ret = 0;
	unsigned char *ptr = msg.buf;

	/* initialize socket */
	nl_fd = _bridge_create_nl_socket(NETLINK_GENERIC, getpid());
	if (nl_fd < 0) {
		fprintf(stderr, "failed to create netlink socket\n");
		return 0;
	}

	/* get family id */
	nl_family_id = _bridge_get_family_id(nl_fd, BRIDGE_GENL_NAME);
	if (!nl_family_id) {
		fprintf(stderr, "Failed to get family id, errno %d\n", errno);
		close(nl_fd);
		return -1;
	}

	_bridge_fill_nlhdr(&msg, nl_family_id, 0, BRIDGE_CMD_ECHO);
	nla_put_u32(&ptr, BRIDGE_ATTR_API_ID, CMD_WIFI_SET_MAC);
	nla_put_string(&ptr, BRIDGE_ATTR_MAC, mac);
	msg.n.nlmsg_len += ptr - msg.buf;

	ret = _bridge_send_buf(nl_fd, (char *)&msg, msg.n.nlmsg_len);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	close(nl_fd);

	return ret;
}
