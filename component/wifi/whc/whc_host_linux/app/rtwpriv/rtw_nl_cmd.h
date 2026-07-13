/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 *****************************************************************************/

#ifndef __RTW_NL_CMD_H__
#define __RTW_NL_CMD_H__

/* Print Netlink (-n) command list */
void rtw_nl_cmd_help(void);

/*
 * Try to dispatch argv[2] via the WHC command table.
 * Returns 1 if dispatched (result in *retval), 0 if not found.
 */
int rtw_nl_cmd_try_dispatch(int argc, char *argv[], int *retval);

#endif /* __RTW_NL_CMD_H__ */
