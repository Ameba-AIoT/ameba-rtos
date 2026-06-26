/**
  ******************************************************************************
  * @file    nan_app.c
  * @brief   Minimal km4_tz (WHC host) NAN control + AT command front-end.
  *          Drives the de-hostified rtw_nan_api_* (which talk to the NP-core
  *          NAN driver over the WHC IPC path) for bring-up: init / start /
  *          publish / subscribe / data-path req-rsp-end / stop / deinit, plus
  *          an event-echo callback that also latches the peer address learned
  *          from discovery and the ndp-id learned from an incoming data-path
  *          request, so the data-path commands need no typed-in addresses.
  ******************************************************************************
  */

#include "atcmd_service.h"
#include "rtw_nan_cmd_api.h"
#include "rtw_nan_vendor_def.h"
#include "lwip_netconf.h"
#include "lwip/etharp.h"

#ifdef CONFIG_WIFI_NAN_HOST_APP

#define NAN_APP_INTF "nan0"   /* ignored by the IPC transport; kept non-NULL */

/* Bring-up state. One board acts as publisher OR subscriber per test, so a
 * single latched service + a single data-path scratch struct are enough. The
 * data-path struct is static (not on the AT task stack) as it is ~320 bytes. */
static struct srvc_info g_srvc;
static struct datapath_info g_dp;
static u8 g_peer_nmi[ETH_ALEN];      /* publisher NMI from discovery */
static u8 g_peer_pub_id;             /* publisher's publish id from discovery */
static u8 g_have_disc;
static u8 g_req_ndp_id;              /* ndp id from an incoming data-path req */
static u8 g_req_initiator[ETH_ALEN]; /* initiator NDI from incoming req */
static u8 g_have_req;
static u8 g_peer_ndi[ETH_ALEN];      /* peer NAN data-interface MAC (for ARP) */

/* Event echo: runs on the WHC host event task. Latch the bits the data-path
 * commands need and print a one-liner; no IPC is issued from here. */
static void nan_app_evt_cb(uint8_t event_id, void *buf, uint32_t len)
{
	switch (event_id) {
	case NAN_EVT_DISCOVERY_RESULT: {
		struct nan_evt_disc_result_info *d = buf;
		memcpy(g_peer_nmi, d->peer_nmi, ETH_ALEN);
		g_peer_pub_id = d->publish_id;
		g_have_disc = 1;
		INFO_PRINT("[nan_app] DISC peer=%02x:%02x:%02x:%02x:%02x:%02x pub_id=%d sub_id=%d rssi=%d\n",
				   d->peer_nmi[0], d->peer_nmi[1], d->peer_nmi[2], d->peer_nmi[3],
				   d->peer_nmi[4], d->peer_nmi[5], d->publish_id, d->subscirbe_id, d->peer_rssi);
		break;
	}
	case NAN_EVT_DATAPATH_RECV_REQ: {
		struct rtw_nan_datapath_request_received_event_data *r = buf;
		g_req_ndp_id = r->datapath_id;
		memcpy(g_req_initiator, r->initiator_data_address.ether_addr_octet, ETH_ALEN);
		g_have_req = 1;
		INFO_PRINT("[nan_app] DP_REQ ndp_id=%d pub_id=%d init=%02x:%02x:%02x:%02x:%02x:%02x\n",
				   r->datapath_id, r->publish_id,
				   g_req_initiator[0], g_req_initiator[1], g_req_initiator[2],
				   g_req_initiator[3], g_req_initiator[4], g_req_initiator[5]);
		break;
	}
	case NAN_EVT_DATA_INDICATION: {
		/* This Realtek NAN core signals "responder received an NDP request,
		 * please respond" via a data-indication event (carries ndp_id +
		 * initiator data address), not a separate datapath-req event. */
		struct nan_evt_data_indication_info *i = buf;
		g_req_ndp_id = i->ndp_id;
		memcpy(g_req_initiator, i->initiator_data_address, ETH_ALEN);
		memcpy(g_peer_ndi, i->initiator_data_address, ETH_ALEN);
		g_have_req = 1;
		INFO_PRINT("[nan_app] DP_INDICATION ndp_id=%d pub_id=%d init=%02x:%02x:%02x:%02x:%02x:%02x\n",
				   i->ndp_id, i->publish_id,
				   g_req_initiator[0], g_req_initiator[1], g_req_initiator[2],
				   g_req_initiator[3], g_req_initiator[4], g_req_initiator[5]);
		break;
	}
	case NAN_EVT_DATAPATH_ESTABLISHED: {
		struct nan_evt_data_established_info *e = buf;
		/* Initiator never sees a data-indication, so learn the peer (responder)
		 * NDI here; the responder already latched the initiator NDI. */
		if (!g_have_req) {
			memcpy(g_peer_ndi, e->responder_data_address, ETH_ALEN);
		}
		INFO_PRINT("[nan_app] DP_ESTABLISHED ndp_id=%d pub_id=%d ndi=%02x:%02x:%02x:%02x:%02x:%02x\n",
				   e->ndp_id, e->publish_id,
				   e->responder_data_address[0], e->responder_data_address[1],
				   e->responder_data_address[2], e->responder_data_address[3],
				   e->responder_data_address[4], e->responder_data_address[5]);
		break;
	}
	case NAN_EVT_DATA_CONFIRM: {
		struct nan_evt_data_confirm_info *c = buf;
		INFO_PRINT("[nan_app] DP_CONFIRM ndp_id=%d status=%d\n", c->ndp_id, c->status);
		break;
	}
	case NAN_EVT_DATAPATH_END:
		INFO_PRINT("[nan_app] DP_END\n");
		break;
	default:
		INFO_PRINT("[nan_app] event id=%d len=%u\n", event_id, (unsigned)len);
		break;
	}
}

