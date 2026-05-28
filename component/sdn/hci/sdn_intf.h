#ifndef SDN_INTF_H
#define SDN_INTF_H

#include <basic_types.h>
#include <dlist.h>

#define SDN_INTF_ERR_OK                     0
#define SDN_INTF_ERR_INIT_FAIL              1
#define SDN_INTF_ERR_OPEN_FAIL              2
#define SDN_INTF_ERR_TX_CTRL_FAIL           3
#define SDN_INTF_ERR_TX_DATA_FAIL           4
#define SDN_INTF_ERR_INVALID_PARAM          5

enum sdn_protocol_type {
	SDN_INTF_BT = 0,	/* KEEP it is 0 */
	SDN_INTF_154,
	SDN_INTF_COEX,
	SDN_INTF_CTRL,
	SDN_INTF_PROTOCOL_MAX,
};

enum sdn_intf_status {
	SDN_INTF_STATE_NONE,
	SDN_INTF_STATE_INIT,
	SDN_INTF_STATE_OPEN,
};

enum sdn_intf_ctrl_op {
	SDN_INTF_CTRL_INTF_OPEN,
	SDN_INTF_CTRL_INTF_CLOSE,
	SDN_INTF_CTRL_PROTO_ADD,
	SDN_INTF_CTRL_PROTO_REMOVE,
	SDN_INTF_CTRL_MP,
	SDN_INTF_CTRL_BRIDGE_OPEN,
	SDN_INTF_CTRL_BRIDGE_CLOSE,
	SDN_INTF_CTRL_FIX_ADDR,
};

#define SDN_MSG_PROTOCOL(msgtype)		(((msgtype) >> 5) & 0x7)
#define SDN_MSG_TYPE(msgtype)			((msgtype) & 0x1F)

struct sdn_intf_data_msg {
	union {
		struct {
			uint8_t type : 5;
			uint8_t protocol : 3;     /* enum sdn_protocol_type */
		};
		uint8_t msg_type;
	};
	uint8_t data[0];
} __attribute__((packed));

struct sdn_data_buf {
	struct list_head list;
	uint16_t len;		/* actual length of pmsg */
	struct sdn_intf_data_msg *pmsg;
};

struct sdn_intf_task {
	void *task;
	void *sema;
	uint8_t running : 4;
	uint8_t stop    : 4;
};

/* These functions can only be called in sdn_host.c or sdn_client_ipc.c */
bool sdn_enable(void);
uint32_t sdn_h2c(uint8_t protocol, uint8_t type, void *pdata, uint16_t len);
void sdn_disable(void);
void sdn_add_protocol(uint8_t protocol);
void sdn_remove_protocol(uint8_t protocol);
void sdn_set_mp(bool is_mp);
void sdn_bridge_open(bool to_loguart);
void sdn_bridge_close(void);
void sdn_fix_bt_addr(uint8_t *bdaddr);
/*----------------------------------------------------------------------*/

/* These functions can only be called in sdn_host_ipc.c or sdn_client.c */
void sdn_c2h(struct sdn_data_buf *pdata_buf);
/*----------------------------------------------------------------------*/

#endif
