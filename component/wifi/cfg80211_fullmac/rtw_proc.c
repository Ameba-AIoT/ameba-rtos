// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#include "rtw_cfg80211_fullmac.h"

#define RTW_PROC_NAME "rtl8730e"
#define RTW_PROC_STA_NAME "wlan0"
#define RTW_PROC_AP_NAME "ap"
#define get_proc_net init_net.proc_net

extern struct inic_device global_idev;

static struct proc_dir_entry *rtw_proc = NULL;
static struct proc_dir_entry *rtw_sta_proc = NULL;
static struct proc_dir_entry *rtw_ap_proc = NULL;

inline struct proc_dir_entry *get_rtw_drv_proc(void)
{
	return rtw_proc;
}

inline struct proc_dir_entry *rtw_proc_create_dir(const char *name, struct proc_dir_entry *parent, void *data)
{
	struct proc_dir_entry *entry;

	entry = proc_mkdir_data(name, S_IRUGO | S_IXUGO, parent, data);

	return entry;
}

inline struct proc_dir_entry *rtw_proc_create_entry(const char *name, struct proc_dir_entry *parent,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
		const struct proc_ops *fops,
#else
		const struct file_operations *fops,
#endif
		void *data)
{
	struct proc_dir_entry *entry;

	entry = proc_create_data(name,  S_IFREG | S_IRUGO | S_IWUGO, parent, fops, data);

	return entry;
}

static int proc_get_sta_tsf(struct seq_file *m, void *v)
{
	u64 tsf_value = llhw_wifi_get_tsft(0);

	seq_printf(m, "%llu\n", tsf_value);

	return 0;
}

static int proc_get_ap_tsf(struct seq_file *m, void *v)
{
	u64 tsf_value = llhw_wifi_get_tsft(1);

	seq_printf(m, "%llu\n", tsf_value);

	return 0;
}

static int proc_get_dummy(struct seq_file *m, void *v)
{
	return 0;
}

static ssize_t proc_write_edcca_mode(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];
	u8 edcca_mode;

	if (count != 2) {
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		sscanf(tmp, "%u", (unsigned int *)&edcca_mode);
		if (llhw_wifi_set_edcca_mode(edcca_mode) < 0) {
			return -EFAULT;
		}
	}

	return count;
}

int llhw_wifi_get_ant_info(u8 *antdiv_mode, u8 *curr_ant);
static int proc_read_edcca_mode(struct seq_file *m, void *v)
{
	u8 edcca_mode = 0;
	int ret = 0;

	ret = llhw_wifi_get_edcca_mode(&edcca_mode);
	seq_printf(m, "%d\n", edcca_mode);

	return ret;
}

static int proc_read_antdiv_mode(struct seq_file *m, void *v)
{
	u8 antdiv_mode = 0;
	u8 curr_ant = 0;
	int ret = 0;

	ret = llhw_wifi_get_ant_info(&antdiv_mode, &curr_ant);
	seq_printf(m, "%d\n", antdiv_mode);

	return ret;
}

static int proc_read_curr_ant(struct seq_file *m, void *v)
{
	u8 antdiv_mode = 0;
	u8 curr_ant = 0;
	int ret = 0;

	ret = llhw_wifi_get_ant_info(&antdiv_mode, &curr_ant);
	seq_printf(m, "%d\n", curr_ant);

	return ret;
}


static int proc_read_mp_fw(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", global_idev.mp_fw);

	return 0;
}
/*
* rtw_ndev_ap_proc
*/
const struct rtw_proc_hdl ndev_ap_proc_hdls[] = {
	RTW_PROC_HDL_SSEQ("bcn_time", proc_get_ap_tsf, NULL),
	RTW_PROC_HDL_SSEQ("edcca_mode", proc_read_edcca_mode, proc_write_edcca_mode),
	RTW_PROC_HDL_SSEQ("antdiv_mode", proc_read_antdiv_mode, NULL),
	RTW_PROC_HDL_SSEQ("current_ant", proc_read_curr_ant, NULL),
	RTW_PROC_HDL_SSEQ("mp_fw", proc_read_mp_fw, NULL),
};

const int ndev_ap_proc_hdls_num = sizeof(ndev_ap_proc_hdls) / sizeof(struct rtw_proc_hdl);

static int rtw_ndev_ap_proc_open(struct inode *inode, struct file *file)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
	ssize_t index = (ssize_t)pde_data(inode);
#else
	ssize_t index = (ssize_t)PDE_DATA(inode);