static void at_nan_init(u16 argc, char **argv)
{
	(void)argc;
	(void)argv;
	rtw_nan_api_register_evt_cb(nan_app_evt_cb);
	if (rtw_nan_api_init(0, NAN_APP_INTF) != RTW_RET_STATUS_SUCCESS) {
		at_printf(ATCMD_ERROR_END_STR, -1);
		return;
	}
	at_printf(ATCMD_OK_END_STR);
}

static void at_nan_deinit(u16 argc, char **argv)
{
	(void)argc;
	(void)argv;
	rtw_nan_api_deinit(0);
	at_printf(ATCMD_OK_END_STR);
}

static void at_nan_start(u16 argc, char **argv)
{
	(void)argc;
	(void)argv;
	if (rtw_nan_api_start_nan(NAN_APP_INTF) != RTW_RET_STATUS_SUCCESS) {
		at_printf(ATCMD_ERROR_END_STR, -1);
		return;
	}
	at_printf(ATCMD_OK_END_STR);
}

static void at_nan_stop(u16 argc, char **argv)
{
	(void)argc;
	(void)argv;
	rtw_nan_api_stop_nan(NAN_APP_INTF);
	at_printf(ATCMD_OK_END_STR);
}

/* AT+NANPUB=<service_name>[,<publish_id>] */
static void at_nan_publish(u16 argc, char **argv)
{
	if (argc < 2 || argv[1] == NULL) {
		at_printf(ATCMD_ERROR_END_STR, -1);
		return;
	}
	memset(&g_srvc, 0, sizeof(g_srvc));
	strncpy((char *)g_srvc.service_name, argv[1], MAX_SRVC_NAME_LEN - 1);
	g_srvc.publish_id = (argc > 2 && argv[2]) ? (uint8_t)atoi(argv[2]) : 1;
	g_srvc.sec_type = 0;   /* open (1/2/3 = PMK/passphrase/pairing) */

	if (rtw_nan_api_publish(NAN_APP_INTF, &g_srvc) != RTW_RET_STATUS_SUCCESS) {
		at_printf(ATCMD_ERROR_END_STR, -1);
		return;
	}
	at_printf(ATCMD_OK_END_STR);
}

/* AT+NANSUB=<service_name>[,<subscribe_id>] */
static void at_nan_subscribe(u16 argc, char **argv)
{
	if (argc < 2 || argv[1] == NULL) {
		at_printf(ATCMD_ERROR_END_STR, -1);
		return;
	}
	memset(&g_srvc, 0, sizeof(g_srvc));
	strncpy((char *)g_srvc.service_name, argv[1], MAX_SRVC_NAME_LEN - 1);
	g_srvc.subscribe_id = (argc > 2 && argv[2]) ? (uint8_t)atoi(argv[2]) : 1;
	g_srvc.sec_type = 0;   /* open */

	if (rtw_nan_api_subscribe(NAN_APP_INTF, &g_srvc) != RTW_RET_STATUS_SUCCESS) {
		at_printf(ATCMD_ERROR_END_STR, -1);
		return;
	}
	at_printf(ATCMD_OK_END_STR);
}

/* AT+NANDPREQ : subscriber initiates a data path to the discovered publisher.
 * Uses the peer NMI + publisher id latched from the discovery-result event. */
