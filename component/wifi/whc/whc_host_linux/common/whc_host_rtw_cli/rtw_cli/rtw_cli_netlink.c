#include <errno.h>
#include <netinet/in.h> // for AF_NETLINK
#include <unistd.h>  // for close

#include <whc_host_netlink.h>
#include <whc_host_rtw_cli_api.h>
#include <rtw_cli_api.h>

static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err,
						 void *arg)
{
	//printf("%s\n", __FUNCTION__);

	return NL_SKIP;
}

static int ack_handler(struct nl_msg *msg, void *arg)
{
	int *err = arg;
	*err = 0;
	//printf("%s\n", __FUNCTION__);
	printf("OK\n");
	return NL_STOP;
}


static int no_seq_check(struct nl_msg *msg, void *arg)
{
	//printf("%s\n", __FUNCTION__);
	return NL_OK;
}

static int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
	return NL_SKIP;
}


static int rtw_cli_nl_rx_hdl(struct nl_msg *msg, void *arg)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb_msg[WHC_ATTR_MAX + 1];
	size_t payload_len = 0;
	uint32_t api_id = 0;
	char *payload = NULL;
	int chunk_index = 0;
	char last_chuck = 0;
	char string_data[256] = {0};


	nla_parse(tb_msg, WHC_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			  genlmsg_attrlen(gnlh, 0), NULL);

	if (tb_msg[WHC_ATTR_API_ID]) {
		//printf("WHC_ATTR_API_ID = %u\n", api_id);
		api_id = nla_get_u32(tb_msg[WHC_ATTR_API_ID]);
	}
	if (tb_msg[WHC_ATTR_STRING]) {
		strncpy(string_data,
				nla_get_string(tb_msg[WHC_ATTR_STRING]),
				sizeof(string_data) - 1);
	}
	if (tb_msg[WHC_ATTR_CHUNK_INDEX]) {
		chunk_index = nla_get_u32(tb_msg[WHC_ATTR_CHUNK_INDEX]);
	}
	if (tb_msg[WHC_ATTR_LAST_CHUNK]) {
		last_chuck = nla_get_u8(tb_msg[WHC_ATTR_LAST_CHUNK]);
	}
	if (tb_msg[WHC_ATTR_PAYLOAD]) {
		payload = nla_data(tb_msg[WHC_ATTR_PAYLOAD]);
		payload_len = nla_len(tb_msg[WHC_ATTR_PAYLOAD]);
	}


	whc_cmd_handle_rx_payload(payload, payload_len, api_id,
							  chunk_index, last_chuck);


	return 0;
}

int rtw_cli_init_nl_global(struct rtw_cli_netlink_global *global)
{
	int mcid;
	int ret = 0;
	int err = -ENOMEM;

	global->nl = nl_socket_alloc();
	if (!global->nl) {
		printf("nl_socket_alloc failed\n");
		return -1;
	}

	if (genl_connect(global->nl) != 0) {
		printf("genl_connect failed\n");
		nl_socket_free(global->nl);
		return -1;
	}

	global->family_id = genl_ctrl_resolve(global->nl, WHC_CMD_GENL_NAME);
	if (global->family_id < 0) {
		printf("Can't resolve family %s\n", WHC_CMD_GENL_NAME);
		nl_socket_free(global->nl);
		return -1;
	}

	mcid = genl_ctrl_resolve_grp(global->nl, WHC_CMD_GENL_NAME, "whc_event");
	if (mcid >= 0) {
		ret = nl_socket_add_membership(global->nl, mcid);
	}
	if (ret < 0) {
		printf("Could not add multicast "
			   "membership for events: %d (%s)",
			   ret, nl_geterror(ret));
		return -1;
	}

	global->nl_cb = nl_cb_alloc(NL_CB_DEFAULT);

	nl_cb_set(global->nl_cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);
	nl_cb_set(global->nl_cb, NL_CB_VALID, NL_CB_CUSTOM, rtw_cli_nl_rx_hdl, NULL);
}

