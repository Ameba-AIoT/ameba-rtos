#ifndef SDIO_HOST_STM32_COMMON_H
#define SDIO_HOST_STM32_COMMON_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

typedef GPIO_TypeDef  		rtw_gpio_port_t;
typedef SemaphoreHandle_t  	rtw_semaphore_type_t;


typedef struct {
	rtw_gpio_port_t *port;
	uint8_t          pin_number;
} rtw_gpio_t;

typedef enum {
	BUS_FUNCTION       = 0,
	BACKPLANE_FUNCTION = 1,
	WLAN_FUNCTION      = 2
} rtw_sdio_function_t;

typedef enum {
	RESPONSE_NEEDED,
	NO_RESPONSE
} rtw_sdio_response_needed_t;

typedef enum {
	SDIO_BLOCK_MODE = (0 << 2),   /* These are STM32 implementation specific */
	SDIO_BYTE_MODE  = (1 << 2)    /* These are STM32 implementation specific */
} rtw_sdio_transfer_mode_t;

typedef enum {
	/* If updating this enum, the bus_direction_mapping variable will also need to be updated */
	BUS_READ,
	BUS_WRITE
} rtw_sdio_transfer_direction_t;

typedef enum {
	RTW_FALSE = 0,
	RTW_TRUE  = 1
} rtw_bool_t;

typedef enum {
	SDIO_CMD_0  =  0,
	SDIO_CMD_3  =  3,
	SDIO_CMD_5  =  5,
	SDIO_CMD_7  =  7,
	SDIO_CMD_52 = 52,
	SDIO_CMD_53 = 53,
	__MAX_VAL   = 64
} rtw_sdio_command_t;

typedef enum {
	PIN_SDIO_CLK,
	PIN_SDIO_CMD,
	PIN_SDIO_D0,
	PIN_SDIO_D1,
	PIN_SDIO_D2,
	PIN_SDIO_D3,
	PIN_SDIO_MAX
} rtw_sdio_pin_t;

typedef enum {
	SDIO_1B_BLOCK    =  1,
	SDIO_2B_BLOCK    =  2,
	SDIO_4B_BLOCK    =  4,
	SDIO_8B_BLOCK    =  8,
	SDIO_16B_BLOCK   =  16,
	SDIO_32B_BLOCK   =  32,
	SDIO_64B_BLOCK   =  64,
	SDIO_128B_BLOCK  =  128,
	SDIO_256B_BLOCK  =  256,
	SDIO_512B_BLOCK  =  512,
	SDIO_1024B_BLOCK = 1024,
	SDIO_2048B_BLOCK = 2048
} rtw_sdio_block_size_t;

