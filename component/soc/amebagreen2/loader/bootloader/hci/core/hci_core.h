/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HCI_CORE_H_
#define _HCI_CORE_H_

/* Includes ------------------------------------------------------------------ */

#include "ameba_soc.h"

/* Exported defines ----------------------------------------------------------*/

#define HCI_XFER_REQ_QUERY				0x01U
#define HCI_XFER_REQ_WRITE				0x02U
#define HCI_XFER_REQ_CHECK				0x03U
#define HCI_XFER_REQ_BOOT				0x04U
#define HCI_XFER_REQ_ABORT				0x05U
#define HCI_XFER_REQ_ERASE				0x06U
#define HCI_XFER_REQ_HASH				0x07U

#define HCI_XFER_ACK_MASK				0x80U

#define HCI_XFER_ACK_QUERY				0x81U
#define HCI_XFER_ACK_WRITE				0x82U
#define HCI_XFER_ACK_CHECK				0x83U
#define HCI_XFER_ACK_BOOT				0x84U
#define HCI_XFER_ACK_ABORT				0x85U
#define HCI_XFER_ACK_ERASE				0x86U
#define HCI_XFER_ACK_HASH				0x87U

#define HCI_FW_TYPE_ROM					0xF0U
#define HCI_FW_TYPE_BOOTLOADER			0xF1U
#define HCI_FW_TYPE_APPLICATION			0xF2U

#define HCI_WRITE_CFG_CHK_EN			0x01U

#define HCI_HASH_SIZE					32U

#define HCI_PAGE_SIZE					2048U

#define HCI_PROTOCOL_VERSION			0x0100

#define HCI_CHIP_ID						0x8721

#define HCI_TX_BUF_SIZE					64U   // Max HCI_PKT_DESC_SIZE + HCI_HASH_SIZE
#define HCI_RX_BUF_SIZE					2080U // Max HCI_PKT_DESC_SIZE + HCI_PAGE_SIZE

/* Exported types ------------------------------------------------------------*/

struct _HCI_InterfaceTypeDef;

typedef struct {
	// DWORD 0
	u32	DataLen: 16;	// Data length
	u32	DataOffset: 8;	// Data offset i.e. header length
	u32	Config: 8;		// Configuration

	// DWORD 1
	u32	PacketType: 8;	// Packet type
	u32	XferStatus: 8;	// Transfer status
	u32	Version: 8;		// Version
	u32	DevMode: 8;		// Device mode

	// DWORD 2
	u32	MemAddr;		// Memory address

	// DWORD 3
	u32	MemSize;		// Memory size

	// DWORD 4
	u32 Value;			// Value

	// DWORD 5
	u32	Reserved;		// Reserved
} HCI_PacketDescTypeDef;

typedef struct {
	// DWORD 0
	u32 ChipId: 16;
	u32 CutVersion: 8;
	u32 MemType: 8;

	// DWORD 1
	u32 ProtocolVersion: 16;
	u32 SramSizeInKB: 16;

	// DWORD 2
	u32 PramSizeInKB: 16;
	u32 FlashSizeInKB: 16;

	// DWORD 3-7
	u32 Reserved[5];
} HCI_DeviceInfoTypeDef;

typedef struct {
	HCI_DeviceInfoTypeDef DeviceInfo;
	struct _HCI_InterfaceTypeDef *Interface;
	u8 RxBuffer[HCI_RX_BUF_SIZE];
	u8 TxBuffer[HCI_TX_BUF_SIZE];
	__IO u8 Abort;
	__IO u8 Boot;
	__IO u32 AbortDelayUs;
	__IO u32 BootDelayUs;
} HCI_AdapterTypeDef;

typedef struct _HCI_InterfaceTypeDef {
	int(*Init)(HCI_AdapterTypeDef *adapter);
	void(*DeInit)(HCI_AdapterTypeDef *adapter);
	int(*Transmit)(HCI_AdapterTypeDef *adapter, u8 *buf, u32 size);
} HCI_InterfaceTypeDef;

/* Exported macros -----------------------------------------------------------*/

#define	HCI_PKT_DESC_SIZE				(sizeof(HCI_PacketDescTypeDef))

/* Exported variables --------------------------------------------------------*/

extern HCI_AdapterTypeDef HCI_Adapter;

/* Exported functions --------------------------------------------------------*/

int HCI_Init(void);
void HCI_DeInit(void);
int HCI_RxProcess(HCI_AdapterTypeDef *adapter, u8 *buf, u32 size);
int HCI_WaitForExit(void);

#endif // _HCI_CORE_H_