/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <string.h>
#include <stdio.h>
#include <osif.h>
#include <zephyr/bluetooth/conn.h>
#include <settings_nvs.h>
#include <zephyr/init.h>
#include <hci_core.h>
#include <zephyr/kernel.h>

/*
 * Here collect global variables of zephyr stack into arrays.
 * Mainly in order to make STRUCT_SECTION_FOREACH() work correctly.
 *
 * Moreover, following variables need be initialized:
 * SYS_INIT, K_MEM_SLAB_DEFINE, K_SEM_DEFINE, K_MUTEX_DEFINE.
 */

/*---------- SYS_INIT ----------------------------------------*/
extern struct init_entry Z_INIT_ENTRY_NAME(long_wq_init);
STRUCT_ARRAY_DECLARE(init_entry) = {
	&(Z_INIT_ENTRY_NAME(long_wq_init)),
};
STRUCT_ARRAY_SIZE_DECLARE(init_entry) = sizeof(STRUCT_ARRAY(init_entry)) / sizeof(struct init_entry *);

/*---------- K_MEM_SLAB_DEFINE -------------------------------*/
/*---------- K_MEM_SLAB_DEFINE_STATIC ------------------------*/
extern struct k_mem_slab att_slab;
extern struct k_mem_slab chan_slab;
extern struct k_mem_slab req_slab;
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
extern struct k_mem_slab loopback_buf_pool;
extern struct k_mem_slab segs;
extern struct k_mem_slab local_adv_pool;
#if defined(CONFIG_BT_MESH_RELAY_BUF_COUNT)
extern struct k_mem_slab relay_adv_pool;
#endif
#if defined(CONFIG_BT_MESH_ADV_EXT_FRIEND_SEPARATE)
extern struct k_mem_slab friend_adv_pool;
#endif
#endif
STRUCT_ARRAY_DECLARE(k_mem_slab) = {
	&att_slab,
	&chan_slab,
	&req_slab,
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	&loopback_buf_pool,
	&segs,
	&local_adv_pool,
#if defined(CONFIG_BT_MESH_RELAY_BUF_COUNT)
	&relay_adv_pool,
#endif
#if defined(CONFIG_BT_MESH_ADV_EXT_FRIEND_SEPARATE)
	&friend_adv_pool,
#endif
#endif
};
STRUCT_ARRAY_SIZE_DECLARE(k_mem_slab) = sizeof(STRUCT_ARRAY(k_mem_slab)) / sizeof(struct k_mem_slab *);

/*---------- K_SEM_DEFINE ------------------------------------*/
extern struct k_sem sc_local_pkey_ready;
STRUCT_ARRAY_DECLARE(k_sem) = {
	&sc_local_pkey_ready,
};
STRUCT_ARRAY_SIZE_DECLARE(k_sem) = sizeof(STRUCT_ARRAY(k_sem)) / sizeof(struct k_sem *);

/*---------- K_MUTEX_DEFINE ------------------------------------*/
extern struct k_mutex settings_lock;
#if defined(CONFIG_BT_CLASSIC)
extern struct k_mutex a2dp_mutex;
extern struct k_mutex avdtp_mutex;
#endif
STRUCT_ARRAY_DECLARE(k_mutex) = {
	&settings_lock,
#if defined(CONFIG_BT_CLASSIC)
	&a2dp_mutex,
	&avdtp_mutex,
#endif
};
STRUCT_ARRAY_SIZE_DECLARE(k_mutex) = sizeof(STRUCT_ARRAY(k_mutex)) / sizeof(struct k_mutex *);

