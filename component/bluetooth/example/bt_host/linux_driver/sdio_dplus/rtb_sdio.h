/* BT ID base should bigger than WHC_WIFI_EVT_MAX. */
struct btsdio_data {
	struct hci_dev   *hdev;
	struct sdio_func *func;

	struct work_struct work;
	struct sk_buff_head txq;
};

struct btsdio_priv {
	int int_count;
	struct task_struct *thread;
	wait_queue_head_t wq;

	spinlock_t lock;

};

#define VERSION "0.12.40fa29d.20230313-112552"
#define BTSDIO_DMA_ALIGN                    8
#define ALIGN_UP(a, size)                   ((a+size-1) & (~(size-1)))

#define BT_OP(ogf, ocf)                     ((ocf) | ((ogf) << 10))
#define BT_OGF(opcode)                      (((opcode) >> 10) & 0x3F)
#define BT_OCF(opcode)                      ((opcode) & 0x3FF)
#define BT_OGF_VS                           0x3F

#define BT_HCI_CMD_VS_BT_ON                 BT_OP(BT_OGF_VS, 0x0201)
#define BT_HCI_CMD_VS_BT_OFF                BT_OP(BT_OGF_VS, 0x0202)
#define BT_HCI_CMD_VS_BT_HOST_SLEEP         BT_OP(BT_OGF_VS, 0x0203)
#define BT_HCI_CMD_VS_BT_HOST_RESUME        BT_OP(BT_OGF_VS, 0x0204)

#define BT_SDIO_DPLUS
#define SDIO_RX_DESC_LEN                    16
#define HCI_SDIO_PKT_HDR_LEN                7

#define REG_RDAT                            0xE000      /* Receiver Data FIFO */
#define REG_TDAT                            0x8000      /* Transmitter Data FIFO */
#define REG_HISR                            0x18        /* Host Interrupt Status Register */
#define HCI_HISR_RX_REQUEST                 BIT(0)      /* HISR BIT(0) : RX_REQUEST INTRRUPT */
#define REG_FREE_TXBD_NUM                   0x20        /* FREE_TXBD_NUM */
#define REG_BT_CTRL                         0x40        /* BT Control register */
#define REG_CPU_IND                         0x87        /* CPU Indication */

/** @defgroup INIC_DESC_Size_definitions
  * @{
  */
#define SIZE_RX_DESC    (sizeof(INIC_RX_DESC))
#define SIZE_TX_DESC    (sizeof(INIC_TX_DESC))

int bt_recv_pkts(struct sk_buff *skb);
int bt_dev_probe(struct device *pdev);
void bt_dev_remove(struct device *pdev);

#ifndef CONFIG_BT_INIC
#define WHC_BT_ID_BASE 0xa5a5a5b0
enum INIC_BT_ID_TYPE {
	INIC_BT_HOST_TX = WHC_BT_ID_BASE,
	INIC_BT_HOST_RX,

	INIC_BT_ID_MAX
};

/**
  * @brief  INIC TX DESC structure definition
  */
typedef struct {
	/* u4Byte 0 */
	u32 txpktsize: 16;      // bit[15:0]
	u32 offset: 8;              // bit[23:16], store the sizeof(INIC_TX_DESC)
	u32 bus_agg_num: 8;     // bit[31:24], the bus aggregation number

	/* u4Byte 1 */
	u32 type: 8;                // bit[7:0], the packet type
	u32 data: 8;                // bit[8:15], the value to be written to the memory
	u32 reply: 1;               // bit[16], request to send a reply message
	u32 rsvd0: 15;

	/* u4Byte 2 */
	u32 start_addr;         // 1) memory write/read start address 2) RAM start_function address

	/* u4Byte 3 */
	u32 data_len: 16;           // bit[15:0], the length to write/read
	u32 rsvd2: 16;          // bit[31:16]
} INIC_TX_DESC, *PINIC_TX_DESC;

/**
  * @brief  INIC RX DESC strucbit[31:24], the bus aggregation number
 **/
typedef struct {
	/* u4Byte 0 */
	u32 pkt_len: 16;            // bit[15:0], the packet size
	u32 offset: 8;          // bit[23:16], the offset from the packet start to the buf start, also means the size of RX Desc
	u32 rsvd0: 6;               // bit[29:24]
	u32 icv: 1;             //icv error
	u32 crc: 1;             // crc error

	/* u4Byte 1 */
	/************************************************/
	/*****************receive packet type*********************/
	/*  0x82:   802.3 packet                              */
	/*  0x80:   802.11 packet                             */
	/*  0x10:   C2H command                       */
	/*  0x50:   Memory Read                           */
	/*  0x52:   Memory Write                              */
	/*  0x54:   Memory Set                            */
	/*  0x60:   Indicate the firmware is started              */
	u32 type: 8;                // bit[7:0], the type of this packet
	u32 rsvd1: 24;          // bit[31:8]

	/* u4Byte 2 */
	u32 start_addr;

	/* u4Byte 3 */
	u32 data_len: 16;           // bit[15:0], the type of this packet
	u32 result: 8;              // bit[23:16], the result of memory write command
	u32 rsvd2: 8;               // bit[31:24]
} INIC_RX_DESC, *PINIC_RX_DESC;

struct whc_api_info {
	u32 event;
	u32 api_id;
};
#endif