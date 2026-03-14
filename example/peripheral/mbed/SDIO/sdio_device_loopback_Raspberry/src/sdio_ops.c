#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include "sdio_type.h"

/**
 *	sdio_register_driver - register a function driver
 *	@drv: SDIO function driver
 */
extern int sdio_register_driver(struct sdio_driver *);
/**
 *	sdio_unregister_driver - unregister a function driver
 *	@drv: SDIO function driver
 */
extern void sdio_unregister_driver(struct sdio_driver *);

/*
 * SDIO I/O operations
 */
/**
*	sdio_claim_host - exclusively claim a bus for a certain SDIO function
*	@func: SDIO function that will be accessed
*
*	Claim a bus for a set of operations. The SDIO function given
*	is used to figure out which bus is relevant.
*/
extern void sdio_claim_host(struct sdio_func *func);
/**
 *	sdio_release_host - release a bus for a certain SDIO function
 *	@func: SDIO function that was accessed
 *
 *	Release a bus, allowing others to claim the bus for their
 *	operations.
 */
extern void sdio_release_host(struct sdio_func *func);
/**
 *	sdio_enable_func - enables a SDIO function for usage
 *	@func: SDIO function to enable
 *
 *	Powers up and activates a SDIO function so that register
 *	access is possible.
 */
extern int sdio_enable_func(struct sdio_func *func);
/**
 *	sdio_disable_func - disable a SDIO function
 *	@func: SDIO function to disable
 *
 *	Powers down and deactivates a SDIO function. Register access
 *	to this function will fail until the function is reenabled.
 */
extern int sdio_disable_func(struct sdio_func *func);
/**
 *	sdio_set_block_size - set the block size of an SDIO function
 *	@func: SDIO function to change
 *	@blksz: new block size or 0 to use the default.
 *
 *	The default block size is the largest supported by both the function
 *	and the host, with a maximum of 512 to ensure that arbitrarily sized
 *	data transfer use the optimal (least) number of commands.
 *
 *	A driver may call this to override the default block size set by the
 *	core. This can be used to set a block size greater than the maximum
 *	that reported by the card; it is the driver's responsibility to ensure
 *	it uses a value that the card supports.
 *
 *	Returns 0 on success, -EINVAL if the host does not support the
 *	requested block size, or -EIO (etc.) if one of the resultant FBR block
 *	size register writes failed.
 *
 */
extern int sdio_set_block_size(struct sdio_func *func, unsigned blksz);
/**
 *	sdio_claim_irq - claim the IRQ for a SDIO function
 *	@func: SDIO function
 *	@handler: IRQ handler callback
 *
 *	Claim and activate the IRQ for the given SDIO function. The provided
 *	handler will be called when that IRQ is asserted.  The host is always
 *	claimed already when the handler is called so the handler must not
 *	call sdio_claim_host() nor sdio_release_host().
 */
extern int sdio_claim_irq(struct sdio_func *func, sdio_irq_handler_t *handler);
/**
 *	sdio_release_irq - release the IRQ for a SDIO function
 *	@func: SDIO function
 *
 *	Disable and release the IRQ for the given SDIO function.
 */
extern int sdio_release_irq(struct sdio_func *func);
/**
 *	sdio_align_size - pads a transfer size to a more optimal value
 *	@func: SDIO function
 *	@sz: original transfer size
 *
 *	Pads the original data size with a number of extra bytes in
 *	order to avoid controller bugs and/or performance hits
 *	(e.g. some controllers revert to PIO for certain sizes).
 *
 *	If possible, it will also adjust the size so that it can be
 *	handled in just a single request.
 *
 *	Returns the improved size, which might be unmodified.
 */
extern unsigned int sdio_align_size(struct sdio_func *func, unsigned int sz);
/**
 *	sdio_readb - read a single byte from a SDIO function
 *	@func: SDIO function to access
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *
 *	Reads a single byte from the address space of a given SDIO
 *	function. If there is a problem reading the address, 0xff
 *	is returned and @err_ret will contain the error code.
 */
extern u8 sdio_readb(struct sdio_func *func, unsigned int addr, int *err_ret);
/**
 *	sdio_readw - read a 16 bit integer from a SDIO function
 *	@func: SDIO function to access
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *
 *	Reads a 16 bit integer from the address space of a given SDIO
 *	function. If there is a problem reading the address, 0xffff
 *	is returned and @err_ret will contain the error code.
 */
extern u16 sdio_readw(struct sdio_func *func, unsigned int addr, int *err_ret);
/**
 *	sdio_readl - read a 32 bit integer from a SDIO function
 *	@func: SDIO function to access
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *
 *	Reads a 32 bit integer from the address space of a given SDIO
 *	function. If there is a problem reading the address,
 *	0xffffffff is returned and @err_ret will contain the error
 *	code.
 */
