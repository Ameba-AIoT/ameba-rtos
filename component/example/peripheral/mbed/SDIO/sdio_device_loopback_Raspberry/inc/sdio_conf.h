#ifndef RTK_SDIO_CONF_H
#define RTK_SDIO_CONF_H

/******************************************************************************
 *
 * Copyright(c) 2020 - 2024 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

#define AMEBADPLUS
//#define RL7005

#ifdef AMEBADPLUS
#define AUTOCONF_INCLUDED
#define RTL871X_MODULE_NAME "AMEBADPLUS"
#define DRV_NAME "amebadplus"
#endif


#ifdef RL7005
#define AUTOCONF_INCLUDED
#define RTL871X_MODULE_NAME "RL7005"
#define DRV_NAME "rl7005"
#define FW_DOWNLOAD
#define SDIO_WIFI
#endif

#define CONFIG_DEBUG

#define CONFIG_SDIO_TX_ENABLE_AVAL_INT
#define CONFIG_SDIO_TX_OVF_CTRL
//#define CONFIG_SDIO_RX_AGGREGATION
//#define CONFIG_SDIO_TX_AGGREGATION

#define CHECK_DATA


#endif
