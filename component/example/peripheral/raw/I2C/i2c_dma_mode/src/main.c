/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "PinNames.h"
#include "basic_types.h"
#include "diag.h"
#include "i2c_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

typedef struct {
	u8 *pbuf;
	int datalen;
} i2c_data;

typedef struct {
	uint32_t i2c_idx;
	I2C_TypeDef *I2Cx;
} i2c_t;

typedef struct {
	i2c_t 		I2Cint;
	i2c_data 	ptxbuf;
	i2c_data 	prxbuf;
	IRQ_FUN 	user_tx_cb;
	IRQ_FUN 	user_rx_cb;
	GDMA_InitTypeDef    I2CTxGdmaInitStruct;              //Pointer to GDMA_InitTypeDef
	GDMA_InitTypeDef    I2CRxGdmaInitStruct;              //Pointer to GDMA_InitTypeDef
} i2c_dma_t;

/*I2C pin location:
* I2C0:
*	  - S0:  PA_25(SCL)/PA_26(SDA).
*	         PB_5(SCL)/PB_6(SDA).
*/

#define MBED_I2C_SLAVE_ADDR0    0x23

#define I2C_DMA_DATA_TX_LENGTH		255
#define I2C_DMA_DATA_RX_LENGTH		255

#define I2C_DATA_LENGTH         255
uint8_t	i2cdatasrc[I2C_DATA_LENGTH];
uint8_t	i2cdatadst[I2C_DATA_LENGTH];
I2C_InitTypeDef I2CInitData[2];

#define I2C_MASTER_DEVICE		1

#define I2C_0 0
#define I2C_1 1


/*if defined 1, master send, slave read mode.
 else master read slave send mode.*/
#define MASTER_SEND				1

i2c_dma_t i2cdmamaster;
i2c_dma_t i2cdmaslave;
int length = 255;

u32
I2CTXGDMAISRHandle(
	IN  void *Data
)
{

	i2c_dma_t *obj = (i2c_dma_t *)Data;
	PGDMA_InitTypeDef   GDMA_InitStruct = &obj->I2CTxGdmaInitStruct;
	I2C_TypeDef *I2Cx = obj->I2Cint.I2Cx;

	printf("%s\n", __func__);

	I2C_DMAControl(I2Cx, I2C_BIT_TDMAE, DISABLE);

	/*while TX FIFO empty*/
	while (0 == I2C_CheckFlagState(I2Cx, I2C_BIT_TFE));

	I2C_ClearAllINT(I2Cx);

	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);

	//3 Clear Pending ISR
	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	if (obj->ptxbuf.datalen > I2C_DMA_DATA_TX_LENGTH) {
		obj->ptxbuf.datalen -= I2C_DMA_DATA_TX_LENGTH;
		obj->ptxbuf.pbuf += I2C_DMA_DATA_TX_LENGTH;

		if (obj->ptxbuf.datalen > I2C_DMA_DATA_TX_LENGTH) {
			GDMA_InitStruct->GDMA_BlockSize = I2C_DMA_DATA_TX_LENGTH;
			GDMA_InitStruct->GDMA_SrcAddr = (u32)obj->ptxbuf.pbuf;
			GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
			GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

			I2C_DmaMode1Config(I2Cx, I2C_BIT_DMODE_ENABLE, I2C_DMA_DATA_TX_LENGTH);
			I2C_DMAControl(I2Cx, I2C_BIT_TDMAE, ENABLE);

		} else if (obj->ptxbuf.datalen <= I2C_DMA_DATA_TX_LENGTH) {
			GDMA_InitStruct->GDMA_BlockSize = obj->ptxbuf.datalen;
			GDMA_InitStruct->GDMA_SrcAddr = (u32)obj->ptxbuf.pbuf;
			GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
			GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

			I2C_DmaMode1Config(I2Cx, I2C_BIT_DMODE_ENABLE | I2C_BIT_DMODE_STOP, obj->ptxbuf.datalen);
			I2C_DMAControl(I2Cx, I2C_BIT_TDMAE, ENABLE);
		}
	} else {

		if (obj->user_tx_cb) {
			obj->user_tx_cb(obj);
		}

		I2C_INTConfig(I2Cx, (I2C_BIT_M_TX_ABRT  |
							 I2C_BIT_M_TX_OVER),
					  DISABLE);

		GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	}
	return 0;
}