void rtw_cli_deinit_nl_global(struct rtw_cli_netlink_global *global)
{

	if (global == NULL) {
		return;
	}

	if (global->nl != NULL) {
		nl_socket_free(global->nl);
	}

	nl_cb_put(global->nl_cb);
}

static void rtw_cli_nlmsg_clear(struct nl_msg *msg)
{
	/*
	 * Clear nlmsg data, e.g., to make sure key material is not left in
	 * heap memory for unnecessarily long time.
	 */
	if (msg) {
		struct nlmsghdr *hdr = nlmsg_hdr(msg);
		void *data = nlmsg_data(hdr);
		/*
		 * This would use nlmsg_datalen() or the older nlmsg_len() if
		 * only libnl were to maintain a stable API.. Neither will work
		 * with all released versions, so just calculate the length
		 * here.
		 */
		int len = hdr->nlmsg_len - NLMSG_HDRLEN;

		memset(data, 0, len);
	}
}




void rtw_cli_nl_recv(struct rtw_cli_netlink_global *global)
{

	nl_recvmsgs(global->nl, global->nl_cb);
}


int send_and_recv_msgs(struct nl_sock *nl_handle,
					   struct nl_msg *msg,
					   int (*valid_handler)(struct nl_msg *, void *),
					   void *valid_data)
{
	struct nl_cb *cb = NULL;
	int err = -ENOMEM;

	if (!msg) {
		return -ENOMEM;
	}

	cb = nl_cb_clone(global->nl_cb);
	if (!cb) {
		goto out;
	}


	err = nl_send_auto(nl_handle, msg);
	if (err < 0) {
		printf("nl_send_auto_complete() failed: %s",
			   nl_geterror(err));

		err = -EBADF;
		goto out;
	}


	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);

	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, no_seq_check, NULL);

	if (valid_handler) {
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM,
				  valid_handler, valid_data);
	} else {
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, rtw_cli_nl_rx_hdl, NULL);
	}

	err = 1;
	while (err > 0) {
		int res = nl_recvmsgs(nl_handle, cb);
		if (res < 0) {
			printf("%s->nl_recvmsgs failed: %d (%s)",
				   __func__, res, nl_geterror(res));
			break;
		}
	}

out:
	nl_cb_put(cb);

	/* Always clear the message as it can potentially contain keys */
	rtw_cli_nlmsg_clear(msg);

	return err;
}

int rtw_cli_send_nl_string(uint8_t whc_cmd, uint8_t *buf,
						   uint32_t buf_len, uint8_t api_id,
						   int (*valid_handler)(struct nl_msg *, void *),
						   void *valid_data)
{
	struct nl_msg *msg;
	int ret = 0;

	msg = nlmsg_alloc();
	if (!msg) {
		return 0;
	}

	genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, global->family_id,
				0, 0, whc_cmd, 0);

	nla_put_u32(msg, WHC_ATTR_API_ID, api_id);
	nla_put_string(msg, WHC_ATTR_STRING, buf);

	ret = send_and_recv_msgs(global->nl, msg, valid_handler, valid_data);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	if (msg) {
		nlmsg_free(msg);
	}

	return 0;
}

int rtw_cli_send_nl_data(uint8_t whc_cmd, uint8_t *buf,
						 uint32_t buf_len, uint8_t api_id,
						 int (*valid_handler)(struct nl_msg *, void *),
						 void *valid_data)
{
	struct nl_msg *msg;
	int ret = 0;
	u32 nl_port = getpid();


	msg = nlmsg_alloc();
	if (!msg) {
		return 0;
	}

	genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, global->family_id,
				0, 0, whc_cmd, 0);

	nla_put_u32(msg, WHC_ATTR_API_ID, api_id);
	nla_put(msg, WHC_ATTR_PAYLOAD, buf_len, buf);

	ret = send_and_recv_msgs(global->nl, msg, valid_handler, valid_data);
	if (ret < 0) {
		printf("msg send fail\n");
	}

	if (msg) {
		nlmsg_free(msg);
	}

	return ret;


}