/*---------- BT_CONN_CB_DEFINE -------------------------------*/
#if defined(CONFIG_BT_EATT_AUTO_CONNECT)
extern struct bt_conn_cb bt_conn_cb_conn_callback_att;
#endif
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
extern struct bt_conn_cb bt_conn_cb_conn_callbacks_gatt_cli;
extern struct bt_conn_cb bt_conn_cb_conn_callbacks_pb_gatt_srv;
#if defined(CONFIG_BT_MESH_GATT_PROXY) && CONFIG_BT_MESH_GATT_PROXY
extern struct bt_conn_cb bt_conn_cb_conn_callbacks_proxy_srv;
#endif
#endif
STRUCT_ARRAY_DECLARE(bt_conn_cb) = {
#if defined(CONFIG_BT_EATT_AUTO_CONNECT)
	&bt_conn_cb_conn_callback_att,
#endif
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	&bt_conn_cb_conn_callbacks_gatt_cli,
	&bt_conn_cb_conn_callbacks_pb_gatt_srv,
#if defined(CONFIG_BT_MESH_GATT_PROXY) && CONFIG_BT_MESH_GATT_PROXY
	&bt_conn_cb_conn_callbacks_proxy_srv,
#endif
#endif
};
STRUCT_ARRAY_SIZE_DECLARE(bt_conn_cb) = sizeof(_bt_conn_cb_list_array) / sizeof(struct bt_conn_cb *);

/*---------- BT_L2CAP_CHANNEL_DEFINE -------------------------*/
extern struct bt_l2cap_fixed_chan z_att_fixed_chan;
extern struct bt_l2cap_fixed_chan le_fixed_chan;
extern struct bt_l2cap_fixed_chan smp_fixed_chan;
#if defined(CONFIG_BT_CLASSIC)
extern struct bt_l2cap_fixed_chan smp_br_fixed_chan;
#endif
STRUCT_ARRAY_DECLARE(bt_l2cap_fixed_chan) = {
	&z_att_fixed_chan,
	&le_fixed_chan,
	&smp_fixed_chan,
#if defined(CONFIG_BT_CLASSIC)
	&smp_br_fixed_chan,
#endif
};
STRUCT_ARRAY_SIZE_DECLARE(bt_l2cap_fixed_chan) = sizeof(STRUCT_ARRAY(bt_l2cap_fixed_chan)) / sizeof(struct bt_l2cap_fixed_chan *);

/*---------- BT_L2CAP_BR_CHANNEL_DEFINE ----------------------*/
#if defined(CONFIG_BT_CLASSIC)
extern struct bt_l2cap_br_fixed_chan br_fixed_chan;
STRUCT_ARRAY_DECLARE(bt_l2cap_br_fixed_chan) = {
	&br_fixed_chan,
};
STRUCT_ARRAY_SIZE_DECLARE(bt_l2cap_br_fixed_chan) = sizeof(STRUCT_ARRAY(bt_l2cap_br_fixed_chan)) / sizeof(struct bt_l2cap_br_fixed_chan *);
#endif

/*---------- BT_GATT_SERVICE_DEFINE --------------------------*/
extern struct bt_gatt_service_static _1_gatt_svc;
extern struct bt_gatt_service_static _2_gap_svc;
STRUCT_ARRAY_DECLARE(bt_gatt_service_static) = {
	&_1_gatt_svc,
	&_2_gap_svc,
};
STRUCT_ARRAY_SIZE_DECLARE(bt_gatt_service_static) = sizeof(STRUCT_ARRAY(bt_gatt_service_static)) / sizeof(struct bt_gatt_service_static *);