/* SDIO Device CCCR offsets */
/* TODO: What does CIS/CCCR stand for? */
/* CCCR accesses do not require backpane clock */
#define SDIOD_CCCR_REV             ( (uint32_t)  0x00 )    /* CCCR/SDIO Revision */
#define SDIOD_CCCR_SDREV           ( (uint32_t)  0x01 )    /* SD Revision */
#define SDIOD_CCCR_IOEN            ( (uint32_t)  0x02 )    /* I/O Enable */
#define SDIOD_CCCR_IORDY           ( (uint32_t)  0x03 )    /* I/O Ready */
#define SDIOD_CCCR_INTEN           ( (uint32_t)  0x04 )    /* Interrupt Enable */
#define SDIOD_CCCR_INTPEND         ( (uint32_t)  0x05 )    /* Interrupt Pending */
#define SDIOD_CCCR_IOABORT         ( (uint32_t)  0x06 )    /* I/O Abort */
#define SDIOD_CCCR_BICTRL          ( (uint32_t)  0x07 )    /* Bus Interface control */
#define SDIOD_CCCR_CAPABLITIES     ( (uint32_t)  0x08 )    /* Card Capabilities */
#define SDIOD_CCCR_CISPTR_0        ( (uint32_t)  0x09 )    /* Common CIS Base Address Pointer Register 0 (LSB) */
#define SDIOD_CCCR_CISPTR_1        ( (uint32_t)  0x0A )    /* Common CIS Base Address Pointer Register 1 */
#define SDIOD_CCCR_CISPTR_2        ( (uint32_t)  0x0B )    /* Common CIS Base Address Pointer Register 2 (MSB - only bit 1 valid)*/
#define SDIOD_CCCR_BUSSUSP         ( (uint32_t)  0x0C )    /*  */
#define SDIOD_CCCR_FUNCSEL         ( (uint32_t)  0x0D )    /*  */
#define SDIOD_CCCR_EXECFLAGS       ( (uint32_t)  0x0E )    /*  */
#define SDIOD_CCCR_RDYFLAGS        ( (uint32_t)  0x0F )    /*  */
#define SDIOD_CCCR_BLKSIZE_0       ( (uint32_t)  0x10 )    /* Function 0 (Bus) SDIO Block Size Register 0 (LSB) */
#define SDIOD_CCCR_BLKSIZE_1       ( (uint32_t)  0x11 )    /* Function 0 (Bus) SDIO Block Size Register 1 (MSB) */
#define SDIOD_CCCR_POWER_CONTROL   ( (uint32_t)  0x12 )    /* Power Control */
#define SDIOD_CCCR_SPEED_CONTROL   ( (uint32_t)  0x13 )    /* Bus Speed Select  (control device entry into high-speed clocking mode)  */
#define SDIOD_CCCR_UHS_I           ( (uint32_t)  0x14 )    /* UHS-I Support */
#define SDIOD_CCCR_DRIVE           ( (uint32_t)  0x15 )    /* Drive Strength */
#define SDIOD_CCCR_INTEXT          ( (uint32_t)  0x16 )    /* Interrupt Extension */
#define SDIOD_SEP_INT_CTL          ( (uint32_t)  0xF2 )    /* Separate Interrupt Control*/
#define SDIOD_CCCR_F1INFO          ( (uint32_t) 0x100 )    /* Function 1 (Backplane) Info */
#define SDIOD_CCCR_F1HP            ( (uint32_t) 0x102 )    /* Function 1 (Backplane) High Power */
#define SDIOD_CCCR_F1CISPTR_0      ( (uint32_t) 0x109 )    /* Function 1 (Backplane) CIS Base Address Pointer Register 0 (LSB) */
#define SDIOD_CCCR_F1CISPTR_1      ( (uint32_t) 0x10A )    /* Function 1 (Backplane) CIS Base Address Pointer Register 1       */
#define SDIOD_CCCR_F1CISPTR_2      ( (uint32_t) 0x10B )    /* Function 1 (Backplane) CIS Base Address Pointer Register 2 (MSB - only bit 1 valid) */
#define SDIOD_CCCR_F1BLKSIZE_0     ( (uint32_t) 0x110 )    /* Function 1 (Backplane) SDIO Block Size Register 0 (LSB) */
#define SDIOD_CCCR_F1BLKSIZE_1     ( (uint32_t) 0x111 )    /* Function 1 (Backplane) SDIO Block Size Register 1 (MSB) */
#define SDIOD_CCCR_F2INFO          ( (uint32_t) 0x200 )    /* Function 2 (WLAN Data FIFO) Info */
#define SDIOD_CCCR_F2HP            ( (uint32_t) 0x202 )    /* Function 2 (WLAN Data FIFO) High Power */
#define SDIOD_CCCR_F2CISPTR_0      ( (uint32_t) 0x209 )    /* Function 2 (WLAN Data FIFO) CIS Base Address Pointer Register 0 (LSB) */
#define SDIOD_CCCR_F2CISPTR_1      ( (uint32_t) 0x20A )    /* Function 2 (WLAN Data FIFO) CIS Base Address Pointer Register 1       */
#define SDIOD_CCCR_F2CISPTR_2      ( (uint32_t) 0x20B )    /* Function 2 (WLAN Data FIFO) CIS Base Address Pointer Register 2 (MSB - only bit 1 valid) */
#define SDIOD_CCCR_F2BLKSIZE_0     ( (uint32_t) 0x210 )    /* Function 2 (WLAN Data FIFO) SDIO Block Size Register 0 (LSB) */
#define SDIOD_CCCR_F2BLKSIZE_1     ( (uint32_t) 0x211 )    /* Function 2 (WLAN Data FIFO) SDIO Block Size Register 1 (MSB) */
#define SDIOD_CCCR_F3INFO          ( (uint32_t) 0x300 )    /* Function 3 (Bluetooth Data FIFO) Info */
#define SDIOD_CCCR_F3HP            ( (uint32_t) 0x302 )    /* Function 3 (Bluetooth Data FIFO) High Power */
#define SDIOD_CCCR_F3CISPTR_0      ( (uint32_t) 0x309 )    /* Function 3 (Bluetooth Data FIFO) CIS Base Address Pointer Register 0 (LSB) */
#define SDIOD_CCCR_F3CISPTR_1      ( (uint32_t) 0x30A )    /* Function 3 (Bluetooth Data FIFO) CIS Base Address Pointer Register 1       */
#define SDIOD_CCCR_F3CISPTR_2      ( (uint32_t) 0x30B )    /* Function 3 (Bluetooth Data FIFO) CIS Base Address Pointer Register 2 (MSB - only bit 1 valid) */
#define SDIOD_CCCR_F3BLKSIZE_0     ( (uint32_t) 0x310 )    /* Function 3 (Bluetooth Data FIFO) SDIO Block Size Register 0 (LSB) */
#define SDIOD_CCCR_F3BLKSIZE_1     ( (uint32_t) 0x311 )    /* Function 3 (Bluetooth Data FIFO) SDIO Block Size Register 1 (MSB) */

