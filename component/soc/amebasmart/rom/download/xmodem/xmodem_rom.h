/*
	X-Modem Header File

	1999/09/03	sprite, support Xmode Tx & Rx
*/

#ifndef	_XMODE_H_
#define	_XMODE_H_

#include <basic_types.h>

/*****************
 * X-Modem status
 *****************/
#define	XMODEM_OK		1
#define	XMODEM_CANCEL	2
#define	XMODEM_ACK		3
#define	XMODEM_NAK		4
#define	XMODEM_COMPLETE	5
#define	XMODEM_NO_SESSION	6
#define	XMODEM_ABORT	7
#define	XMODEM_TIMEOUT	8

/*****************
 * X-Modem result
 *****************/
#define XM_RESULT_OK	0	/* Xmodem download OK */
#define XM_RESULT_CAN	1	/* Xmodem download cancelled */
#define XM_RESULT_USB	2	/* Switch to USB download mode */

/****************************
 * flow control character
 ****************************/
#define	SOH			0x01		/* Start of header */
#define	STX			0x02		/* Start of header XModem-1K */
#define	EOT			0x04		/* End of transmission */
#define	ACK			0x06		/* Acknowledge */
#define	NAK			0x15		/* Not acknowledge */
#define	CAN			0x18		/* Cancel */
#define	ESC			0x1b		/* User Break */
#define	BAUDSET		0x05		/* Part of shakehands, added by Realtek*/
#define	BAUDCHK	0x07		/* Part of shakehands, added by Realtek*/
#define	XMERASE	0x17		/* Erase Flash, added by Realtek*/
#define	XMREAD		0x19		/* Read Flash, added by Realtek*/
#define	XMREADV2	0x20		/* Read Flash, added by Realtek*/
#define	RXSTATUS	0x21		/* Read Status Register, added by Realtek*/
#define	TXSTATUS	0x26		/* Write Status Register, added by Realtek, AZ ACUT ROM not support*/
#define	XM_CHECKSUM	0x27		/* check flash write checksum: AZ ACUT ROM not support */
#define	XM_TXREG	0x29		/* Write REG or RAM, AZ ACUT ROM not support */
#define	XM_RXREG	0x31		/* Read REG or RAM, AZ ACUT ROM not support */
#define	XM_ROMVER	0x33		/* read rom code version(02/03/04...): AZ ACUT ROM not support */

/****************************
 * Xmode paramters
 ****************************/
#define	FRAME_SIZE	132							/* X-modem structure */
#define	FRAME_SIZE_1K	1028						/* X-modem structure */
#define	XM_BUFFER_SIZE	1024						/* X-modem buffer */
/*read loguart register cost 349.44ns, KM4 is 240MHz(NPPLL/4)*/
#define	WAIT_FRAME_TIME     (2862 * 1000) 			/* 1 sec, wait frame timeout */
#define	WAIT_CHAR_TIME      (1431 * 1000)			/* 0.5 sec, wait char timeout */
#define	WAIT_HANDSHAKE_TIME	(5724 * 1000)			/* 2 sec, handshake timeout */
#define	XMODEM_READ_MAXRETRANS		25
#define	FRAME_BUF_ADDR		(char*)(MSP_RAM_HP_NS + 4 - 16 * 1024)

/***********************
 * frame structure
 ***********************/
typedef	struct {
	unsigned char	soh;
	unsigned char	recordNo;
	unsigned char	recordNoInverted;
	unsigned char	buffer[XM_BUFFER_SIZE];
	unsigned char	CRC;
} XMODEM_FRAME;

typedef struct _XMODEM_CTRL_ {
	u16 currentFrame; /* current frame number */
	u16 expected;
	s32 rFinish;
	u32 total_frame;
} XMODEM_CTRL, *PXMODEM_CTRL;

u32 xmodem_img_rxbuffer(u32 baud_rate);

extern u32 usbd_enabled;

/* wll define in section header later */
#define FWU_DATA_SECTION

#endif /* _XMODE_H_ */