/*---------- SETTINGS_STATIC_HANDLER_DEFINE ------------------*/
#if defined(CONFIG_BT_CLASSIC)
extern struct settings_handler_static settings_handler_bt_link_key;
#endif
extern struct settings_handler_static settings_handler_bt_ccc;
extern struct settings_handler_static settings_handler_bt_sc;
#if defined(CONFIG_BT_GATT_CACHING)
extern struct settings_handler_static settings_handler_bt_cf;
extern struct settings_handler_static settings_handler_bt_hash;
#endif
extern struct settings_handler_static settings_handler_bt_keys;
extern struct settings_handler_static settings_handler_bt;
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
extern struct settings_handler_static settings_handler_bt_mesh;
extern struct settings_handler_static settings_handler_bt_mesh_sig_mod;
extern struct settings_handler_static settings_handler_bt_mesh_vnd_mod;
extern struct settings_handler_static settings_handler_bt_mesh_comp;
#if defined(CONFIG_BT_MESH_LARGE_COMP_DATA_SRV)
extern struct settings_handler_static settings_handler_bt_mesh_metadata;
#endif
extern struct settings_handler_static settings_handler_bt_mesh_app;
extern struct settings_handler_static settings_handler_bt_mesh_cdb;
extern struct settings_handler_static settings_handler_bt_mesh_cfg;
extern struct settings_handler_static settings_handler_bt_mesh_pub;
extern struct settings_handler_static settings_handler_bt_mesh_net;
extern struct settings_handler_static settings_handler_bt_mesh_iv;
extern struct settings_handler_static settings_handler_bt_mesh_seq;
#if defined(CONFIG_BT_MESH_RPR_SRV)
extern struct settings_handler_static settings_handler_bt_mesh_dev_key;
#endif
extern struct settings_handler_static settings_handler_bt_mesh_rpl;
#if defined(CONFIG_BT_MESH_PROXY_SOLICITATION) && CONFIG_BT_MESH_PROXY_SOLICITATION
extern struct settings_handler_static settings_handler_bt_mesh_sseq;
#endif
#if defined(CONFIG_BT_MESH_OD_PRIV_PROXY_SRV) && CONFIG_BT_MESH_OD_PRIV_PROXY_SRV
extern struct settings_handler_static settings_handler_bt_mesh_srpl;
#endif
extern struct settings_handler_static settings_handler_bt_mesh_subnet;
#if CONFIG_BT_MESH_LABEL_COUNT > 0
extern struct settings_handler_static settings_handler_bt_mesh_va;
#endif
#endif
STRUCT_ARRAY_DECLARE(settings_handler_static) = {
#if defined(CONFIG_BT_CLASSIC)
	&settings_handler_bt_link_key,
#endif
	&settings_handler_bt_ccc,
	&settings_handler_bt_sc,
#if defined(CONFIG_BT_GATT_CACHING)
	&settings_handler_bt_cf,
	&settings_handler_bt_hash,
#endif
	&settings_handler_bt_keys,
	&settings_handler_bt,
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	&settings_handler_bt_mesh,
	&settings_handler_bt_mesh_sig_mod,
	&settings_handler_bt_mesh_vnd_mod,
	&settings_handler_bt_mesh_comp,
#if defined(CONFIG_BT_MESH_LARGE_COMP_DATA_SRV)
	&settings_handler_bt_mesh_metadata,
#endif
	&settings_handler_bt_mesh_app,
	&settings_handler_bt_mesh_cdb,
	&settings_handler_bt_mesh_cfg,
	&settings_handler_bt_mesh_pub,
	&settings_handler_bt_mesh_net,
	&settings_handler_bt_mesh_iv,
	&settings_handler_bt_mesh_seq,
#if defined(CONFIG_BT_MESH_RPR_SRV)
	&settings_handler_bt_mesh_dev_key,
#endif
	&settings_handler_bt_mesh_rpl,
#if defined(CONFIG_BT_MESH_PROXY_SOLICITATION) && CONFIG_BT_MESH_PROXY_SOLICITATION
	&settings_handler_bt_mesh_sseq,
#endif
#if defined(CONFIG_BT_MESH_OD_PRIV_PROXY_SRV) && CONFIG_BT_MESH_OD_PRIV_PROXY_SRV
	&settings_handler_bt_mesh_srpl,
#endif
	&settings_handler_bt_mesh_subnet,
#if CONFIG_BT_MESH_LABEL_COUNT > 0
	&settings_handler_bt_mesh_va,
#endif
#endif
};
STRUCT_ARRAY_SIZE_DECLARE(settings_handler_static) = sizeof(STRUCT_ARRAY(settings_handler_static)) / sizeof(struct settings_handler_static *);

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
/*---------- BT_MESH_HB_CB_DEFINE ----------------------------*/
STRUCT_ARRAY_DECLARE(bt_mesh_hb_cb) = {
};
STRUCT_ARRAY_SIZE_DECLARE(bt_mesh_hb_cb) = sizeof(STRUCT_ARRAY(bt_mesh_hb_cb)) / sizeof(struct bt_mesh_hb_cb *);