/* SDIOD_CCCR_SPEED_CONTROL Bits */
#define SDIO_SPEED_SHS             ( (uint32_t)  0x01 )    /* supports high-speed [clocking] mode (RO) */
#define SDIO_SPEED_EHS             ( (uint32_t)  0x02 )    /* enable high-speed [clocking] mode (RW) */

/* SDIOD_CCCR_IOEN Bits */
#define SDIO_FUNC_ENABLE_1         ( (uint32_t)  0x02 )    /* function 1 I/O enable */

/* SDIOD_CCCR_BICTRL Bits */
#define BUS_CARD_DETECT_DIS        ( (uint32_t)  0x80 )    /* Card Detect disable */
#define BUS_SPI_CONT_INTR_CAP      ( (uint32_t)  0x40 )    /* support continuous SPI interrupt */
#define BUS_SPI_CONT_INTR_EN       ( (uint32_t)  0x20 )    /* continuous SPI interrupt enable */
#define BUS_SD_DATA_WIDTH_MASK     ( (uint32_t)  0x03 )    /* bus width mask */
#define BUS_SD_DATA_WIDTH_4BIT     ( (uint32_t)  0x02 )    /* bus width 4-bit mode */
#define BUS_SD_DATA_WIDTH_1BIT     ( (uint32_t)  0x00 )    /* bus width 1-bit mode */

/* SDIOD_CCCR_IORDY Bits */
#define SDIO_FUNC_READY_1          ( (uint32_t)  0x02 )    /* function 1 I/O ready */
#define SDIO_FUNC_READY_2          ( (uint32_t)  0x04 )    /* function 2 I/O ready */
#define SDIO_FUNC_READY_3          ( (uint32_t)  0x08 )    /* function 3 I/O ready */

/* SDIOD_CCCR_INTEN Bits */
#define INTR_CTL_MASTER_EN         ( (uint32_t)  0x01 )    /* interrupt enable master */
#define INTR_CTL_FUNC1_EN          ( (uint32_t)  0x02 )    /* interrupt enable for function 1 */
#define INTR_CTL_FUNC2_EN          ( (uint32_t)  0x04 )    /* interrupt enable for function 2 */

//READ-ONLY for driver, Modify by FW
#define SPDIO_REG_TXBD_NUM					0x1A4
#define SPDIO_REG_TXBD_WPTR					0x1A8
#define SPDIO_REG_TXBD_RPTR					0x1AC
#define SPDIO_REG_RXBD_NUM					0x1B4 // 2 bytes
#define SPDIO_REG_TXBUF_UNIT_SZ				0x1D9 // 1 bytes

