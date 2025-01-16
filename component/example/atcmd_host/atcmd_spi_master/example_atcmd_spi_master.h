#ifndef EXAMPLE_ATCMD_SPI_MASTER_H
#define EXAMPLE_ATCMD_SPI_MASTER_H

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
#endif

#define HOST_UART_BAUDRATE 38400

void example_atcmd_spi_master(void);

#endif /* EXAMPLE_HTTPC_H */