/*Uesd for data length large than 4095*/
u32
I2CRXGDMAISRHandle(
	IN  void *Data
)
{
	i2c_dma_t *obj = (i2c_dma_t *)Data;
	PGDMA_InitTypeDef   GDMA_InitStruct = &obj->I2CRxGdmaInitStruct;
	I2C_TypeDef *I2Cx = obj->I2Cint.I2Cx;

	printf("%s\n", __func__);

	I2C_ClearAllINT(I2Cx);
	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	//GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
	if (obj->prxbuf.datalen > I2C_DMA_DATA_RX_LENGTH) {
		obj->prxbuf.datalen -= I2C_DMA_DATA_RX_LENGTH;
		obj->prxbuf.pbuf += I2C_DMA_DATA_RX_LENGTH;

		if (obj->prxbuf.datalen > I2C_DMA_DATA_RX_LENGTH) {

			GDMA_InitStruct->GDMA_BlockSize = I2C_DMA_DATA_RX_LENGTH;
			GDMA_InitStruct->GDMA_DstAddr = (u32)obj->prxbuf.pbuf;
			GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
			GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
			GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

		} else if (obj->prxbuf.datalen <= I2C_DMA_DATA_RX_LENGTH) {
			GDMA_InitStruct->GDMA_BlockSize = obj->prxbuf.datalen;
			GDMA_InitStruct->GDMA_DstAddr = (u32)obj->prxbuf.pbuf;
			GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
			GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
			GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
			GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
		}
	} else {

		if (obj->user_rx_cb) {
			obj->user_rx_cb(obj);
		}

		GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
		GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
		I2C_INTConfig(I2Cx, 0xFFFFFFFF, DISABLE);
	}
	return 0;
}

static HAL_Status
RtkI2CSendDmaMaster(
	IN  i2c_dma_t *obj
)
{
	printf("%s\n", __func__);
	uint32_t i2c_idx = obj->I2Cint.i2c_idx;
	I2C_InitTypeDef   *I2C_InitStruct = &I2CInitData[i2c_idx];
	PGDMA_InitTypeDef   GDMA_InitStruct = &obj->I2CTxGdmaInitStruct;
	I2C_TypeDef *I2Cx = obj->I2Cint.I2Cx;

	I2C_TXGDMA_Init(i2c_idx, GDMA_InitStruct, obj,
					(IRQ_FUN)(I2CTXGDMAISRHandle), obj->ptxbuf.pbuf, obj->ptxbuf.datalen);

	/* I2C Enable TX Related Interrupts */
	I2C_INTConfig(I2Cx, (I2C_BIT_M_TX_ABRT |
						 I2C_BIT_M_TX_OVER),
				  ENABLE);

	/* I2C DMA configuration */
	if (I2C_InitStruct->I2CDMAMod == I2C_DMA_REGISTER) {
		I2C_DmaMode1Config(I2Cx, I2C_BIT_DMODE_ENABLE | I2C_BIT_DMODE_STOP, obj->ptxbuf.datalen);
	}

	I2C_DMAControl(I2Cx, I2C_BIT_TDMAE, ENABLE);
	return HAL_OK;
}

