#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/file.h>
#include <whc_host_linux.h>
#include "whc_sdio_host.h"

u8 old_data_crc_err_cnt;
u8 new_data_crc_err_cnt;

char *rtw_fw1_path = "/etc/image/km4tz_fullmac.bin";

u32	rtw_sdio_get_current_time(void)
{
	return jiffies;
}

u32 rtw_sdio_systime_to_ms(u32 systime)
{
	return systime * 1000 / HZ;
}

u32 rtw_sdio_get_passing_time_ms(u32 start)
{
	return rtw_sdio_systime_to_ms(rtw_sdio_get_current_time() - start);
}

/*
 * Open a file with the specific @param path, @param flag, @param mode
 * @param fpp the pointer of struct file pointer to get struct file pointer while file opening is success
 * @param path the path of the file to open
 * @param flag file operation flags, please refer to linux document
 * @param mode please refer to linux document
 * @return Linux specific error code
 */
static int rtw_sdio_openfile(struct file **fpp, char *path, int flag, int mode)
{
	struct file *fp;

	fp = filp_open(path, flag, mode);
	if (IS_ERR(fp)) {
		*fpp = NULL;
		return PTR_ERR(fp);
	} else {
		*fpp = fp;
		return 0;
	}
}

/*
 * Close the file with the specific @param fp
 * @param fp the pointer of struct file to close
 * @return always 0
 */
static int rtw_sdio_closefile(struct file *fp)
{
	filp_close(fp, NULL);
	return 0;
}

static int rtw_sdio_readfile(struct file *fp, char *buf, int len)
{
	int rlen = 0, sum = 0;

	while (sum < len) {
		// rlen = fp->f_op->read(fp, buf + sum, len - sum, &fp->f_pos);
		// rlen = vfs_read(fp, buf + sum, len - sum, &fp->f_pos);
		rlen = kernel_read(fp, buf + sum, len - sum, &fp->f_pos);
		// rlen = simple_attr_read(fp, buf + sum, len - sum, &fp->f_pos);// BUG: kernel NULL pointer dereference, address: 0000000000000000
		if (rlen > 0) {
			sum += rlen;
		} else if (0 != rlen) {
			// dev_info(&func->dev,"%s rlen is %d\n", __FUNCTION__, rlen);
			return rlen;
		} else {
			// dev_info(&func->dev,"%s rlen is 0\n", __FUNCTION__);
			break;
		}
	}

	return sum;
}

/*
 * Open the file with @param path and retrive the file content into memory starting from @param buf for @param sz at most
 * @param path the paretriveFromFileth of the file to open and read
 * @param buf the starting address of the buffer to store file content
 * @param sz how many bytes to read at most
 * @return the byte we've read, or Linux specific error code
 */
static int rtw_sdio_retrivefromfile(char *path, u8 *buf, u32 sz)
{
	int ret = -1;
	struct file *fp;

	if (path && buf) {
		if (0 == (ret = rtw_sdio_openfile(&fp, path, O_RDONLY, 0))) {

			ret = rtw_sdio_readfile(fp, buf, sz);

			rtw_sdio_closefile(fp);
		} else {
		}
	} else {
		ret = -EINVAL;
	}
	return ret;
}

/*
 * Open the file with @param path and retrive the file content into memory starting from @param buf for @param sz at most
 * @param path the path of the file to open and read
 * @param buf the starting address of the buffer to store file content
 * @param sz how many bytes to read at most
 * @return the byte we've read
 */
int rtw_sdio_retrive_from_file(char *path, u8 *buf, u32 sz)
{
	int ret = rtw_sdio_retrivefromfile(path, buf, sz);
	return ret >= 0 ? ret : 0;
}


int rtw_sdio_write_fw(struct whc_sdio *priv, u32 cnt, u8 *pmem)
{
	int r_val;

	struct sdio_func *func = priv->func;

	sdio_claim_host(priv->func);
	r_val = sdio_memcpy_toio(priv->func, SDIO_TX_FW_ADDR | cnt >> 2, (void *)pmem, cnt);
	sdio_release_host(priv->func);

	if (r_val) {
		dev_err(&func->dev, "%s error: %d\n", __FUNCTION__, r_val);
	}

	return r_val;
}