extern u32 sdio_readl(struct sdio_func *func, unsigned int addr, int *err_ret);
/**
 *	sdio_memcpy_fromio - read a chunk of memory from a SDIO function
 *	@func: SDIO function to access
 *	@dst: buffer to store the data
 *	@addr: address to begin reading from
 *	@count: number of bytes to read
 *
 *	Reads from the address space of a given SDIO function. Return
 *	value indicates if the transfer succeeded or not.
 */
extern int sdio_memcpy_fromio(struct sdio_func *func, void *dst,
							  unsigned int addr, int count);
/**
 *	sdio_readsb - read from a FIFO on a SDIO function
 *	@func: SDIO function to access
 *	@dst: buffer to store the data
 *	@addr: address of (single byte) FIFO
 *	@count: number of bytes to read
 *
 *	Reads from the specified FIFO of a given SDIO function. Return
 *	value indicates if the transfer succeeded or not.
 */
extern int sdio_readsb(struct sdio_func *func, void *dst,
					   unsigned int addr, int count);
/**
 *	sdio_writeb - write a single byte to a SDIO function
 *	@func: SDIO function to access
 *	@b: byte to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Writes a single byte to the address space of a given SDIO
 *	function. @err_ret will contain the status of the actual
 *	transfer.
 */
extern void sdio_writeb(struct sdio_func *func, u8 b,
						unsigned int addr, int *err_ret);
/**
 *	sdio_writew - write a 16 bit integer to a SDIO function
 *	@func: SDIO function to access
 *	@b: integer to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Writes a 16 bit integer to the address space of a given SDIO
 *	function. @err_ret will contain the status of the actual
 *	transfer.
 */
extern void sdio_writew(struct sdio_func *func, u16 b,
						unsigned int addr, int *err_ret);
/**
 *	sdio_writel - write a 32 bit integer to a SDIO function
 *	@func: SDIO function to access
 *	@b: integer to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Writes a 32 bit integer to the address space of a given SDIO
 *	function. @err_ret will contain the status of the actual
 *	transfer.
 */
extern void sdio_writel(struct sdio_func *func, u32 b,
						unsigned int addr, int *err_ret);
/**
 *	sdio_writeb_readb - write and read a byte from SDIO function
 *	@func: SDIO function to access
 *	@write_byte: byte to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Performs a RAW (Read after Write) operation as defined by SDIO spec -
 *	single byte is written to address space of a given SDIO function and
 *	response is read back from the same address, both using single request.
 *	If there is a problem with the operation, 0xff is returned and
 *	@err_ret will contain the error code.
 */
extern u8 sdio_writeb_readb(struct sdio_func *func, u8 write_byte,
							unsigned int addr, int *err_ret);
/**
 *	sdio_memcpy_toio - write a chunk of memory to a SDIO function
 *	@func: SDIO function to access
 *	@addr: address to start writing to
 *	@src: buffer that contains the data to write
 *	@count: number of bytes to write
 *
 *	Writes to the address space of a given SDIO function. Return
 *	value indicates if the transfer succeeded or not.
 */
extern int sdio_memcpy_toio(struct sdio_func *func, unsigned int addr,
							void *src, int count);
/**
 *	sdio_writesb - write to a FIFO of a SDIO function
 *	@func: SDIO function to access
 *	@addr: address of (single byte) FIFO
 *	@src: buffer that contains the data to write
 *	@count: number of bytes to write
 *
 *	Writes to the specified FIFO of a given SDIO function. Return
 *	value indicates if the transfer succeeded or not.
 */
extern int sdio_writesb(struct sdio_func *func, unsigned int addr,
						void *src, int count);
/**
 *	sdio_f0_readb - read a single byte from SDIO function 0
 *	@func: an SDIO function of the card
 *	@addr: address to read
 *	@err_ret: optional status value from transfer
 *
 *	Reads a single byte from the address space of SDIO function 0.
 *	If there is a problem reading the address, 0xff is returned
 *	and @err_ret will contain the error code.
 */
extern unsigned char sdio_f0_readb(struct sdio_func *func,
								   unsigned int addr, int *err_ret);
/**
 *	sdio_f0_writeb - write a single byte to SDIO function 0
 *	@func: an SDIO function of the card
 *	@b: byte to write
 *	@addr: address to write to
 *	@err_ret: optional status value from transfer
 *
 *	Writes a single byte to the address space of SDIO function 0.
 *	@err_ret will contain the status of the actual transfer.
 *
 *	Only writes to the vendor specific CCCR registers (0xF0 -
 *	0xFF) are permiited; @err_ret will be set to -EINVAL for *
 *	writes outside this range.
 */