static HAL_Status
RtkI2CSendDmaSlave(
	IN i2c_dma_t *obj
)
{
	uint32_t i2c_idx = obj->I2Cint.i2c_idx;
	I2C_InitTypeDef   *I2C_InitStruct = &I2CInitData[i2c_idx];
	PGDMA_InitTypeDef   GDMA_InitStruct = &obj->I2CTxGdmaInitStruct;
	I2C_TypeDef *I2Cx = obj->I2Cint.I2Cx;

	printf("%s\n", __func__);

	I2C_TXGDMA_Init(i2c_idx, GDMA_InitStruct, obj,
					(IRQ_FUN)(I2CTXGDMAISRHandle), obj->ptxbuf.pbuf, obj->ptxbuf.datalen);

	/* I2C Enable TX Related Interrupts */
	I2C_INTConfig(I2Cx, (I2C_BIT_M_TX_ABRT  |
						 I2C_BIT_M_TX_OVER),
				  ENABLE);

	/* I2C DMA configuration */
	if (I2C_InitStruct->I2CDMAMod == I2C_DMA_REGISTER) {
		I2C_DmaMode1Config(I2Cx, I2C_BIT_DMODE_ENABLE, obj->ptxbuf.datalen);
	}

	while ((I2Cx->IC_RAW_INTR_STAT & I2C_BIT_RD_REQ) == 0);
	I2C_DMAControl(I2Cx, I2C_BIT_TDMAE, ENABLE);
	return HAL_OK;
}

HAL_Status
RtkI2CDmaSend(i2c_dma_t *obj, uint16_t devaddr)
{
	uint32_t i2c_idx = obj->I2Cint.i2c_idx;
	I2C_InitTypeDef   *I2C_InitStruct = &I2CInitData[i2c_idx];
	I2C_TypeDef *I2Cx = obj->I2Cint.I2Cx;

	printf("%s\n", __func__);

	/* Check if it's Master Mode */
	if (I2C_InitStruct->I2CMaster == I2C_MASTER_MODE) {

		/* Master run-time update  target address */
		if (I2C_InitStruct->I2CAckAddr != devaddr) {
			while (1) {
				/* Check Master activity status is 0 && TX FIFO status empty */
				if (!I2C_CheckFlagState(I2Cx, I2C_BIT_ACTIVITY) &&
					I2C_CheckFlagState(I2Cx, I2C_BIT_TFE)) {
					break;
				}
			}
			/* Update Master Target address */
			I2C_SetSlaveAddress(I2Cx, devaddr);
		}
		RtkI2CSendDmaMaster(obj);
	}/* if (I2C_InitStruct->I2CMaster == I2C_MASTER_MODE) */
	else {
		RtkI2CSendDmaSlave(obj);
	}

	return HAL_OK;
}

static HAL_Status
RtkI2CReceiveDmaMaster(
	IN  i2c_dma_t *obj
)
{
	uint32_t i2c_idx = obj->I2Cint.i2c_idx;
	I2C_InitTypeDef   *I2C_InitStruct = &I2CInitData[i2c_idx];
	PGDMA_InitTypeDef   GDMA_InitStruct = &obj->I2CRxGdmaInitStruct;
	I2C_TypeDef *I2Cx = obj->I2Cint.I2Cx;

	printf("%s\n", __func__);

	I2C_RXGDMA_Init(i2c_idx, GDMA_InitStruct, obj,
					(IRQ_FUN)(I2CRXGDMAISRHandle), obj->prxbuf.pbuf, obj->prxbuf.datalen);

	/*I2C Enable RX Related Interrupts*/
	I2C_INTConfig(I2Cx, (I2C_BIT_M_RX_OVER |
						 I2C_BIT_M_RX_UNDER),
				  ENABLE);

	if (I2C_InitStruct->I2CDMAMod == I2C_DMA_REGISTER) {
		I2C_DmaMode1Config(I2Cx, I2C_BIT_DMODE_ENABLE | I2C_BIT_DMODE_STOP | I2C_BIT_DMODE_CMD,
						   obj->prxbuf.datalen);
	}

	I2C_DMAControl(I2Cx, I2C_BIT_RDMAE, ENABLE);
	return HAL_OK;
}