typedef enum {
	RTW_SUCCESS = 0,
	RTW_PENDING,
	RTW_TIMEOUT,
	RTW_BADARG,
	RTW_UNFINISHED,
	RTW_SEMAPHORE_ERROR,
	RTW_BUS_READ_REGISTER_ERROR,
	RTW_SDIO_RETRIES_EXCEEDED
} rtw_result_t;


#ifndef ALIGNED
#define ALIGNED_PRE(size)
#define ALIGNED(size)    __attribute__((aligned(size)))
#endif

#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(x) /*@-noeffect@*/ ( (void)(x) ) /*@+noeffect@*/
#endif

#define rtw_assert( error_string, assertion )         do { (void)(assertion); } while(0)

#define RTW_RTOS_DEFINE_ISR(function) \
        void function(void); \
        __attribute__((used)) void function(void)

#define RTW_RTOS_MAP_ISR(function, irq_handler) \
        extern void irq_handler(void); \
        __attribute__((alias(#function))) void irq_handler (void);

#define RTW_VERIFY_RESULT(x)                                                       \
{                                                                              \
	rtw_result_t verify_result;                                                \
	verify_result = (x);                                                       \
	if (verify_result != RTW_SUCCESS) {                                            \
		printf("fail: func=%s line=%d\n", __FUNCTION__, __LINE__); \
		return verify_result;                                                  \
	}                                                                          \
}

void stm32_init_infrastructure(void);
uint8_t stm32_gpio_get_port_number(rtw_gpio_port_t *gpio_port);
rtw_result_t stm32_powersave_disable(void);
rtw_result_t stm32_powersave_enable(void);
rtw_result_t rtw_sdio_cmd52(rtw_sdio_transfer_direction_t direction, rtw_sdio_function_t function, uint32_t address, uint8_t value,
							rtw_sdio_response_needed_t response_expected, uint8_t *response);
rtw_result_t rtw_sdio_cmd53(rtw_sdio_transfer_direction_t direction, rtw_sdio_function_t function, rtw_sdio_transfer_mode_t mode, uint32_t address,
							uint16_t data_size, uint8_t *data, rtw_sdio_response_needed_t response_expected, uint32_t *response);
rtw_result_t rtw_sdio_transfer(rtw_sdio_transfer_direction_t direction, rtw_sdio_function_t function, uint32_t address, uint16_t data_size, uint8_t *data,
							   rtw_sdio_response_needed_t response_expected);
rtw_result_t rtw_write_register_value(rtw_sdio_function_t function, uint32_t address, uint8_t value_length, uint32_t value);
rtw_result_t rtw_read_register_value(rtw_sdio_function_t function, uint32_t address, uint8_t value_length, uint8_t *value);
rtw_result_t rtw_rtos_delay_milliseconds(uint32_t num_ms);
rtw_result_t rtw_rtos_set_semaphore(rtw_semaphore_type_t *semaphore, rtw_bool_t called_from_ISR);
rtw_result_t rtw_rtos_get_semaphore(rtw_semaphore_type_t *semaphore, uint32_t timeout_ms, rtw_bool_t will_set_in_isr);
rtw_result_t rtw_rtos_init_semaphore(rtw_semaphore_type_t *semaphore);
rtw_result_t rtw_rtos_deinit_semaphore(rtw_semaphore_type_t *semaphore);
rtw_result_t stm32_gpio_set_alternate_function(rtw_gpio_port_t *gpio_port, uint8_t pin_number, GPIOOType_TypeDef output_type,
		GPIOPuPd_TypeDef pull_up_down_type, uint8_t alternation_function);
rtw_result_t stm32_gpio_deinit(const rtw_gpio_t *gpio);
rtw_result_t stm32_sdio_transfer(rtw_sdio_transfer_direction_t direction, rtw_sdio_command_t command, rtw_sdio_transfer_mode_t mode,
								 rtw_sdio_block_size_t block_size, uint32_t argument, /*@null@*/ uint32_t *data, uint16_t data_size,
								 rtw_sdio_response_needed_t response_expected, /*@out@*/ /*@null@*/ uint32_t *response);
void Usart_SendString(USART_TypeDef *pUSARTx, char *str, u16 len);
rtw_result_t stm32_sdio_enumerate(void);
rtw_result_t stm32_enable_high_speed_sdio(void);
rtw_result_t stm32_bus_init(void);

#endif