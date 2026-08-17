#include <sdn_host.h>
#include <rtk_status.h>
#include <os_wrapper.h>
#include "rom/log.h"


#ifndef BIT
#define BIT(x)              (1UL << (x))
#endif

void *sdn_host_mtx = NULL;
uint8_t sdn_host_protos = 0;

extern int rtk_wpan_vhdlc_receive(uint8_t *buf, uint32_t length);
SDN_C2H_CB bt_c2h_cb = NULL;
uint8_t sdn_bt_addr[6] = {0};
uint8_t sdn_bt_addr_fixed = false;

void sdn_host_init(void)
{
	if (RTK_SUCCESS != rtos_mutex_create(&sdn_host_mtx)) {
		return;
	}
}

bool sdn_host_is_enabled(void)
{
	bool ret;

	rtos_mutex_take(sdn_host_mtx, MUTEX_WAIT_TIMEOUT);
	ret = sdn_host_protos ? true : false;
	rtos_mutex_give(sdn_host_mtx);

	return ret;
}

uint32_t sdn_host_enable(uint8_t protocol)
{
	uint32_t ret = SDN_INTF_ERR_OK;

	if (protocol >= SDN_INTF_PROTOCOL_MAX) {
		return SDN_INTF_ERR_INVALID_PARAM;
	}

	rtos_mutex_take(sdn_host_mtx, MUTEX_WAIT_TIMEOUT);
	if (!(sdn_host_protos & BIT(protocol))) {
		if (!sdn_host_protos) {
			sdn_enable();
		}

		if (sdn_bt_addr_fixed && (protocol == SDN_INTF_BT)) {
			sdn_fix_bt_addr(sdn_bt_addr);
		}
		sdn_add_protocol(protocol);
		sdn_host_protos |= BIT(protocol);
	}
	rtos_mutex_give(sdn_host_mtx);

	return ret;
}

uint32_t sdn_host_send(uint8_t protocol, uint8_t type, uint8_t *pdata, uint16_t len)
{
	uint32_t ret = SDN_INTF_ERR_TX_DATA_FAIL;

	if (protocol >= SDN_INTF_PROTOCOL_MAX || pdata == NULL || len == 0) {
		return SDN_INTF_ERR_INVALID_PARAM;
	}

	rtos_mutex_take(sdn_host_mtx, MUTEX_WAIT_TIMEOUT);
	if (sdn_host_protos & BIT(protocol)) {
		ret = sdn_h2c(protocol, type, pdata, len);
	}
	rtos_mutex_give(sdn_host_mtx);

	return ret;
}

void sdn_host_disable(uint8_t protocol)
{
	if (protocol >= SDN_INTF_PROTOCOL_MAX) {
		return;
	}

	rtos_mutex_take(sdn_host_mtx, MUTEX_WAIT_TIMEOUT);
	if (sdn_host_protos & BIT(protocol)) {
		sdn_remove_protocol(protocol);
		sdn_host_protos &= (~BIT(protocol));
		if (!sdn_host_protos) {
			sdn_disable();
		}
	}
	rtos_mutex_give(sdn_host_mtx);
}

#ifdef CONFIG_MP_INCLUDED
void sdn_host_set_mp(bool is_mp)
{
	rtos_mutex_take(sdn_host_mtx, MUTEX_WAIT_TIMEOUT);
	sdn_set_mp(is_mp);
	rtos_mutex_give(sdn_host_mtx);
}

void sdn_host_bridge_open(bool to_loguart)
{
	rtos_mutex_take(sdn_host_mtx, MUTEX_WAIT_TIMEOUT);
	sdn_bridge_open(to_loguart);
	rtos_mutex_give(sdn_host_mtx);
}

void sdn_host_bridge_close(void)
{
	rtos_mutex_take(sdn_host_mtx, MUTEX_WAIT_TIMEOUT);
	sdn_bridge_close();
	rtos_mutex_give(sdn_host_mtx);
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
void sdn_c2h(struct sdn_data_buf *pdata_buf)
{
#if defined(CONFIG_BT_INIC) && CONFIG_BT_INIC
	bt_inic_send_to_host(pdata_buf->msg_type, pdata_buf->data, pdata_buf->len);
#else
	switch (pdata_buf->protocol) {
#ifdef CONFIG_BT_SDN
	case SDN_INTF_BT:
		bt_c2h_cb(pdata_buf->type, pdata_buf->data, pdata_buf->len);
		break;
#endif

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	case SDN_INTF_154:
		rtk_wpan_vhdlc_receive(pdata_buf->data, pdata_buf->len);
		break;
#endif

	default:
		break;
	}
#endif
}