/*---------- BT_MESH_APP_KEY_CB_DEFINE ----------------------------*/
extern struct bt_mesh_app_key_cb bt_mesh_app_key_cb_app_key_evt;
STRUCT_ARRAY_DECLARE(bt_mesh_app_key_cb) = {
	&bt_mesh_app_key_cb_app_key_evt,
};
STRUCT_ARRAY_SIZE_DECLARE(bt_mesh_app_key_cb) = sizeof(STRUCT_ARRAY(bt_mesh_app_key_cb)) / sizeof(struct bt_mesh_app_key_cb *);

/*---------- BT_MESH_SUBNET_CB_DEFINE ----------------------------*/
extern struct bt_mesh_subnet_cb bt_mesh_subnet_cb_app_keys;
extern struct bt_mesh_subnet_cb bt_mesh_subnet_cb_beacon;
extern struct bt_mesh_subnet_cb bt_mesh_subnet_cb_friend;
#if defined(CONFIG_BT_MESH_GATT_PROXY) && CONFIG_BT_MESH_GATT_PROXY
extern struct bt_mesh_subnet_cb bt_mesh_subnet_cb_gatt_services;
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
extern struct bt_mesh_subnet_cb bt_mesh_subnet_cb_lpn;
#endif
extern struct bt_mesh_subnet_cb bt_mesh_subnet_cb_proxy_cli;
extern struct bt_mesh_subnet_cb bt_mesh_subnet_cb_rpr_srv;
STRUCT_ARRAY_DECLARE(bt_mesh_subnet_cb) = {
	&bt_mesh_subnet_cb_app_keys,
	&bt_mesh_subnet_cb_beacon,
	&bt_mesh_subnet_cb_friend,
#if defined(CONFIG_BT_MESH_GATT_PROXY) && CONFIG_BT_MESH_GATT_PROXY
	&bt_mesh_subnet_cb_gatt_services,
#endif
#if defined(RTK_BLE_MESH_DEVICE_SUPPORT) && RTK_BLE_MESH_DEVICE_SUPPORT
	&bt_mesh_subnet_cb_lpn,
#endif
	&bt_mesh_subnet_cb_proxy_cli,
	// &bt_mesh_subnet_cb_rpr_srv,  // Delete mask this veriable when compile rpr_srv.c
};
STRUCT_ARRAY_SIZE_DECLARE(bt_mesh_subnet_cb) = sizeof(STRUCT_ARRAY(bt_mesh_subnet_cb)) / sizeof(struct bt_mesh_subnet_cb *);

/*---------- BT_MESH_PROXY_CB_DEFINE ----------------------------*/
STRUCT_ARRAY_DECLARE(bt_mesh_proxy_cb) = {

};
STRUCT_ARRAY_SIZE_DECLARE(bt_mesh_proxy_cb) = sizeof(STRUCT_ARRAY(bt_mesh_proxy_cb)) / sizeof(struct bt_mesh_proxy_cb *);

/*---------- BT_MESH_LPN_CB_DEFINE ----------------------------*/
extern struct bt_mesh_lpn_cb bt_mesh_lpn_cb_lpn_cb;
STRUCT_ARRAY_DECLARE(bt_mesh_lpn_cb) = {
	&bt_mesh_lpn_cb_lpn_cb,
};
STRUCT_ARRAY_SIZE_DECLARE(bt_mesh_lpn_cb) = sizeof(STRUCT_ARRAY(bt_mesh_lpn_cb)) / sizeof(struct bt_mesh_lpn_cb *);

/*---------- BT_MESH_FRIEND_CB_DEFINE ----------------------------*/
extern struct bt_mesh_friend_cb bt_mesh_friend_cb_friend;
STRUCT_ARRAY_DECLARE(bt_mesh_friend_cb) = {
	&bt_mesh_friend_cb_friend,
};
STRUCT_ARRAY_SIZE_DECLARE(bt_mesh_friend_cb) = sizeof(STRUCT_ARRAY(bt_mesh_friend_cb)) / sizeof(struct bt_mesh_friend_cb *);
#endif  // RTK_BLE_MESH_SUPPORT
/*------------------------------------------------------------*/

