#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include "sdio_type.h"


#define DEVICE_NAME "sdio_chardev"
#define BUF_LEN 80

static dev_t first_dev;
static struct cdev cdev;
static struct class *cls;
static struct device *device;
extern sdio_adapter adapter;


void rtw_sdio_loopback(u8 *data, u32 sz)
{

	sdio_adapter *padapter = &adapter;
	u8 *pbuf = data;
	pbuf[sz - 1] = '\0';

	if (memcmp(pbuf + 9, "start", 5) == 0) {
		rtw_sdio_loopback_start(padapter);
	} else {
		rtw_sdio_loopback_stop(padapter);
	}

	_func_exit_;
}


static void rtk_sdio_dbg(u8 *data, u8 sz)
{
	sdio_adapter *padapter = &adapter;
	const char delim[] = " ";
	int write = 0; //0 for read, 1 for write
	u32 count = 0;
	int buffer[5] = {0};
	char *str, *ptr = NULL;
	int temp;
	u32 fn, addr, cnt, val = 0;
	u8 *pbuf = data;

	pbuf[sz - 1] = '\0';
	ptr = (char *)(pbuf + 5);

	count = 0;
	do {
		str = (char *)strsep(&ptr, delim);
		if (NULL == str) {
			break;
		}
		sscanf(str, "%i", &temp);
		buffer[count++] = temp;
	} while (1);

	fn = (u32)buffer[0];
	write = (u32)buffer[1];
	addr = (u32)buffer[2];
	cnt = (u32)buffer[3];

	printk("DBG: %x, %x, %x, %x\n", fn, write, addr, cnt);

	if (write) {
		val = buffer[4];
		printk("%s: fn: %d write 0x%04x (%d bytes) to register @ 0x%04x\n", __FUNCTION__, fn, val, cnt, addr);
		if (fn == 0) {
			rtw_f0_write8(padapter, addr, val);
		} else {
			if (cnt == 1) {
				rtw_write8(padapter, addr, val & 0xff);
			} else if (cnt == 2) {
				rtw_write16(padapter, addr, val & 0xffff);
			} else {
				rtw_write32(padapter, addr, val);
			}
		}
		rtw_memcpy((void *)data, (void *)&val, 4);
	} else {
		printk("%s: fn: %d read %d bytes from register @ 0x%04x\n", __FUNCTION__, fn, cnt, addr);
		if (fn == 0) {
			val = rtw_f0_read8(padapter, addr);
		} else {
			if (cnt == 1) {
				val = rtw_read8(padapter, addr);
			} else if (cnt == 2) {
				val = rtw_read16(padapter, addr);
			} else {
				val = rtw_read32(padapter, addr);
			}
		}
		printk("%s: fn: %d register(offset: 0x%04x): 0x%08x\n", __FUNCTION__, fn, addr, val);

		rtw_memcpy((void *)data, (void *)&val, 4);

	}

}

static long rtk_sdio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	u8 data[50];

	switch (cmd) {
	case RTK_SDIO_DBG:
		if (copy_from_user(data, (char __user *)arg, 50)) {
			return -EFAULT;
		}
		rtk_sdio_dbg(data, 50);

		if (copy_to_user((void __user *)arg, data, 4)) {
			return -EFAULT;
		}
		break;
	case RTK_SDIO_LOOPSTART:
		if (copy_from_user(data, (char __user *)arg, 50)) {
			return -EFAULT;
		}

		rtw_sdio_loopback(data, 20);
		break;
	default:
		printk("Unsupported ioctl cmd: 0x%08X\n", cmd);
		break;
	}

	return ret;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = rtk_sdio_ioctl,
};

int mychardev_init(void)
{
	int alloc_ret = 0;

	alloc_ret = alloc_chrdev_region(&first_dev, 0, 1, DEVICE_NAME);
	if (alloc_ret < 0) {
		printk(KERN_ALERT "Failed to allocate char device region.\n");
		return alloc_ret;
	}

	cls = class_create(DEVICE_NAME);
	if (!cls) {
		printk("Failed to create class.\n");
	}

	device = device_create(cls, NULL, first_dev, NULL, DEVICE_NAME);
	if (!device) {
		printk("Failed to create device.\n");
	}

	cdev_init(&cdev, &fops);
	cdev.owner = THIS_MODULE;
	cdev.dev = first_dev;

	if (cdev_add(&cdev, first_dev, 1)) {
		printk("Failed to add char device.\n");
		unregister_chrdev_region(first_dev, 1);
		return -1;
	}

	printk("Create chardev (major=%d minor=%d) OK.\n", MAJOR(first_dev), MINOR(first_dev));

	return 0;
}

void mychardev_exit(void)
{
	RTK_PRINTF("%s enter=>", __FUNCTION__);
	device_destroy(cls, first_dev);
	class_destroy(cls);
	cdev_del(&cdev);
	unregister_chrdev_region(first_dev, 1);
	printk("Goodbye\n");

}