HAL_Status
RtkI2CReceiveDmaSlave(
	IN i2c_dma_t *obj
)
{
	uint32_t i2c_idx = obj->I2Cint.i2c_idx;
	I2C_InitTypeDef   *I2C_InitStruct = &I2CInitData[i2c_idx];
	PGDMA_InitTypeDef   GDMA_InitStruct = &obj->I2CRxGdmaInitStruct;
	I2C_TypeDef *I2Cx = obj->I2Cint.I2Cx;

	printf("%s\n", __func__);

	if (obj->prxbuf.datalen <= I2C_DMA_DATA_RX_LENGTH) {
		I2C_RXGDMA_Init(i2c_idx, GDMA_InitStruct, obj,
						(IRQ_FUN)(I2CRXGDMAISRHandle), obj->prxbuf.pbuf, obj->prxbuf.datalen);
	} else {
		I2C_RXGDMA_Init(i2c_idx, GDMA_InitStruct, obj,
						(IRQ_FUN)(I2CRXGDMAISRHandle), obj->prxbuf.pbuf, I2C_DMA_DATA_RX_LENGTH);
	}

	/*I2C Enable RX Related Interrupts*/
	I2C_INTConfig(I2Cx, (I2C_BIT_M_RX_OVER |
						 I2C_BIT_M_RX_UNDER),
				  ENABLE);

	if (I2C_InitStruct->I2CDMAMod == I2C_DMA_REGISTER) {
		I2C_DmaMode1Config(I2Cx, I2C_BIT_DMODE_ENABLE, obj->prxbuf.datalen);
	}

	while ((I2Cx->IC_RAW_INTR_STAT & I2C_BIT_RX_FULL) == 0);
	I2C_DMAControl(I2Cx, I2C_BIT_RDMAE, ENABLE);
	return HAL_OK;
}

HAL_Status RtkI2CDmaReceive(IN i2c_dma_t *obj, uint16_t devaddr)
{
	uint32_t i2c_idx = obj->I2Cint.i2c_idx;
	I2C_InitTypeDef   *I2C_InitStruct = &I2CInitData[i2c_idx];
	I2C_TypeDef *I2Cx = obj->I2Cint.I2Cx;

	if (I2C_InitStruct->I2CMaster == I2C_MASTER_MODE) {
		/* Master run-time update  target address */
		if (I2C_InitStruct->I2CAckAddr != devaddr) {
			while (1) {
				if (!I2C_CheckFlagState(I2Cx, I2C_BIT_ACTIVITY) &&
					I2C_CheckFlagState(I2Cx, I2C_BIT_TFE)) {
					break;
				}
			}
			I2C_SetSlaveAddress(I2Cx, devaddr);
		}

		RtkI2CReceiveDmaMaster(obj);
	}/*if (I2C_InitStruct->I2CMaster == I2C_MASTER_MODE)*/
	else {
		RtkI2CReceiveDmaSlave(obj);
	}

	return HAL_OK;
}

void RtkI2CInit(i2c_dma_t *obj, PinName sda, PinName scl)
{
	uint32_t i2c_idx = obj->I2Cint.i2c_idx;

	obj->I2Cint.I2Cx = I2C_DEV_TABLE[i2c_idx].I2Cx;

	/* Set I2C Device Number */
	I2CInitData[i2c_idx].I2CIdx = i2c_idx;
	I2CInitData[i2c_idx].I2CDMAMod = I2C_DMA_REGISTER;

	/* I2C Pin Mux Initialization */
	if (i2c_idx == I2C_0) {
		Pinmux_Config(sda, PINMUX_FUNCTION_I2C0_SDA);
		Pinmux_Config(scl, PINMUX_FUNCTION_I2C0_SCL);
	} else {
		Pinmux_Config(sda, PINMUX_FUNCTION_I2C1_SDA);
		Pinmux_Config(scl, PINMUX_FUNCTION_I2C1_SCL);
	}

	PAD_PullCtrl(sda, GPIO_PuPd_UP);
	PAD_PullCtrl(scl, GPIO_PuPd_UP);

	/* I2C HAL Initialization */
	I2C_Init(obj->I2Cint.I2Cx, &I2CInitData[i2c_idx]);

	/* I2C Enable Module */
	I2C_Cmd(obj->I2Cint.I2Cx, ENABLE);
}

u32 dma_tx_callback(void *data)
{
	i2c_dma_t *obj = (i2c_dma_t *)data;

	printf("DMA Tx Done!!\r\n");

	for (int i = 0; i < obj->ptxbuf.datalen; i++) {
		printf("%02x ", *(obj->ptxbuf.pbuf + i));
	}
	printf("\n");

	return 0;
}

