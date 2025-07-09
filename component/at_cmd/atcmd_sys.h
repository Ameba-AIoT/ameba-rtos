/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_SYS_H__
#define __ATCMD_SYS_H__

#define ATCMD_VERSION          2       /* ATCMD MAJOR VERSION, AT FORMAT CHANGED, OR PORTING NEW CHIPS */
#define ATCMD_SUBVERSION       4       /* ATCMD MINOR VERSION, NEW COMMAND ADDED OR DELETED */
#define ATCMD_REVISION         2       /* ATCMD FIX BUG REVISION */

typedef struct {
	u8 Pattern[8];
	u8 Rsvd1[8];
	u8 Ver;
	u8 ImgID;
	u8 AuthAlg;
	u8 HashAlg;
	u16 MajorKeyVer;
	u16 MinorKeyVer;
} Certificate_TypeDef;
typedef enum atcmd_cpuload_type_e {
	atcmd_cpuload_type_update = 0,
	atcmd_cpuload_type_sema = 1,
	atcmd_cpuload_type_tick = 2,
	atcmd_cpuload_type_stop = 3,
	atcmd_cpuload_type_invalid
} atcmd_cpuload_type;

enum {
	IMG_CERT   =   0x0,
	IMG_IMG2,
	IMG_TYPE_CNT,
};

extern void print_bt_ext_at(void);

void at_sys_init(void);
void print_system_at(void);
#endif