static void at_nan_dp_req(u16 argc, char **argv)
{
	(void)argc;
	(void)argv;
	if (!g_have_disc) {
		at_printf(ATCMD_ERROR_END_STR, -1);
		return;
	}
	g_srvc.publish_id = g_peer_pub_id;
	memset(&g_dp, 0, sizeof(g_dp));
	g_dp.ndp_id = 0;          /* new data path */
	g_dp.service = &g_srvc;
	g_dp.sec_type = 0;        /* open */
	memcpy(g_dp.responder_ndi, g_peer_nmi, ETH_ALEN);

	if (rtw_nan_api_send_datapath_req(&g_dp) != RTW_RET_STATUS_SUCCESS) {
		at_printf(ATCMD_ERROR_END_STR, -1);
		return;
	}
	at_printf(ATCMD_OK_END_STR);
}

/* AT+NANDPRSP : publisher accepts an incoming data-path request. Uses the
 * ndp-id + initiator NDI latched from the data-path-request event. */
static void at_nan_dp_rsp(u16 argc, char **argv)
{
	(void)argc;
	(void)argv;
	if (!g_have_req) {
		at_printf(ATCMD_ERROR_END_STR, -1);
		return;
	}
	memset(&g_dp, 0, sizeof(g_dp));
	g_dp.ndp_id = g_req_ndp_id;
	g_dp.service = &g_srvc;
	g_dp.sec_type = 0;        /* open */
	memcpy(g_dp.initiator_ndi, g_req_initiator, ETH_ALEN);

	if (rtw_nan_api_send_datapath_rsp(&g_dp) != RTW_RET_STATUS_SUCCESS) {
		at_printf(ATCMD_ERROR_END_STR, -1);
		return;
	}
	at_printf(ATCMD_OK_END_STR);
}

/* AT+NANDPEND[=<ndp_id>] : tear down a data path (defaults to last ndp-id). */
static void at_nan_dp_end(u16 argc, char **argv)
{
	uint8_t id = (argc > 1 && argv[1]) ? (uint8_t)atoi(argv[1]) : g_req_ndp_id;
	rtw_nan_api_send_datapath_end(id);
	at_printf(ATCMD_OK_END_STR);
}

/* AT+NANDPIP : bring the NAN data netif (index 2) up with a static IPv4 +
 * a static ARP entry for the peer NDI, so a plain IPv4 ping flows over the
 * open NDP. This RTOS port has LWIP_IPV6 off; the NDP carries Ethernet frames
 * regardless of L3, so 192.168.99.x over if2 works. Responder takes .1,
 * initiator .2 (role inferred from whether a data-indication was seen). */
static void at_nan_dp_ip(u16 argc, char **argv)
{
	u32 self_ip;
	u8 peer_last;
	ip4_addr_t peer_ip;
	struct eth_addr peer_eth;

	(void)argc;
	(void)argv;
	if (g_have_req) {
		self_ip = 0xC0A86301;
		peer_last = 2;   /* responder = .1 */
	} else {
		self_ip = 0xC0A86302;
		peer_last = 1;   /* initiator = .2 */
	}

	lwip_set_ip(NETIF_WLAN_NAN_INDEX, self_ip, 0xFFFFFF00, self_ip);
	lwip_netif_set_up(NETIF_WLAN_NAN_INDEX);
	lwip_netif_set_link_up(NETIF_WLAN_NAN_INDEX);

	IP4_ADDR(&peer_ip, 192, 168, 99, peer_last);
	memcpy(peer_eth.addr, g_peer_ndi, ETH_ALEN);
	etharp_add_static_entry(&peer_ip, &peer_eth);

	INFO_PRINT("[nan_app] NAN if2 self=192.168.99.%d peer=192.168.99.%d peer_ndi=%02x:%02x:%02x:%02x:%02x:%02x\n",
			   g_have_req ? 1 : 2, peer_last,
			   g_peer_ndi[0], g_peer_ndi[1], g_peer_ndi[2],
			   g_peer_ndi[3], g_peer_ndi[4], g_peer_ndi[5]);
	at_printf(ATCMD_OK_END_STR);
}

ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_nan_items[] = {
	{"+NANINIT",   at_nan_init},
	{"+NANDEINIT", at_nan_deinit},
	{"+NANSTART",  at_nan_start},
	{"+NANSTOP",   at_nan_stop},
	{"+NANPUB",    at_nan_publish},
	{"+NANSUB",    at_nan_subscribe},
	{"+NANDPREQ",  at_nan_dp_req},
	{"+NANDPRSP",  at_nan_dp_rsp},
	{"+NANDPEND",  at_nan_dp_end},
	{"+NANDPIP",   at_nan_dp_ip},
};

/* AT table registration is linker-section based on amebagreen2/km4_tz; this
 * hook is kept for future state init, not for command registration. */
void nan_app_at_init(void)
{
}

#endif /* CONFIG_WIFI_NAN_HOST_APP */
