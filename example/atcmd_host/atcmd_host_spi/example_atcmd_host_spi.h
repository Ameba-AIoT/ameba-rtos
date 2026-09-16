#ifndef EXAMPLE_ATCMD_HOST_SPI_H
#define EXAMPLE_ATCMD_HOST_SPI_H

#if defined (CONFIG_AMEBASMART)
// SPI1
#define SPI1_MOSI  _PB_19
#define SPI1_MISO  _PB_20
#define SPI1_SCLK  _PB_18
#define SPI1_CS    _PB_17

#define MASTER_SYNC_PIN PA_20
#define SLAVE_SYNC_PIN PA_21

#define HOST_UART_TX PA_3
#define HOST_UART_RX PA_2

#elif defined (CONFIG_AMEBALITE)
// SPI1
#define SPI1_MOSI	_PB_3
#define SPI1_MISO	_PB_4
#define SPI1_SCLK	_PB_2
#define SPI1_CS		_PB_5

#define MASTER_SYNC_PIN PB_2
#define SLAVE_SYNC_PIN PB_3

#define HOST_UART_TX PA_28
#define HOST_UART_RX PA_29

#elif defined (CONFIG_AMEBADPLUS)
// SPI1 FID=8 & Fully PG
#define SPI1_MOSI	_PB_19
#define SPI1_MISO	_PB_20
#define SPI1_SCLK	_PB_18
#define SPI1_CS		_PB_21

#define MASTER_SYNC_PIN PB_30
#define SLAVE_SYNC_PIN PB_31

#define HOST_UART_TX PA_26
#define HOST_UART_RX PA_27

#elif defined (CONFIG_RTL8720F)
// SPI1 fully-programmable pins. Avoid SDIO-device group (PA7-PA12 / PA25-PA30
// in SDIO_PAD), LOGUART (PA19/PA20) and the SPI1-dedicated pads (PB2/PB3/PB4).
#define SPI1_MOSI	_PA_14
#define SPI1_MISO	_PA_15
#define SPI1_SCLK	_PA_13
#define SPI1_CS		_PA_16

#define MASTER_SYNC_PIN PA_17
#define SLAVE_SYNC_PIN PA_18

// HOST_UART goes to UART0; serial_init requires TX/RX on PA_0..PA_30.
#define HOST_UART_TX PA_21
#define HOST_UART_RX PA_22

#else
#define SPI1_MOSI	_PB_19
#define SPI1_MISO	_PB_20
#define SPI1_SCLK	_PB_18
#define SPI1_CS		_PB_21

#define MASTER_SYNC_PIN PB_30
#define SLAVE_SYNC_PIN PB_31

#define HOST_UART_TX PA_26
#define HOST_UART_RX PA_27

#endif

#define HOST_UART_BAUDRATE 38400

void example_atcmd_host_spi(void);

#endif /* EXAMPLE_HTTPC_H */