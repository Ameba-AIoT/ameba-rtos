#ifndef RTK_SDIO_TYPE_H
#define RTK_SDIO_TYPE_H

#include "sdio_reg.h"
#include "sdio_conf.h"
#include <linux/mmc/sdio_func.h>
#include <linux/version.h>
#include <linux/spinlock.h>
#include <linux/compiler.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>

#include <asm/byteorder.h>
#include <asm/atomic.h>
#include <asm/io.h>

#include <linux/sched/signal.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/vmalloc.h>




#ifdef CONFIG_DEBUG
#define RTK_PRINTF  printk
#else
#define RTK_PRINTF(...)
#endif

#define _func_enter_   //RTK_PRINTF("%s enter=>", __FUNCTION__);
#define _func_exit_    //RTK_PRINTF("%s exit=>", __FUNCTION__);


#define	RTK_SDIO_IOCTL_MAGIC 	0x88
#define RTK_SDIO_DBG			_IOR(RTK_SDIO_IOCTL_MAGIC, 0, u32)
#define RTK_SDIO_LOOPSTART		_IOR(RTK_SDIO_IOCTL_MAGIC, 1, u32)

#define _RND(sz, r) ((((sz)+((r)-1))/(r))*(r))


#define TRUE	1
#define FALSE	0


#define _FAIL		0
#define _SUCCESS	1

#ifndef BIT0
#define BIT0		0x0001
#define BIT1		0x0002
#define BIT2		0x0004
#define BIT3		0x0008
#define BIT4		0x0010
#define BIT5		0x0020
#define BIT6		0x0040
#define BIT7		0x0080
#define BIT8		0x0100
#define BIT9		0x0200
#define BIT10	0x0400
#define BIT11	0x0800
#define BIT12	0x1000
#define BIT13	0x2000
#define BIT14	0x4000
#define BIT15	0x8000
#define BIT16	0x00010000
#define BIT17	0x00020000
#define BIT18	0x00040000
#define BIT19	0x00080000
#define BIT20	0x00100000
#define BIT21	0x00200000
#define BIT22	0x00400000
#define BIT23	0x00800000
#define BIT24	0x01000000
#define BIT25	0x02000000
#define BIT26	0x04000000
#define BIT27	0x08000000
#define BIT28	0x10000000
#define BIT29	0x20000000
#define BIT30	0x40000000
#define BIT31	0x80000000
#endif


#define TRX_BUFFER_SIZE		16384

#define LOOPBACK_SIZE   1500

typedef void		*_thread_hdl_;
typedef int		thread_return;
#define THREAD_RETURN_SUCCESS (0)
typedef void	*thread_context;
typedef thread_return(*thread_func_t)(thread_context context);


typedef struct {
	// u4Byte 0
	u32	txpktsize: 16;      // bit[15:0]
	u32	offset: 8;    		// bit[23:16], store the sizeof(TX_DESC)
	u32	bus_agg_num: 8;		// bit[31:24], the bus aggregation number

	// u4Byte 1
	u32 type: 8;            // bit[7:0], the packet type
	u32 data: 8;            // bit[8:15], the value to be written to the memory
	u32 reply: 1;           // bit[16], request to send a reply message
	u32 rsvd0: 15;

	// u4Byte 2
	u32	start_addr;         // memory write/read start address, function start address

	// u4Byte 3
	u32 data_len: 16;      // bit[15:0], the length to write/read
	u32 rsvd2: 16;          // bit[31:16]
} tx_desc;


typedef struct {
	// u4Byte 0
	u32 pkt_len: 16;		// bit[15:0], the packet size
	u32 offset: 8;			// bit[23:16], the offset from the packet start to the buf start, also means the size of RX Desc
	u32 rsvd0: 6;			// bit[29:24]
	u32 icv: 1; 			//icv error
	u32 crc: 1; 			// crc error

	u32 type: 8;        // bit[7:0], the type of this packet
	u32 rsvd1: 24;      // bit[31:8]

	// u4Byte 2
	u32 start_addr;

	// u4Byte 3
	u32 data_len: 16;		// bit[15:0], the type of this packet
	u32 result: 8;			// bit[23:16], the result of memory write command
	u32 rsvd2: 8;			// bit[31:24]
} rx_desc;



typedef struct {
	s32 surprise_remove;
	s32 stop_trx; //when wifi off/on, to indicate stop/resume packet transmittion

	struct sdio_func *func;
	u16 free_txbd;
	u32 tx_max_size;
	u16 rx_len;
	u32 block_transfer_len;
	u32 txbd_size;

	u32 sdio_hisr;
	u32 sdio_himr;

	u8 *tx_buffer;
	u8 *rx_buffer;

	struct task_struct *LPThread;
} sdio_adapter;


u8 rtw_read8(sdio_adapter *adapter, u32 addr);

u16 rtw_read16(sdio_adapter *adapter, u32 addr);

u32 rtw_read32(sdio_adapter *adapter, u32 addr);

void rtw_write8(sdio_adapter *adapter, u32 addr, u8 val);

void rtw_write16(sdio_adapter *adapter, u32 addr, u16 val);

void rtw_write32(sdio_adapter *adapter, u32 addr, u32 val);

u8 rtw_f0_read8(sdio_adapter *adapter, u32 addr);

void rtw_f0_write8(sdio_adapter *adapter, u32 addr, u8 val);

int rtw_write_txfifo(sdio_adapter *padapter, u32 cnt, u8 *pmem);

int rtw_write_fw(sdio_adapter *padapter, u32 cnt, u8 *pmem);

int rtw_read_rxfifo(sdio_adapter *adapter, u32 cnt, u8 *pmem);

int rtw_register_driver(struct sdio_driver *sdio);

void rtw_unregister_driver(struct sdio_driver *sdio);

int rtw_enable_function(struct sdio_func *func);

int rtw_disable_function(struct sdio_func *func);

int rtw_set_block_size(struct sdio_func *func, u32 blk_sz);

int rtw_register_irq(struct sdio_func *func, void *handler);

int rtw_release_irq(struct sdio_func *func);

void rtw_update_free_txbd(sdio_adapter *adapter);

int rtw_sdio_query_free_txbd(sdio_adapter *adapter, u16 need_num);

u8 *rtw_malloc(u32 size);

void rtw_mfree(void *pbuf);

void rtw_memcpy(void *dst, void *src, u32 sz);

u8 *rtw_zmalloc(u32 size);

int rtw_thread_should_stop(void);

void rtw_thread_exit(void);

int rtw_create_task_ex(struct task_struct **ptask, const char *name, thread_func_t func, void *thctx);

void rtw_delete_task_ex(struct task_struct *task);

u32	rtw_get_current_time(void);

u32 rtw_systime_to_ms(u32 systime);

u32 rtw_ms_to_systime(u32 ms);

u32 rtw_get_passing_time_ms(u32 start);

void rtw_msleep_os(int ms);

void rtw_mdelay_os(int ms);

void rtw_enable_interrupt(sdio_adapter *padapter);

void rtw_disable_interrupt(sdio_adapter *padapter);


void loopback_recv(sdio_adapter *padapter, u32 size);


int mychardev_init(void);
void mychardev_exit(void);


void rtw_sdio_loopback_start(sdio_adapter *padapter);
void rtw_sdio_loopback_stop(sdio_adapter *padapter);
thread_return rtw_sdio_loopback_thread(thread_context context);

#endif