u32 dma_rx_callback(void *data)
{
	i2c_dma_t *obj = (i2c_dma_t *)data;

	printf("DMA Rx Done!!\r\n");

	for (int i = 0; i < obj->prxbuf.datalen; i++) {
		printf("%02x ", *(obj->prxbuf.pbuf + i));
	}
	printf("\n");

	return 0;
}




#if MASTER_SEND
void i2c_dma_test(void)
{
	rtos_time_delay_ms(3000);
	RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_I2C1, APBPeriph_I2C1_CLOCK, ENABLE);

	int i2clocalcnt;

	// prepare for transmission
	_memset(&i2cdatasrc[0], 0x00, I2C_DATA_LENGTH);
	_memset(&i2cdatadst[0], 0x00, I2C_DATA_LENGTH);

	for (i2clocalcnt = 0; i2clocalcnt < length; i2clocalcnt++) {
		i2cdatasrc[i2clocalcnt] = i2clocalcnt + 0x2;
	}

	printf("\ni2c dma mode demo\n");

#if I2C_MASTER_DEVICE
	printf("Slave addr=%x\n", MBED_I2C_SLAVE_ADDR0);
	_memset(&i2cdmamaster, 0x00, sizeof(i2c_dma_t));
//	i2c_StructInit(&i2cdmamaster.I2Cint, I2C_0, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL, I2C_MASTER_MODE);
//
	i2cdmamaster.I2Cint.i2c_idx = I2C_0;

	I2C_StructInit(&I2CInitData[i2cdmamaster.I2Cint.i2c_idx]);
//
	I2CInitData[i2cdmamaster.I2Cint.i2c_idx].I2CAckAddr	= MBED_I2C_SLAVE_ADDR0;
	I2CInitData[i2cdmamaster.I2Cint.i2c_idx].I2CMaster = I2C_MASTER_MODE;
	I2CInitData[i2cdmamaster.I2Cint.i2c_idx].I2CSpdMod = I2C_SS_MODE;

	RtkI2CInit(&i2cdmamaster, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL);
	I2C_Init(i2cdmamaster.I2Cint.I2Cx, &I2CInitData[i2cdmamaster.I2Cint.i2c_idx]);

	I2C_Cmd(i2cdmamaster.I2Cint.I2Cx, ENABLE);
	// Master write - Slave read
	printf("\r\nMaster write>>>\n");

	i2cdmamaster.ptxbuf.pbuf = i2cdatasrc;
	i2cdmamaster.ptxbuf.datalen = length;
	i2cdmamaster.user_tx_cb = (IRQ_FUN)dma_tx_callback;

//	i2c_Write(&i2cdmamaster.I2Cint, MBED_I2C_SLAVE_ADDR0, &i2cdatasrc[0], I2C_DATA_LENGTH, 1);

	RtkI2CDmaSend(&i2cdmamaster, MBED_I2C_SLAVE_ADDR0);

#else //I2C_SLAVE_DEVICE
	printf("Slave addr=%x\n", MBED_I2C_SLAVE_ADDR0);
	_memset(&i2cdmaslave, 0x00, sizeof(i2c_dma_t));

	i2cdmaslave.I2Cint.i2c_idx = I2C_0;
	I2C_StructInit(&I2CInitData[i2cdmaslave.I2Cint.i2c_idx]);
	I2CInitData[i2cdmaslave.I2Cint.i2c_idx].I2CAckAddr	= MBED_I2C_SLAVE_ADDR0;
	I2CInitData[i2cdmaslave.I2Cint.i2c_idx].I2CMaster = I2C_SLAVE_MODE;
	I2CInitData[i2cdmamaster.I2Cint.i2c_idx].I2CSpdMod = I2C_SS_MODE;

	RtkI2CInit(&i2cdmaslave, MBED_I2C_SLV_SDA, MBED_I2C_SLV_SCL);

	// Master write - Slave read
	printf("\r\nSlave read>>>\n");

	i2cdmaslave.prxbuf.pbuf = i2cdatadst;
	i2cdmaslave.prxbuf.datalen = length;
	i2cdmaslave.user_rx_cb = (IRQ_FUN)dma_rx_callback;

	RtkI2CDmaReceive(&i2cdmaslave, MBED_I2C_SLAVE_ADDR0);