#endif
	const struct rtw_proc_hdl *hdl = ndev_ap_proc_hdls + index;
	void *private = NULL;

	if (hdl->type == RTW_PROC_HDL_TYPE_SEQ) {
		int res = seq_open(file, hdl->u.seq_op);

		if (res == 0) {
			((struct seq_file *)file->private_data)->private = private;
		}

		return res;
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.show ? hdl->u.show : proc_get_dummy;

		return single_open(file, show, private);
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SZSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.sz.show ? hdl->u.sz.show : proc_get_dummy;

		return single_open_size(file, show, private, hdl->u.sz.size);
	} else {
		return -EROFS;
	}
}

static ssize_t rtw_ndev_ap_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
	ssize_t index = (ssize_t)pde_data(file_inode(file));
#else
	ssize_t index = (ssize_t)PDE_DATA(file_inode(file));
#endif
	const struct rtw_proc_hdl *hdl = ndev_ap_proc_hdls + index;
	ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *, void *) = hdl->write;

	if (write) {
		return write(file, buffer, count, pos, NULL);
	}

	return -EROFS;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
static const struct proc_ops rtw_ap_proc_seq_fops = {
	.proc_open = rtw_ndev_ap_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
	.proc_write = rtw_ndev_ap_proc_write,
};

static const struct proc_ops rtw_ap_proc_sseq_fops = {
	.proc_open = rtw_ndev_ap_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
	.proc_write = rtw_ndev_ap_proc_write,
};
#else
static const struct file_operations rtw_ap_proc_seq_fops = {
	.owner = THIS_MODULE,
	.open = rtw_ndev_ap_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
	.write = rtw_ndev_ap_proc_write,
};

static const struct file_operations rtw_ap_proc_sseq_fops = {
	.owner = THIS_MODULE,
	.open = rtw_ndev_ap_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.write = rtw_ndev_ap_proc_write,
};
#endif

static struct proc_dir_entry *rtw_ndev_ap_proc_init(void)
{
	struct proc_dir_entry *entry = NULL;
	ssize_t i;

	if (rtw_proc == NULL) {
		dev_err(global_idev.fullmac_dev, "rtw driver proc dir not existed");
		goto exit;
	}

	if (rtw_ap_proc) {
		dev_info(global_idev.fullmac_dev, "rtw driver ap proc dir existed");
		goto exit;
	}

	rtw_ap_proc = rtw_proc_create_dir(RTW_PROC_AP_NAME, rtw_proc, NULL);
	if (rtw_ap_proc == NULL) {
		dev_err(global_idev.fullmac_dev, "rtw driver created ap proc failed");
		goto exit;
	}

	for (i = 0; i < ndev_ap_proc_hdls_num; i++) {
		if (ndev_ap_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SEQ) {
			entry = rtw_proc_create_entry(ndev_ap_proc_hdls[i].name, rtw_ap_proc, &rtw_ap_proc_seq_fops, (void *)i);
		} else if (ndev_ap_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SSEQ ||
				   ndev_ap_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SZSEQ) {
			entry = rtw_proc_create_entry(ndev_ap_proc_hdls[i].name, rtw_ap_proc, &rtw_ap_proc_sseq_fops, (void *)i);
		} else {
			entry = NULL;
		}

		if (!entry) {
			dev_err(global_idev.fullmac_dev, "rtw driver created ap entry failed");
			goto exit;
		}
	}

exit:
	return rtw_ap_proc;
}

static void rtw_ndev_ap_proc_deinit(void)
{
	int i;

	if (rtw_ap_proc == NULL) {
		dev_info(global_idev.fullmac_dev, "rtw driver ap entry not existed");
		return;
	}

	for (i = 0; i < ndev_ap_proc_hdls_num; i++) {
		remove_proc_entry(ndev_ap_proc_hdls[i].name, rtw_ap_proc);
	}

	remove_proc_entry(RTW_PROC_AP_NAME, rtw_proc);
	rtw_ap_proc = NULL;
}

/*
* rtw_ndev_sta_proc
*/
const struct rtw_proc_hdl ndev_sta_proc_hdls[] = {
	RTW_PROC_HDL_SSEQ("bcn_time", proc_get_sta_tsf, NULL),
	RTW_PROC_HDL_SSEQ("edcca_mode", proc_read_edcca_mode, proc_write_edcca_mode),
	RTW_PROC_HDL_SSEQ("antdiv_mode", proc_read_antdiv_mode, NULL),
	RTW_PROC_HDL_SSEQ("current_ant", proc_read_curr_ant, NULL),
	RTW_PROC_HDL_SSEQ("mp_fw", proc_read_mp_fw, NULL),

#ifndef CONFIG_SDIO_BRIDGE
#ifdef CONFIG_FULLMAC_HCI_SDIO
	RTW_PROC_HDL_SSEQ("offload_enable", proc_get_offload_enable, proc_set_offload_enable),

#if defined(CONFIG_OFFLOAD_MDNS_V4) || defined(CONFIG_OFFLOAD_MDNS_V6)
	RTW_PROC_HDL_SSEQ("offload_mdns_domain_name", proc_get_offload_mdns_domain_name, proc_set_offload_mdns_domain_name),
	RTW_PROC_HDL_SSEQ("offload_mdns_machine_name", proc_get_offload_mdns_machine_name, proc_set_offload_mdns_machine_name),
	RTW_PROC_HDL_SSEQ("offload_mdns_service_info", proc_get_offload_mdns_service_info, proc_set_offload_mdns_service_info),
	RTW_PROC_HDL_SSEQ("offload_mdns_service_info_txt_rsp", proc_get_offload_mdns_txt_rsp, proc_set_offload_mdns_txt_rsp),
#endif /* CONFIG_OFFLOAD_MDNS_V4 || CONFIG_OFFLOAD_MDNS_V6 */
#endif
#endif
};

