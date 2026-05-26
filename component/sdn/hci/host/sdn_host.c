#include <sdn_host.h>
#include <rtk_status.h>
#include <os_wrapper.h>
#include "rom/log.h"


#ifndef BIT
#define BIT(x)              (1UL << (x))
#endif

struct sdn_host_t {
	void *mutex;
	uint8_t protocols;
} g_sdn_host = {0};

extern int rtk_wpan_vhdlc_receive(uint8_t *buf, uint32_t length);
SDN_C2H_CB bt_c2h_cb = NULL;
uint8_t sdn_bt_addr[6] = {0};
uint8_t sdn_bt_addr_fixed = false;

void sdn_host_init(void)
{
	if (RTK_SUCCESS != rtos_mutex_create(&g_sdn_host.mutex)) {
		return;
	}
}

bool sdn_host_is_enabled(void)
{
	bool ret;

	rtos_mutex_take(g_sdn_host.mutex, MUTEX_WAIT_TIMEOUT);
	ret = g_sdn_host.protocols ? true : false;
	rtos_mutex_give(g_sdn_host.mutex);

	return ret;
}

uint32_t sdn_host_enable(uint8_t protocol)
{
	uint32_t ret = SDN_INTF_ERR_OK;

	if (protocol >= SDN_INTF_PROTOCOL_MAX) {
		return SDN_INTF_ERR_INVALID_PARAM;
	}

	rtos_mutex_take(g_sdn_host.mutex, MUTEX_WAIT_TIMEOUT);
	if (!(g_sdn_host.protocols & BIT(protocol))) {
		if (!g_sdn_host.protocols) {
			sdn_enable();
		}

		if (sdn_bt_addr_fixed && (protocol == SDN_INTF_BT)) {
			sdn_fix_bt_addr(sdn_bt_addr);
		}
		sdn_add_protocol(protocol);
		g_sdn_host.protocols |= BIT(protocol);
	}
	rtos_mutex_give(g_sdn_host.mutex);

	return ret;
}

uint32_t sdn_host_send(uint8_t protocol, uint8_t type, uint8_t *pdata, uint16_t len)
{
	uint32_t ret = SDN_INTF_ERR_TX_DATA_FAIL;

	if (protocol >= SDN_INTF_PROTOCOL_MAX || pdata == NULL || len == 0) {
		return SDN_INTF_ERR_INVALID_PARAM;
	}

	rtos_mutex_take(g_sdn_host.mutex, MUTEX_WAIT_TIMEOUT);
	if (g_sdn_host.protocols & BIT(protocol)) {
		ret = sdn_h2c(protocol, type, pdata, len);
	}
	rtos_mutex_give(g_sdn_host.mutex);

	return ret;
}

void sdn_host_disable(uint8_t protocol)
{
	if (protocol >= SDN_INTF_PROTOCOL_MAX) {
		return;
	}

	rtos_mutex_take(g_sdn_host.mutex, MUTEX_WAIT_TIMEOUT);
	if (g_sdn_host.protocols & BIT(protocol)) {
		sdn_remove_protocol(protocol);
		g_sdn_host.protocols &= (~BIT(protocol));
		if (!g_sdn_host.protocols) {
			sdn_disable();
		}
	}
	rtos_mutex_give(g_sdn_host.mutex);
}

#ifdef CONFIG_MP_INCLUDED
void sdn_host_set_mp(bool is_mp)
{
	rtos_mutex_take(g_sdn_host.mutex, MUTEX_WAIT_TIMEOUT);
	sdn_set_mp(is_mp);
	rtos_mutex_give(g_sdn_host.mutex);
}

void sdn_host_bridge_open(bool to_loguart)
{
	rtos_mutex_take(g_sdn_host.mutex, MUTEX_WAIT_TIMEOUT);
	sdn_bridge_open(to_loguart);
	rtos_mutex_give(g_sdn_host.mutex);
}

void sdn_host_bridge_close(void)
{
	rtos_mutex_take(g_sdn_host.mutex, MUTEX_WAIT_TIMEOUT);
	sdn_bridge_close();
	rtos_mutex_give(g_sdn_host.mutex);
}
#endif

void sdn_host_set_bt_cb(SDN_C2H_CB cb)
{
	bt_c2h_cb  = cb;
}

void sdn_host_fix_bt_addr(uint8_t *addr)
{
	memcpy(sdn_bt_addr, addr, 6);
	sdn_bt_addr_fixed = true;
}

#if defined(CONFIG_BT_INIC) && CONFIG_BT_INIC
extern void bt_inic_send_to_host(uint8_t type, uint8_t *pdata, uint32_t len);
#endif
void sdn_c2h(struct sdn_data_buf *pdata_buf)//uint8_t protocol, uint8_t type, uint8_t *pdata, uint16_t len)
{
#if defined(CONFIG_BT_INIC) && CONFIG_BT_INIC
	bt_inic_send_to_host(pdata_buf->pmsg->msg_type, pdata_buf->pmsg->data, pdata_buf->len - sizeof(struct sdn_intf_data_msg));
#else
	switch (pdata_buf->pmsg->protocol) {
#ifdef CONFIG_BT_SDN
	case SDN_INTF_BT:
		bt_c2h_cb(pdata_buf->pmsg->type, pdata_buf->pmsg->data, pdata_buf->len - sizeof(struct sdn_intf_data_msg));
		break;
#endif

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	case SDN_INTF_154:
		rtk_wpan_vhdlc_receive(pdata_buf->pmsg->data, pdata_buf->len - sizeof(struct sdn_intf_data_msg));
		break;
#endif

	default:
		break;
	}
#endif
}