/* for hci_core.c bt_hci_cmd_send_sync() */
struct k_sem z_cmd_tx_sems[CONFIG_BT_BUF_CMD_TX_COUNT];

void zephyr_static_sem_init(void)
{
	int i = 0;
	for (i = 0; i < CONFIG_BT_BUF_CMD_TX_COUNT; i++) {
		k_sem_init(&z_cmd_tx_sems[i], 0, 1);
	}

	poll_static_sem_init();
}

void zephyr_static_sem_deinit(void)
{
	int i = 0;
	for (i = 0; i < CONFIG_BT_BUF_CMD_TX_COUNT; i++) {
		k_sem_deinit(&z_cmd_tx_sems[i]);
	}

	poll_static_sem_deinit();
}

extern void zephyr_net_buf_pool_init(void);

/* Handle all resources those are only defined but not created yet in zephyr stack. */
int zephyr_res_alloc(void)
{
	int ret;

	k_work_sys_work_queue_start();

	STRUCT_SECTION_FOREACH(k_mem_slab, _slab) {
		ret = k_mem_slab_init(_slab);
		if (ret < 0) {
			return ret;
		}
	}

	STRUCT_SECTION_FOREACH(k_sem, _sem) {
		ret = k_sem_init(_sem, _sem->count,  _sem->limit);
		if (ret < 0) {
			return ret;
		}
	}

	STRUCT_SECTION_FOREACH(k_mutex, rtos_mutex_t) {
		ret = k_mutex_init(rtos_mutex_t);
		if (ret < 0) {
			return ret;
		}
	}

	STRUCT_SECTION_FOREACH(init_entry, _entry) {
		ret = _entry->init(_entry->dev);
		if (ret < 0) {
			return ret;
		}
	}

	zephyr_static_sem_init();
	zephyr_net_buf_pool_init();

	return 0;
}

extern struct k_work_q bt_long_wq;
/* Free all resources created in zephyr_res_alloc(). */
int zephyr_res_free(void)
{
	zephyr_static_sem_deinit();

	k_work_sys_work_queue_delete();

	STRUCT_SECTION_FOREACH(k_sem, _sem) {
		k_sem_deinit(_sem);
	}

	STRUCT_SECTION_FOREACH(k_mutex, rtos_mutex_t) {
		k_mutex_deinit(rtos_mutex_t);
	}

	k_work_queue_delete(&bt_long_wq);
	return 0;
}

extern struct bt_dev bt_dev;
extern struct settings_nvs default_settings_nvs;
extern bool settings_subsys_initialized;
#if defined(CONFIG_BT_RECV_WORKQ_BT)
extern struct k_work_q bt_workq;
#endif

static void _zephyr_settings_deinit(void)
{
	if (IS_ENABLED(CONFIG_BT_SETTINGS)) {
		// k_mutex_deinit(&default_settings_nvs.cf_nvs.nvs_lock);
		settings_subsys_initialized = false;
	}
}

/* Free all resources created inside zephyr stack, since zephyr doesn't free workq, thread, fifo, lifo, mutex and semaphore. */
void zephyr_internal_res_free(void)
{
	/* Global Semaphores created by k_sem_init */
	/* temp ignore semaphores in rfcomm.c dfu_cli.c */
	k_sem_deinit(&bt_dev.ncmd_sem);
#if defined(CONFIG_BT_CONN)
	k_sem_deinit(&bt_dev.le.acl_pkts);
#endif
#if defined(CONFIG_BT_ISO)
	k_sem_deinit(&bt_dev.le.iso_pkts);
#endif
#if (defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT) && defined(CONFIG_BT_CLASSIC)
	k_sem_deinit(&bt_dev.br.pkts);
#endif

	/* Global Mutexes created by k_mutex_init */
	_zephyr_settings_deinit();

	/* Global struct k_work_q created by k_work_queue_start */
#if defined(CONFIG_BT_RECV_WORKQ_BT)
	osif_msg_queue_delete(bt_workq.queue); /* thread is abort in bt_disable() */
#endif
}