// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __RTW_PROC_H__
#define __RTW_PROC_H__

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define RTW_PROC_HDL_TYPE_SEQ	0
#define RTW_PROC_HDL_TYPE_SSEQ	1
#define RTW_PROC_HDL_TYPE_SZSEQ	2

struct rtw_proc_hdl {
	char *name;
	u8 type;
	union {
		int (*show)(struct seq_file *, void *);
		struct seq_operations *seq_op;
		struct {
			int (*show)(struct seq_file *, void *);
			size_t size;
		} sz;
	} u;
	ssize_t (*write)(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
};

#define RTW_PROC_HDL_SEQ(_name, _seq_op, _write) \
	{ .name = _name, .type = RTW_PROC_HDL_TYPE_SEQ, .u.seq_op = _seq_op, .write = _write}

#define RTW_PROC_HDL_SSEQ(_name, _show, _write) \
	{ .name = _name, .type = RTW_PROC_HDL_TYPE_SSEQ, .u.show = _show, .write = _write}

int rtw_drv_proc_init(void);
void rtw_drv_proc_deinit(void);
int proc_get_offload_enable(struct seq_file *m, void *v);
ssize_t proc_set_offload_enable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_offload_mdns_domain_name(struct seq_file *m, void *v);
ssize_t proc_set_offload_mdns_domain_name(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_offload_mdns_machine_name(struct seq_file *m, void *v);
ssize_t proc_set_offload_mdns_machine_name(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_offload_mdns_service_info(struct seq_file *m, void *v);
ssize_t proc_set_offload_mdns_service_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_offload_mdns_txt_rsp(struct seq_file *m, void *v);
ssize_t proc_set_offload_mdns_txt_rsp(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

#endif
