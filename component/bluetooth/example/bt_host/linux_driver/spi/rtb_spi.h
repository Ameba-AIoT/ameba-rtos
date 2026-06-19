#ifndef __RTB_SPI_H__
#define __RTB_SPI_H__

#include "bt_inic_defs.h"

struct bt_inic_spi_hdr {
	u32 event;
	u32 len : 24;
	u8 type;
} __attribute__((packed));

struct btspi_data {
	struct hci_dev *hdev;
	struct device *dev;

	struct sk_buff_head txq;
	struct sk_buff_head rxq;
	struct task_struct *thread;
	struct semaphore trx_sema;
	struct semaphore vendor_sema;
	u8 vendor_status;
	struct spinlock lock;
};

#define BT_OP(ogf, ocf)                         ((ocf) | ((ogf) << 10))
#define BT_OGF_VS                               0x3F

/********* vendor specific command: [15:10]=0x3F *********/
#define BT_HCI_CMD_VS_BT_ON                     BT_OP(BT_OGF_VS, 0x0201)
#define BT_HCI_CMD_VS_BT_OFF                    BT_OP(BT_OGF_VS, 0x0202)
#define BT_HCI_CMD_VS_BT_HOST_SLEEP             BT_OP(BT_OGF_VS, 0x0203)
#define BT_HCI_CMD_VS_BT_HOST_RESUME            BT_OP(BT_OGF_VS, 0x0204)

void bt_recv_pkts(struct sk_buff *skb);
int bt_dev_probe(struct device *pdev);
void bt_dev_remove(struct device *pdev);

#endif /* __RTB_SPI_H__ */