extern void sdio_f0_writeb(struct sdio_func *func, unsigned char b,
						   unsigned int addr, int *err_ret);
/**
 *	sdio_get_host_pm_caps - get host power management capabilities
 *	@func: SDIO function attached to host
 *
 *	Returns a capability bitmask corresponding to power management
 *	features supported by the host controller that the card function
 *	might rely upon during a system suspend.  The host doesn't need
 *	to be claimed, nor the function active, for this information to be
 *	obtained.
 */
extern mmc_pm_flag_t sdio_get_host_pm_caps(struct sdio_func *func);
/**
 *	sdio_set_host_pm_flags - set wanted host power management capabilities
 *	@func: SDIO function attached to host
 *
 *	Set a capability bitmask corresponding to wanted host controller
 *	power management features for the upcoming suspend state.
 *	This must be called, if needed, each time the suspend method of
 *	the function driver is called, and must contain only bits that
 *	were returned by sdio_get_host_pm_caps().
 *	The host doesn't need to be claimed, nor the function active,
 *	for this information to be set.
 */
extern int sdio_set_host_pm_flags(struct sdio_func *func, mmc_pm_flag_t flags);


u8 rtw_read8(sdio_adapter *padapter, u32 addr)
{
	int r_val;
	u8 data;
	_func_enter_;

	sdio_claim_host(padapter->func);
	data = sdio_readb(padapter->func, addr, &r_val);
	sdio_release_host(padapter->func);

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
	return data;
}

u16 rtw_read16(sdio_adapter *padapter, u32 addr)
{
	int r_val;
	u16 data;
	_func_enter_;

	sdio_claim_host(padapter->func);
	if (addr < 0x100) {
		data = sdio_readw(padapter->func, addr, &r_val);
	} else {
		data = (sdio_readb(padapter->func, addr, &r_val) | (sdio_readb(padapter->func, addr + 1, &r_val) << 8));
	}
	sdio_release_host(padapter->func);

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
	return data;
}

u32 rtw_read32(sdio_adapter *padapter, u32 addr)
{
	int r_val;
	u32 data;
	_func_enter_;

	sdio_claim_host(padapter->func);
	if (addr < 0x100) {
		data = sdio_readl(padapter->func, addr, &r_val);
	} else {
		data = (sdio_readb(padapter->func, addr, &r_val) | (sdio_readb(padapter->func, addr + 1, &r_val) << 8) |
				(sdio_readb(padapter->func, addr + 2, &r_val) << 16) | (sdio_readb(padapter->func, addr + 3, &r_val) << 24));
	}
	sdio_release_host(padapter->func);

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
	return data;

}

void rtw_write8(sdio_adapter *padapter, u32 addr, u8 val)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(padapter->func);
	sdio_writeb(padapter->func, val, addr, &r_val);
	sdio_release_host(padapter->func);

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
}
void rtw_write16(sdio_adapter *padapter, u32 addr, u16 val)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(padapter->func);
	sdio_writew(padapter->func, val, addr, &r_val);
	sdio_release_host(padapter->func);

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
}
void rtw_write32(sdio_adapter *padapter, u32 addr, u32 val)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(padapter->func);
	sdio_writel(padapter->func, val, addr, &r_val);
	sdio_release_host(padapter->func);

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
}

u8 rtw_f0_read8(sdio_adapter *padapter, u32 addr)
{
	int r_val;
	u8 data;
	_func_enter_;

	sdio_claim_host(padapter->func);
	data = sdio_f0_readb(padapter->func, addr, &r_val);
	sdio_release_host(padapter->func);

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
	return data;
}

void rtw_f0_write8(sdio_adapter *padapter, u32 addr, u8 val)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(padapter->func);
	sdio_f0_writeb(padapter->func, val, addr, &r_val);
	sdio_release_host(padapter->func);

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
}

int rtw_write_txfifo(sdio_adapter *padapter, u32 cnt, u8 *pmem)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(padapter->func);
	r_val = sdio_memcpy_toio(padapter->func, SDIO_TX_FIFO_ADDR | cnt >> 2, (void *)pmem, cnt);
	sdio_release_host(padapter->func);

	if (padapter->free_txbd > 0) {
		padapter->free_txbd -= 1;
	}

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
	return r_val;
}

int rtw_write_fw(sdio_adapter *padapter, u32 cnt, u8 *pmem)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(padapter->func);
	r_val = sdio_memcpy_toio(padapter->func, SDIO_TX_FW_ADDR | cnt >> 2, (void *)pmem, cnt);
	sdio_release_host(padapter->func);

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
	return r_val;
}


