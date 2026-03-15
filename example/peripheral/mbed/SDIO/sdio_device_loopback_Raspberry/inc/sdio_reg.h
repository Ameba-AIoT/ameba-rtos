#ifndef RTK_SDIO_REG_H
#define RTK_SDIO_REG_H


#define SDIO_TX_FIFO_ADDR							0x8000
#define SDIO_RX_FIFO_ADDR 							0xE000

#define SDIO_TX_FW_ADDR 							0xA000

/*********************************************************
*
*SDIO Local registers
*
*********************************************************/
#define SDIO_REG_TX_CTRL							0x00 // 1byte
#define SDIO_REG_STATIS_RECOVERY_TIMOUT				0x02 // 2bytes
#define SDIO_REG_32K_TRANS_IDLE_TIME				0x04 // 2bytes
#define SDIO_REG_HIMR								0x14 // 4bytes
#define SDIO_REG_HISR								0x18 // 4bytes
#define SDIO_REG_RX0_REQ_LEN						0x1c // 4bytes
#define SDIO_REG_FREE_TXBD_NUM						0x20 // 2bytes
#define SDIO_REG_TX_SEQNUM 							0x24 // 1byte, not used
#define SDIO_REG_HCPWM								0x38 // 1byte, host domain, sync from CCPWM
#define SDIO_REG_HCPWM2								0x3a // 2bytes, sync from CCPWM2
#define SDIO_REG_H2C_MSG							0x60 // 4bytes, host domain, sync from CPU_H2C_MSG
#define SDIO_REG_C2H_MSG							0x64 // 4bytes, sync from CPU_C2H_MSG
#define SDIO_REG_H2C_MSG_EXT						0x68 // 4bytes, sync from CPU_H2C_MSG_EXT
#define SDIO_REG_C2H_MSG_EXT						0x6c // 4bytes, sync from CPU_C2H_MSG_EXT
#define SDIO_REG_HRPWM								0x80 // 1byte, driver to FW, host domain, sync to CRPWM
#define SDIO_REG_HRPWM2								0x82 // 2bytes, driver to FW, host domain, sync to CRPWM2
#define SDIO_REG_CPU_IND							0x87 // 1 bytes, firmware indication to host
#define SDIO_REG_CMD_IN2RSP_TIME					0x88 // 2 bytes, command IN to response OUT time
#define SDIO_REG_ERR_FLAG							0xC0 // 1 byte, ERR FLAG for SDIO DEBUG
#define SDIO_REG_CMD_ERRCNT							0xC1 // 1 byte, the number of CRC error of SDIO command
#define SDIO_REG_DATA_ERRCNT						0xC2 // 1 byte, the number of CRC error of SDIO data block
#define SDIO_REG_CRC_ERR_INDEX						0xC3 // 1 byte, indicate there is crc error on D3-D0
#define SDIO_REG_AVAI_BD_NUM_TH_L					0xD0 // 4 bytes, low threshold for TXBD Number
#define SDIO_REG_AVAI_BD_NUM_TH_H					0xD4 // 4 bytes, high threshold for TXBD Number
#define SDIO_REG_RX_AGG_CFG							0xD8 // 2 bytes, SDIO bus Rx Aggregation config register

// only for RL7005 or 6973
#define SDIO_REG_FW_DL_ADDR					0xE0 // 4 bytes, SDIO firmware download address register
#define SDIO_REG_FW_DL_CTRL					0xE4 // 1 bytes, SDIO firmware download control register

//READ-ONLY for driver, Modify by FW
#define SDIO_REG_TXBD_NUM							0x1A4 // 2 bytes
#define SDIO_REG_RXBD_NUM							0x1B4 // 2 bytes
#define SDIO_REG_CPU_INT_MASK						0x1C0 // 2 bytes
#define SDIO_REG_CPU_INT							0x1C2 // 2 bytes
#define SDIO_REG_CCPWM								0x1C4 // 1 bytes, sync with HCPWM
#define SDIO_REG_CCPWM2								0x1C6 // 1 bytes, sync with HCPWM2
#define SDIO_REG_AHB_DMA_CTRL						0x1D4 // 4 bytes
#define SDIO_REG_TXBUF_UNIT_SZ						0x1D9 // 1 bytes
#define SDIO_REG_FREE_RXBD_CNT						0x1DA // 2 bytes