const int ndev_sta_proc_hdls_num = sizeof(ndev_sta_proc_hdls) / sizeof(struct rtw_proc_hdl);

static int rtw_ndev_sta_proc_open(struct inode *inode, struct file *file)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
	ssize_t index = (ssize_t)pde_data(inode);
#else
	ssize_t index = (ssize_t)PDE_DATA(inode);
#endif
	const struct rtw_proc_hdl *hdl = ndev_sta_proc_hdls + index;
	void *private = NULL;

	if (hdl->type == RTW_PROC_HDL_TYPE_SEQ) {
		int res = seq_open(file, hdl->u.seq_op);

		if (res == 0) {
			((struct seq_file *)file->private_data)->private = private;
		}

		return res;
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.show ? hdl->u.show : proc_get_dummy;

		return single_open(file, show, private);
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SZSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.sz.show ? hdl->u.sz.show : proc_get_dummy;

		return single_open_size(file, show, private, hdl->u.sz.size);
	} else {
		return -EROFS;
	}
}

static ssize_t rtw_ndev_sta_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
	ssize_t index = (ssize_t)pde_data(file_inode(file));
#else
	ssize_t index = (ssize_t)PDE_DATA(file_inode(file));
#endif
	const struct rtw_proc_hdl *hdl = ndev_sta_proc_hdls + index;
	ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *, void *) = hdl->write;

	if (write) {
		return write(file, buffer, count, pos, NULL);
	}

	return -EROFS;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
static const struct proc_ops rtw_sta_proc_seq_fops = {
	.proc_open = rtw_ndev_sta_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
	.proc_write = rtw_ndev_sta_proc_write,
};

static const struct proc_ops rtw_sta_proc_sseq_fops = {
	.proc_open = rtw_ndev_sta_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
	.proc_write = rtw_ndev_sta_proc_write,
};
#else
static const struct file_operations rtw_sta_proc_seq_fops = {
	.owner = THIS_MODULE,
	.open = rtw_ndev_sta_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
	.write = rtw_ndev_sta_proc_write,
};

static const struct file_operations rtw_sta_proc_sseq_fops = {
	.owner = THIS_MODULE,
	.open = rtw_ndev_sta_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.write = rtw_ndev_sta_proc_write,
};
#endif

static struct proc_dir_entry *rtw_ndev_sta_proc_init(void)
{
	struct proc_dir_entry *entry = NULL;
	ssize_t i;

	if (rtw_proc == NULL) {
		dev_err(global_idev.fullmac_dev, "rtw driver proc dir not existed");
		goto exit;
	}

	if (rtw_sta_proc) {
		dev_info(global_idev.fullmac_dev, "rtw driver sta proc dir existed");
		goto exit;
	}

	rtw_sta_proc = rtw_proc_create_dir(RTW_PROC_STA_NAME, rtw_proc, NULL);
	if (rtw_sta_proc == NULL) {
		dev_err(global_idev.fullmac_dev, "rtw driver created sta proc failed");
		goto exit;
	}

	for (i = 0; i < ndev_sta_proc_hdls_num; i++) {
		if (ndev_sta_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SEQ) {
			entry = rtw_proc_create_entry(ndev_sta_proc_hdls[i].name, rtw_sta_proc, &rtw_sta_proc_seq_fops, (void *)i);
		} else if (ndev_sta_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SSEQ ||
				   ndev_sta_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SZSEQ) {
			entry = rtw_proc_create_entry(ndev_sta_proc_hdls[i].name, rtw_sta_proc, &rtw_sta_proc_sseq_fops, (void *)i);
		} else {
			entry = NULL;
		}

		if (!entry) {
			dev_err(global_idev.fullmac_dev, "rtw driver created sta entry failed");
			goto exit;
		}
	}

exit:
	return rtw_sta_proc;
}