int rtw_read_rxfifo(sdio_adapter *padapter, u32 cnt, u8 *pmem)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(padapter->func);
	r_val = sdio_memcpy_fromio(padapter->func, (void *) pmem, SDIO_RX_FIFO_ADDR, cnt);
	sdio_release_host(padapter->func);

	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
	return r_val;
}


int rtw_register_driver(struct sdio_driver *sdio)
{
	int r_val;
	_func_enter_;

	r_val = sdio_register_driver(sdio);

	_func_exit_;
	return r_val;
}

void rtw_unregister_driver(struct sdio_driver *sdio)
{
	_func_enter_;

	sdio_unregister_driver(sdio);

	_func_exit_;

}

int rtw_enable_function(struct sdio_func *func)
{
	int r_val;
	_func_enter_;
	sdio_claim_host(func);
	r_val = sdio_enable_func(func);
	sdio_release_host(func);
	if (r_val) {
		RTK_PRINTF("%s error: %d\n", __FUNCTION__, r_val);
	}

	_func_exit_;
	return r_val;
}

int rtw_disable_function(struct sdio_func *func)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(func);
	r_val = sdio_disable_func(func);
	sdio_release_host(func);

	_func_exit_;
	return r_val;
}

int rtw_set_block_size(struct sdio_func *func, u32 blk_sz)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(func);
	r_val = sdio_set_block_size(func, blk_sz);
	sdio_release_host(func);

	_func_exit_;
	return r_val;

}

int rtw_register_irq(struct sdio_func *func, void *handler)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(func);
	r_val = sdio_claim_irq(func, handler);
	sdio_release_host(func);

	_func_exit_;
	return r_val;

}

int rtw_release_irq(struct sdio_func *func)
{
	int r_val;
	_func_enter_;

	sdio_claim_host(func);
	r_val = sdio_release_irq(func);
	sdio_release_host(func);

	_func_exit_;
	return r_val;

}


void rtw_update_free_txbd(sdio_adapter *padapter)
{
	u16 free_txbd;

	free_txbd = rtw_read16(padapter, SDIO_REG_FREE_TXBD_NUM);

	padapter->free_txbd = free_txbd;

	//RTK_PRINTF("free txbd: %d", free_txbd);
}

int rtw_sdio_query_free_txbd(sdio_adapter *padapter, u16 need_num)
{
	if (padapter->free_txbd > need_num) {
		return TRUE;
	} else {
		return FALSE;
	}
}

u8 *rtw_malloc(u32 size)
{
	u8 	*pbuf = NULL;
	pbuf = kmalloc(size, in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
	return pbuf;
}

u8 *rtw_zmalloc(u32 size)
{
	u8 *pbuf = NULL;
	pbuf = kmalloc(size, in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
	if (pbuf != NULL) {
		memset(pbuf, 0, size);
	}
	return pbuf;
}

void rtw_mfree(void *pbuf)
{
	kfree(pbuf);
}


void rtw_memcpy(void *dst, void *src, u32 sz)
{
	memcpy(dst, src, sz);
}


int rtw_thread_should_stop(void)
{
	if (kthread_should_stop()) {
		return TRUE;
	}
	return FALSE;
}

void rtw_thread_exit(void)
{

}

int rtw_create_task_ex(struct task_struct **ptask, const char *name, thread_func_t func, void *thctx)
{
	*ptask = kthread_create(func, thctx, name);
	if (IS_ERR(*ptask)) {
		printk("%s() thread start (%s) Failed!\n", __FUNCTION__, name);
		return _FAIL;
	}

	wake_up_process(*ptask);

	return _SUCCESS;
}

void rtw_delete_task_ex(struct task_struct *task)
{
	if (task) {
		kthread_stop(task);
	}
}

u32	rtw_get_current_time(void)
{
	return jiffies;
}

u32 rtw_systime_to_ms(u32 systime)
{
	return systime * 1000 / HZ;
}

u32 rtw_ms_to_systime(u32 ms)
{
	return ms * HZ / 1000;
}

u32 rtw_get_passing_time_ms(u32 start)
{
	return rtw_systime_to_ms(rtw_get_current_time() - start);
}

void rtw_msleep_os(int ms)
{
	msleep((unsigned int)ms);
}

void rtw_mdelay_os(int ms)
{
	mdelay(ms);
}


void rtw_enable_interrupt(sdio_adapter *padapter)
{
	rtw_write32(padapter, SDIO_REG_HIMR, padapter->sdio_himr);
}

void rtw_disable_interrupt(sdio_adapter *padapter)
{
	rtw_write32(padapter, SDIO_REG_HIMR, SDIO_HIMR_DISABLED);
}