/************************************************
*
*BIT MAP FOR SOME REGISTERS
*
************************************************/
// SDIO_REG_HISR: SDIO Host Interrupt Service Routine
#define SDIO_HISR_RX_REQUEST						(BIT0)
#define SDIO_HISR_AVAL_INT							(BIT1)
#define SDIO_HISR_TXPKT_OVER_BUFF					(BIT2)
#define SDIO_HISR_TX_AGG_SIZE_MISMATCH				(BIT3)
#define SDIO_HISR_TXBD_OVF							(BIT4)
//BIT5~16 not used
#define SDIO_HISR_C2H_MSG_INT						(BIT17)
#define SDIO_HISR_CPWM1								(BIT18)
#define SDIO_HISR_CPWM2								(BIT19)
#define SDIO_HISR_H2C_BUS_FAIL						(BIT20)
//BIT21 not used
#define SDIO_HISR_CPU_NOT_RDY						(BIT22)
//BIT23~31 not used

#define MASK_SDIO_HISR_CLEAR						(SDIO_HISR_TXPKT_OVER_BUFF|\
													SDIO_HISR_TX_AGG_SIZE_MISMATCH|\
													SDIO_HISR_TXBD_OVF|\
													SDIO_HISR_C2H_MSG_INT|\
													SDIO_HISR_CPWM1|\
													SDIO_HISR_CPWM2|\
													SDIO_HISR_H2C_BUS_FAIL|\
													SDIO_HISR_CPU_NOT_RDY)



// RTL8195A SDIO Host Interrupt Mask Register
#define SDIO_HIMR_RX_REQUEST_MSK					(BIT0)
#define SDIO_HIMR_AVAL_MSK							(BIT1)
#define SDIO_HIMR_TXPKT_SIZE_OVER_BUFF_MSK			(BIT2)
#define SDIO_HIMR_AGG_SIZE_MISMATCH_MSK				(BIT3)
#define SDIO_HIMR_TXBD_OVF_MSK						(BIT4)
//BIT5~16 not used
#define SDIO_HIMR_C2H_MSG_MSK						(BIT17)
#define SDIO_HIMR_CPWM1_MSK							(BIT18)
#define SDIO_HIMR_CPWM2_MSK							(BIT19)
#define SDIO_HIMR_H2C_BUS_FAIL_MSK					(BIT20)
//BIT21 not used
#define SDIO_HIMR_CPU_NOT_RDY_MSK					(BIT22)
//BIT23~31 not used

#define MASK_SDIO_HIMR 								(SDIO_HIMR_RX_REQUEST_MSK | \
													SDIO_HIMR_TXBD_OVF_MSK| \
													SDIO_HIMR_AVAL_MSK	| \
													SDIO_HIMR_CPU_NOT_RDY_MSK | \
													SDIO_HIMR_CPWM1_MSK )

#define SDIO_HIMR_DISABLED							0


// Register SDIO_REG_RX0_REQ_LEN
#define SDIO_RX_REQ_LEN_RDY							(BIT31)
#define SDIO_RX_REQ_LEN_MSK							(0xffffff)

// Register REG_SPDIO_CPU_IND
#define SDIO_SYSTEM_TRX_RDY_IND						(BIT0)

//Register SDIO_REG_RX_AGG_CFG
#define SDIO_RX_AGG_EN								(BIT15)

// Register SDIO_REG_HCPWM
#define SDIO_HCPWM_BIT0								(BIT0)
#define SDIO_HCPWM_TOGGLE							(BIT7)

// Register SDIO_REG_FW_DL_CTRL
#define SD_FW_PAGE_DONE								(BIT0)
#define SD_FW_DL_DONE								(BIT1)
#define SD_FW_DL_MODE								(BIT7)

#endif