static void rtw_ndev_sta_proc_deinit(void)
{
	int i;

	if (rtw_sta_proc == NULL) {
		dev_info(global_idev.fullmac_dev, "rtw driver sta entry not existed");
		return;
	}

	for (i = 0; i < ndev_sta_proc_hdls_num; i++) {
		remove_proc_entry(ndev_sta_proc_hdls[i].name, rtw_sta_proc);
	}

	remove_proc_entry(RTW_PROC_STA_NAME, rtw_proc);
	rtw_sta_proc = NULL;
}

/*
* rtw_drv_proc
*/
const struct rtw_proc_hdl drv_proc_hdls[] = {
};

const int drv_proc_hdls_num = sizeof(drv_proc_hdls) / sizeof(struct rtw_proc_hdl);

static int rtw_drv_proc_open(struct inode *inode, struct file *file)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
	ssize_t index = (ssize_t)pde_data(inode);
#else
	ssize_t index = (ssize_t)PDE_DATA(inode);
#endif
	const struct rtw_proc_hdl *hdl = drv_proc_hdls + index;
	void *private = NULL;

	if (hdl->type == RTW_PROC_HDL_TYPE_SEQ) {
		int res = seq_open(file, hdl->u.seq_op);

		if (res == 0) {
			((struct seq_file *)file->private_data)->private = private;
		}

		return res;
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.show ? hdl->u.show : proc_get_dummy;

		return single_open(file, show, private);
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SZSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.sz.show ? hdl->u.sz.show : proc_get_dummy;

		return single_open_size(file, show, private, hdl->u.sz.size);
	} else {
		return -EROFS;
	}
}

static ssize_t rtw_drv_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
	ssize_t index = (ssize_t)pde_data(file_inode(file));
#else
	ssize_t index = (ssize_t)PDE_DATA(file_inode(file));
#endif
	const struct rtw_proc_hdl *hdl = drv_proc_hdls + index;
	ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *, void *) = hdl->write;

	if (write) {
		return write(file, buffer, count, pos, NULL);
	}

	return -EROFS;
}

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
static const struct proc_ops rtw_drv_proc_seq_fops = {
	.proc_open = rtw_drv_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
	.proc_write = rtw_drv_proc_write,
};

static const struct proc_ops rtw_drv_proc_sseq_fops = {
	.proc_open = rtw_drv_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
	.proc_write = rtw_drv_proc_write,
};
#else
static const struct file_operations rtw_drv_proc_seq_fops = {
	.owner = THIS_MODULE,
	.open = rtw_drv_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
	.write = rtw_drv_proc_write,
};

static const struct file_operations rtw_drv_proc_sseq_fops = {
	.owner = THIS_MODULE,
	.open = rtw_drv_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.write = rtw_drv_proc_write,
};
#endif

/*
* rtw_drv_proc
*/
int rtw_drv_proc_init(void)
{
	int ret = 0;
	int i = 0;
	struct proc_dir_entry *entry = NULL;

	if (rtw_proc != NULL) {
		dev_info(global_idev.fullmac_dev, "rtw driver proc existed");
		goto exit;
	}

	rtw_proc = rtw_proc_create_dir(RTW_PROC_NAME, get_proc_net, NULL);
	if (rtw_proc == NULL) {
		dev_err(global_idev.fullmac_dev, "rtw driver proc create dir failed");
		goto exit;
	}

	if (drv_proc_hdls_num > 0) {
		for (i = 0; i < drv_proc_hdls_num; i++) {
			if (drv_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SEQ) {
				entry = rtw_proc_create_entry(drv_proc_hdls[i].name, rtw_proc, &rtw_drv_proc_seq_fops, (void *)(uintptr_t)i);
			} else if (drv_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SSEQ ||
					   drv_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SZSEQ) {
				entry = rtw_proc_create_entry(drv_proc_hdls[i].name, rtw_proc, &rtw_drv_proc_sseq_fops, (void *)(uintptr_t)i);
			} else {
				entry = NULL;
			}

			if (!entry) {
				dev_err(global_idev.fullmac_dev, "rtw driver proc create entry failed");
				goto exit;
			}
		}
	}

	rtw_ndev_sta_proc_init();
	rtw_ndev_ap_proc_init();

	ret = 1;

exit:
	return ret;
}

void rtw_drv_proc_deinit(void)
{
	int i = 0;

	if (rtw_proc == NULL) {
		return;
	}

	rtw_ndev_sta_proc_deinit();
	rtw_ndev_ap_proc_deinit();

	if (drv_proc_hdls_num > 0) {
		for (i = 0; i < drv_proc_hdls_num; i++) {
			remove_proc_entry(drv_proc_hdls[i].name, rtw_proc);
		}
	}

	remove_proc_entry(RTW_PROC_NAME, get_proc_net);
	rtw_proc = NULL;
}