#endif // #ifdef I2C_SLAVE_DEVICE

	while (1);
}

#else

void i2c_dma_test(void)
{
	RCC_PeriphClockCmd(APBPeriph_I2C0, APBPeriph_I2C0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_I2C1, APBPeriph_I2C1_CLOCK, ENABLE);
	int i2clocalcnt;

	rtos_time_delay_ms(3000);

	// prepare for transmission
	_memset(&i2cdatasrc[0], 0x00, I2C_DATA_LENGTH);
	_memset(&i2cdatadst[0], 0x00, I2C_DATA_LENGTH);

	for (i2clocalcnt = 0; i2clocalcnt < length; i2clocalcnt++) {
		i2cdatasrc[i2clocalcnt] = i2clocalcnt + 0x2;
	}

	printf("\ni2c dma mode demo\n");

#if I2C_MASTER_DEVICE
	printf("Slave addr=%x\n", MBED_I2C_SLAVE_ADDR0);
	_memset(&i2cdmamaster, 0x00, sizeof(i2c_dma_t));

	i2cdmamaster.I2Cint.i2c_idx = I2C_0;

	I2C_StructInit(&I2CInitData[i2cdmamaster.I2Cint.i2c_idx]);

	I2CInitData[i2cdmamaster.I2Cint.i2c_idx].I2CAckAddr =  MBED_I2C_SLAVE_ADDR0;
	I2CInitData[i2cdmamaster.I2Cint.i2c_idx].I2CMaster = I2C_MASTER_MODE;
	I2CInitData[i2cdmamaster.I2Cint.i2c_idx].I2CSpdMod = I2C_SS_MODE;

	RtkI2CInit(&i2cdmamaster, MBED_I2C_MTR_SDA, MBED_I2C_MTR_SCL);

	// Master read - Slave write
	printf("\r\nMaster read>>>\n");

	i2cdmamaster.prxbuf.pbuf = i2cdatadst;
	i2cdmamaster.prxbuf.datalen = length;
	i2cdmamaster.user_rx_cb = (IRQ_FUN)dma_rx_callback;

	RtkI2CDmaReceive(&i2cdmamaster, MBED_I2C_SLAVE_ADDR0);

#else //I2C_SLAVE_DEVICE
	printf("Slave addr=%x\n", MBED_I2C_SLAVE_ADDR0);
	_memset(&i2cdmaslave, 0x00, sizeof(i2c_dma_t));

	i2cdmaslave.I2Cint.i2c_idx = I2C_0;

	I2C_StructInit(&I2CInitData[i2cdmaslave.I2Cint.i2c_idx]);

	I2CInitData[i2cdmaslave.I2Cint.i2c_idx].I2CMaster = I2C_SLAVE_MODE;
	I2CInitData[i2cdmaslave.I2Cint.i2c_idx].I2CAckAddr =  MBED_I2C_SLAVE_ADDR0;
	I2CInitData[i2cdmaslave.I2Cint.i2c_idx].I2CSpdMod = I2C_SS_MODE;

	RtkI2CInit(&i2cdmaslave, MBED_I2C_SLV_SDA, MBED_I2C_SLV_SCL);

	// Master read - Slave write
	printf("\r\nSlave write>>>\n");

	i2cdmaslave.ptxbuf.pbuf = i2cdatasrc;
	i2cdmaslave.ptxbuf.datalen = length;
	i2cdmaslave.user_tx_cb = (IRQ_FUN)dma_tx_callback;

	RtkI2CDmaSend(&i2cdmaslave, MBED_I2C_SLAVE_ADDR0);

#endif // #ifdef I2C_SLAVE_DEVICE

	while (1);
}

#endif

int main(void)
{
	if (rtos_task_create(NULL, "I2C DMA DEMO", (rtos_task_t)i2c_dma_test, NULL, (2048), (1)) != SUCCESS) {
		printf("Cannot create i2c_dma_test demo task\n\r");
	}

	rtos_sched_start();
	return 0;
}