static int rtw_sdio_fwpagewrite(struct whc_sdio *priv, u32 offset, void *pdata, u32 size)
{
	u8 ret = false;
	int try_cnt = 100;
	u8 reg8;
	struct sdio_func *func = priv->func;

	ret = rtw_write32(priv, SDIO_REG_FW_DL_ADDR, offset);
	if (ret) {
		dev_err(&func->dev, "%s: Write addr fail (%d)!\n", __FUNCTION__, ret);
		goto exit;
	}

	reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
	reg8 &= (~SD_FW_PAGE_DONE);
	ret = rtw_write8(priv, SDIO_REG_FW_DL_CTRL, reg8);
	if (ret) {
		dev_err(&func->dev, "%s: Write SD_FW_PAGE_DONE fail (%d)!\n", __FUNCTION__, ret);
		goto exit;
	}

	ret = rtw_sdio_write_fw(priv, size, pdata);

	msleep(2); // wait memory write done
	for (; try_cnt > 0; try_cnt--) {
		// check page done bit
		reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
		if (reg8 & SD_FW_PAGE_DONE) {
			// dev_info(&func->dev,"%s: FW_PAGE_DONE\n", __FUNCTION__);
			ret = true;
			break;
		} else {
			msleep(1); // wait 1ms * 100
		}
	}

	if (try_cnt <= 0) {
		dev_err(&func->dev, "%s: Polling SD_FW_PAGE_DONE bit timeout!\n", __FUNCTION__);
		ret = false;
		goto exit;
	}

	new_data_crc_err_cnt = rtw_read8(priv, SDIO_REG_CRC_ERR_INDEX);
	if (new_data_crc_err_cnt != old_data_crc_err_cnt) {
		dev_err(&func->dev, "%s: CRC error occur! old (%x), new (%x)!\n", __FUNCTION__, old_data_crc_err_cnt, new_data_crc_err_cnt);
		old_data_crc_err_cnt = new_data_crc_err_cnt;
		ret = false;
	}

exit:
	return ret;
}

static int rtw_sdio_writefw(struct whc_sdio *priv, void *buffer, u32 startaddr, u32 size)
{
	int ret = true;
	u32 pageNums, remainSize;
	u32 page, offset;
	u8 *bufferPtr = (u8 *)buffer;
	u32 fw_startaddr;
	int retry_cnt = 2;
	struct sdio_func *func = priv->func;

	pageNums = size / MAX_DLFW_PAGE_SIZE;
	remainSize = size % MAX_DLFW_PAGE_SIZE;

	dev_info(&func->dev, "%s: %dpage(%dKB) + %dB\n", __FUNCTION__, pageNums, MAX_DLFW_PAGE_SIZE >> 10, remainSize);

	fw_startaddr = startaddr;

	old_data_crc_err_cnt = rtw_read8(priv, SDIO_REG_CRC_ERR_INDEX);

	for (page = 0; page < pageNums;) {
		offset = page * MAX_DLFW_PAGE_SIZE;

		// dev_info(&func->dev,"[%d] StartAddr=0x%08x Len=%d\n", page, (fw_startaddr + offset), MAX_DLFW_PAGE_SIZE);

		ret = rtw_sdio_fwpagewrite(priv, (fw_startaddr + offset), bufferPtr + offset, MAX_DLFW_PAGE_SIZE);
		if (ret == true) {
			page++;
			retry_cnt = 10;
		} else {
			retry_cnt--;
			dev_err(&func->dev, "[%d]page write fail, retry %d time(s)!\n", page, 10 - retry_cnt);
			if (retry_cnt <= 0) {
				goto exit;
			}
		}
	}

	if (remainSize) {
		offset = pageNums * MAX_DLFW_PAGE_SIZE;
		page = pageNums;

		dev_info(&func->dev, "[Remain] StartAddr=0x%08x Len=%d\n", (fw_startaddr + offset), remainSize);

		while (1) {
			ret = rtw_sdio_fwpagewrite(priv, (fw_startaddr + offset), bufferPtr + offset, remainSize);
			if (ret == true) {
				break;
			} else {
				dev_err(&func->dev, "[Remain] page write fail, retry %d time(s)!\n", 10 - retry_cnt);
				if (retry_cnt <= 0) {
					break;
				}
			}
		}
	}

exit:
	return ret;
}


int rtw_sdio_firmware_download(struct whc_sdio *priv, u32 need_checksum)
{
	s32 rtStatus = true;
	u8 write_fw = 0;
	u32 fwdl_start_time;
	u8 *FwBuffer8721f = NULL;
	u8 *pFirmwareBuf;
	u32 FirmwareLen;
	u8 *fwfilepath;
	u8 reg8;
	u16 reg16;
	int try_cnt = 3;

	u32 checksum = 0;
	u32 checksum_device = 0;
	int i;
	u32 *buf;

	u32 loop_cnt = 10;
	struct sdio_func *func = priv->func;

	dev_info(&func->dev, "enter %s \n", __FUNCTION__);

	// check FW_NEED_UPDATE
	reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);

	if (!(reg8 & SD_FW_DL_MODE)) {
		dev_info(&func->dev, "%s: No need to update firmware!\n", __FUNCTION__);
		return true;
	}

	FwBuffer8721f = kmalloc(FW_RL005_SIZE, GFP_KERNEL);
	if (!FwBuffer8721f) {
		dev_err(&func->dev, "%s memory allocate failed\n", __FUNCTION__);
		rtStatus = false;
		goto exit;
	}

dl_again:
	// Download Image1
	fwfilepath = rtw_fw1_path;
	rtw_write32(priv, SDIO_REG_HIMR, 0x10000000);
	rtStatus = rtw_sdio_retrive_from_file(fwfilepath, FwBuffer8721f, FW_RL005_SIZE);
	fwdl_start_time = rtw_sdio_get_current_time();
	FirmwareLen = rtStatus >= 0 ? rtStatus : 0;
	pFirmwareBuf = FwBuffer8721f;
	while (!priv->bSurpriseRemoved && (write_fw++ < 3 || rtw_sdio_get_passing_time_ms(fwdl_start_time) < 5000)) {
		// dev_info(&func->dev,"pFirmwareBuf %p, 0x%x, 0x%x\n", pFirmwareBuf, FW1_START_ADDR, FirmwareLen);
		rtStatus = rtw_sdio_writefw(priv, pFirmwareBuf, FW1_START_ADDR, FirmwareLen);
		if (rtStatus == true) {
			break;
		}
	}

	if (write_fw >= 3) {
		dev_err(&func->dev, "write fw1 fail\n");
		goto fwdl_stat;
	}

	if (true != rtStatus) {
		goto fwdl_stat;
	}

	// padapter->FirmwareEntryFun = 0;

	// todo_eric. Tell device all images have been downloaded
	if (need_checksum == 0) {

		for (; try_cnt > 0; try_cnt--) {
			reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
			reg8 |= SD_FW_DL_DONE;
			rtw_write8(priv, SDIO_REG_FW_DL_CTRL, reg8);

			reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
			if (reg8 & SD_FW_DL_DONE) {
				break;
			}
		}
	} else {
		rtw_write32(priv, SDIO_REG_H2C_MSG_EXT, FW1_START_ADDR);
		rtw_write32(priv, SDIO_REG_H2C_MSG, FirmwareLen);
		reg16 = rtw_read16(priv, SDIO_REG_HRPWM2);
		reg16 |= HRPWM2_NEED_CHECKSUM; // host need checksum
		rtw_write16(priv, SDIO_REG_HRPWM2, reg16);

		// Tell device all images have been downloaded
		for (; try_cnt > 0; try_cnt--) {
			reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
			reg8 |= SD_FW_DL_DONE;
			rtw_write8(priv, SDIO_REG_FW_DL_CTRL, reg8);

			reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
			if (reg8 & SD_FW_DL_DONE) {
				break;
			}
		}

		checksum = 0;
		checksum_device = 0;
		buf = (u32 *)pFirmwareBuf;
		for (i = 0; i < FirmwareLen / 4; i++) {
			checksum ^= buf[i];
		}
		// dev_info(&func->dev,"checksum value: %x \n", checksum);
		while (1) {
			if (rtw_read16(priv, SDIO_REG_HCPWM2) & HCPWM2_CHECKSUM_READY) {
				checksum_device = rtw_read32(priv, SDIO_REG_C2H_MSG_EXT);
				// dev_info(&func->dev,"checksum device value: %x \n", checksum_device);

				if (checksum == checksum_device) {
					dev_info(&func->dev, "checksum success\n");
					break;
				} else {
					dev_err(&func->dev, "checksum fail\n");
					reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
					reg8 &= ~SD_FW_DL_DONE;
					rtw_write8(priv, SDIO_REG_FW_DL_CTRL, reg8);

					reg16 = rtw_read16(priv, SDIO_REG_HRPWM2);
					reg16 &= ~HRPWM2_NEED_CHECKSUM;
					rtw_write16(priv, SDIO_REG_HRPWM2, reg16);
					msleep(1000);
					rtStatus = true;
					write_fw = 0;
					try_cnt = 3;
					rtw_write16(priv, SDIO_REG_HRPWM2, 0);
					if (loop_cnt--) {
						goto dl_again;
					} else {
						goto exit;
					}
				}
			}
		}
	}

	if (try_cnt <= 0) {
		dev_err(&func->dev, "Fail to Set FW download complete bit %s\n", __FUNCTION__);
		rtStatus = false;
	}

fwdl_stat:
	dev_err(&func->dev, "FWDL %s. write_fw:%u, %dms\n", (rtStatus == true) ? "success" : "fail", write_fw, rtw_sdio_get_passing_time_ms(fwdl_start_time));

exit:

	if (FwBuffer8721f) {
		kfree((u8 *)FwBuffer8721f);
	}

	return rtStatus;
}
