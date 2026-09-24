#include <string.h> /* For memcpy */
#include "sdio_host_common.h"

/******************************************************
 *             Constants
 ******************************************************/

#define CMD52_TIMEOUT_LOOPS (100000)
#define CMD53_TIMEOUT_LOOPS    (100000)
#define SDIO_ERROR_MASK                      ( SDIO_STA_CCRCFAIL | SDIO_STA_DCRCFAIL | SDIO_STA_CTIMEOUT | SDIO_STA_DTIMEOUT | SDIO_STA_TXUNDERR | SDIO_STA_RXOVERR | SDIO_STA_STBITERR )
#define SDIO_IRQ_CHANNEL                     ((u8)0x31)
#define DMA2_3_IRQ_CHANNEL                   ((u8)DMA2_Stream3_IRQn)
#define BUS_LEVEL_MAX_RETRIES                (5)
#define SDIO_ENUMERATION_TIMEOUT_MS          (500)

/******************************************************
 *             Variables
 ******************************************************/

static const uint32_t bus_direction_mapping[] = {
	[BUS_READ]  = SDIO_TransferDir_ToSDIO,
	[BUS_WRITE] = SDIO_TransferDir_ToCard
};

const rtw_gpio_t atcmd_sdio_pins[] = {
	[PIN_SDIO_CLK    ] = { GPIOC, 12 },
	[PIN_SDIO_CMD    ] = { GPIOD,  2 },
	[PIN_SDIO_D0     ] = { GPIOC,  8 },
	[PIN_SDIO_D1     ] = { GPIOC,  9 },
	[PIN_SDIO_D2     ] = { GPIOC, 10 },
	[PIN_SDIO_D3     ] = { GPIOC, 11 },
};

ALIGNED_PRE(4) static uint8_t       temp_dma_buffer[2 * 1024] ALIGNED(4);
static uint8_t                     *user_data;
static uint32_t                     user_data_size;
static uint8_t                     *dma_data_source;
static uint32_t                     dma_transfer_size;
static rtw_semaphore_type_t        sdio_transfer_finished_semaphore;
static rtw_bool_t                 sdio_transfer_failed;
static rtw_sdio_transfer_direction_t current_transfer_direction;
static uint32_t                     current_command;

static int32_t	stm32f2_clock_needed_counter = 0;
static rtw_semaphore_type_t sdio_transfer_lock;
extern rtw_semaphore_type_t host_irq_sema;

static uint32_t          sdio_get_blocksize_dctrl(rtw_sdio_block_size_t block_size);
static rtw_sdio_block_size_t find_optimal_block_size(uint32_t data_size);
static void              sdio_prepare_data_transfer(rtw_sdio_transfer_direction_t direction, rtw_sdio_block_size_t block_size, /*@unique@*/ uint8_t *data,
		uint16_t data_size) /*@modifies dma_data_source, user_data, user_data_size, dma_transfer_size@*/;

static const uint32_t gpio_peripheral_clocks[8] = {
	[0] = RCC_AHB1Periph_GPIOA,
	[1] = RCC_AHB1Periph_GPIOB,
	[2] = RCC_AHB1Periph_GPIOC,
	[3] = RCC_AHB1Periph_GPIOD,
	[4] = RCC_AHB1Periph_GPIOE,
	[5] = RCC_AHB1Periph_GPIOF,
	[6] = RCC_AHB1Periph_GPIOG,
	[7] = RCC_AHB1Periph_GPIOH,
};

rtw_result_t stm32_gpio_set_alternate_function(rtw_gpio_port_t *gpio_port, uint8_t pin_number, GPIOOType_TypeDef output_type,
		GPIOPuPd_TypeDef pull_up_down_type, uint8_t alternation_function)
{
	GPIO_InitTypeDef  gpio_init_structure;
	uint8_t           port_number = stm32_gpio_get_port_number(gpio_port);

	stm32_powersave_disable();

	/* Enable peripheral clock for this port */
	RCC->AHB1ENR |= gpio_peripheral_clocks[port_number];

	gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_structure.GPIO_Mode  = GPIO_Mode_AF;
	gpio_init_structure.GPIO_OType = output_type;
	gpio_init_structure.GPIO_PuPd  = pull_up_down_type;
	gpio_init_structure.GPIO_Pin   = (uint32_t)(1 << pin_number);

	GPIO_Init(gpio_port, &gpio_init_structure);
	GPIO_PinAFConfig(gpio_port, pin_number, alternation_function);

	stm32_powersave_enable();

	return RTW_SUCCESS;
}

rtw_result_t stm32_gpio_deinit(const rtw_gpio_t *gpio)
{
	GPIO_InitTypeDef  gpio_init_structure;

	rtw_assert("bad argument", (gpio != NULL));

	stm32_powersave_disable();

	/* Set to Input high-impedance */
	gpio_init_structure.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init_structure.GPIO_Mode  = GPIO_Mode_IN;
	gpio_init_structure.GPIO_OType = GPIO_OType_PP; /* arbitrary. not applicable */
	gpio_init_structure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	gpio_init_structure.GPIO_Pin   = (uint32_t)(1 << gpio->pin_number);

	GPIO_Init(gpio->port, &gpio_init_structure);

	stm32_powersave_enable();

	return RTW_SUCCESS;
}

uint8_t stm32_gpio_get_port_number(rtw_gpio_port_t *gpio_port)
{
	switch ((uint32_t) gpio_port) {
	case GPIOA_BASE:
		return EXTI_PortSourceGPIOA;
	case GPIOB_BASE:
		return EXTI_PortSourceGPIOB;
	case GPIOC_BASE:
		return EXTI_PortSourceGPIOC;
	case GPIOD_BASE:
		return EXTI_PortSourceGPIOD;
	case GPIOE_BASE:
		return EXTI_PortSourceGPIOE;
	case GPIOF_BASE:
		return EXTI_PortSourceGPIOF;
	case GPIOG_BASE:
		return EXTI_PortSourceGPIOG;
	case GPIOH_BASE:
		return EXTI_PortSourceGPIOH;
	case GPIOI_BASE:
		return EXTI_PortSourceGPIOI;
	default:
		return 0xff;
	}
}

void stm32_init_rtos_irq_priorities(void)
{
	/* Setup the system handler priorities */
	NVIC_SetPriority(MemoryManagement_IRQn, 0);                                      /* Mem Manage system handler priority    */
	NVIC_SetPriority(BusFault_IRQn, 0);                                              /* Bus Fault system handler priority     */
	NVIC_SetPriority(UsageFault_IRQn, 0);                                            /* Usage Fault system handler priority   */
	NVIC_SetPriority(SVCall_IRQn, 0);                                                /* SVCall system handler priority        */
	NVIC_SetPriority(DebugMonitor_IRQn, 0);                                          /* Debug Monitor system handler priority */
	NVIC_SetPriority(PendSV_IRQn, configKERNEL_INTERRUPT_PRIORITY >> 4);             /* PendSV system handler priority        */
	NVIC_SetPriority(SysTick_IRQn, configKERNEL_INTERRUPT_PRIORITY >> 4);            /* SysTick system handler priority       */
}

void stm32_init_peripheral_irq_priorities(void)
{
	NVIC_SetPriority(SDIO_IRQn,  5);           /* WLAN SDIO           */
	NVIC_SetPriority(DMA2_Stream3_IRQn,  6);   /* WLAN SDIO DMA       */
}

void stm32_init_infrastructure(void)
{
	uint8_t i;

	/* Initialise interrupt priorities */
	for (i = 0; i < 96; i++) {
		NVIC_SetPriority((IRQn_Type) i, 0xf);
	}
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	stm32_init_rtos_irq_priorities();
	stm32_init_peripheral_irq_priorities();
}


static void sdio_enable_bus_irq(void)
{
	SDIO->MASK = SDIO_MASK_SDIOITIE | SDIO_MASK_CMDRENDIE | SDIO_MASK_CMDSENTIE;
}

static void sdio_disable_bus_irq(void)
{
	SDIO->MASK = 0;
}


rtw_result_t stm32_bus_init(void)
{
	SDIO_InitTypeDef sdio_init_structure;
	rtw_result_t     result;
	uint8_t          a;

	stm32_powersave_disable();
	result = rtw_rtos_init_semaphore(&sdio_transfer_finished_semaphore);
	if (result != RTW_SUCCESS) {
		return result;
	}

	result = rtw_rtos_init_semaphore(&sdio_transfer_lock);   // kw
	if (result != RTW_SUCCESS) {
		return result;
	}
	rtw_rtos_set_semaphore(&sdio_transfer_lock, RTW_TRUE);  // kw

	/* Turn on SDIO IRQ */
	SDIO->ICR = (uint32_t) 0xffffffff;

	/* Must be lower priority than the value of configMAX_SYSCALL_INTERRUPT_PRIORITY */
	/* otherwise FreeRTOS will not be able to mask the interrupt */
	/* keep in mind that ARMCM3 interrupt priority logic is inverted, the highest value */
	/* is the lowest priority */
	NVIC_EnableIRQ((IRQn_Type) SDIO_IRQ_CHANNEL);
	NVIC_EnableIRQ((IRQn_Type) DMA2_3_IRQ_CHANNEL);

	/* Setup GPIO pins for SDIO data & clock */
	for (a = PIN_SDIO_CLK; a < PIN_SDIO_MAX; a++) {
		stm32_gpio_set_alternate_function(atcmd_sdio_pins[ a ].port, atcmd_sdio_pins[ a ].pin_number, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF_SDIO);
	}

	/*!< Enable the SDIO AHB Clock and the DMA2 Clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);

	SDIO_DeInit();
	/*!< SDIO_CK = 48MHZ / (SDIO_TRANSFER_CLK_DIV + 2) */
	sdio_init_structure.SDIO_ClockDiv            = (uint8_t) 120; /* 0x78, clock is taken from the high speed APB bus ; */ /* About 400KHz */

	sdio_init_structure.SDIO_ClockEdge           = SDIO_ClockEdge_Rising;
	sdio_init_structure.SDIO_ClockBypass         = SDIO_ClockBypass_Disable;
	sdio_init_structure.SDIO_ClockPowerSave      = SDIO_ClockPowerSave_Enable;
	sdio_init_structure.SDIO_BusWide             = SDIO_BusWide_1b;
	sdio_init_structure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
	SDIO_Init(&sdio_init_structure);
	SDIO_SetPowerState(SDIO_PowerState_ON);
	SDIO_SetSDIOReadWaitMode(SDIO_ReadWaitMode_CLK);
	SDIO_ClockCmd(ENABLE);

	stm32_powersave_enable();

	return RTW_SUCCESS;
}

rtw_result_t stm32_sdio_enumerate(void)
{
	rtw_result_t result;
	uint32_t       loop_count;
	uint32_t       data = 0;

	loop_count = 0;

	do {
		/* Send CMD0 to set it to idle state */
		stm32_sdio_transfer(BUS_WRITE, SDIO_CMD_0, SDIO_BYTE_MODE, SDIO_1B_BLOCK, 0, 0, 0, NO_RESPONSE, NULL);

		/* CMD5. */
		result = stm32_sdio_transfer(BUS_READ, SDIO_CMD_5, SDIO_BYTE_MODE, SDIO_1B_BLOCK, 0, 0, 0, NO_RESPONSE, &data);

		if ((((data >> 28) & 0x7) != 0)) {

			printf("func0 + other %d functions\n", data >> 28 & 0x7);
			result = stm32_sdio_transfer(BUS_READ, SDIO_CMD_5, SDIO_BYTE_MODE, SDIO_1B_BLOCK, (data & 0xffffff), 0, 0, NO_RESPONSE, &data);

			while (!(data & 0x80000000)) {
				result = stm32_sdio_transfer(BUS_READ, SDIO_CMD_5, SDIO_BYTE_MODE, SDIO_1B_BLOCK, (data & 0xffffff), 0, 0, NO_RESPONSE, &data);
			}

		} else {
			printf("only func0 or invalid ocr\n");
		}

		/* Send CMD3 to get RCA. */
		result = stm32_sdio_transfer(BUS_READ, SDIO_CMD_3, SDIO_BYTE_MODE, SDIO_1B_BLOCK, 0, 0, 0, RESPONSE_NEEDED, &data);

		loop_count++;
		if (loop_count >= (uint32_t) SDIO_ENUMERATION_TIMEOUT_MS) {
			return RTW_TIMEOUT;
		}
	} while ((result != RTW_SUCCESS) && (rtw_rtos_delay_milliseconds((uint32_t) 1), (1 == 1)));
	/* If you're stuck here, check the platform matches your hardware */

	/* Send CMD7 with the returned RCA to select the card */
	stm32_sdio_transfer(BUS_WRITE, SDIO_CMD_7, SDIO_BYTE_MODE, SDIO_1B_BLOCK, data, 0, 0, RESPONSE_NEEDED, NULL);

	return RTW_SUCCESS;
}

rtw_result_t stm32_bus_deinit(void)
{
	rtw_result_t result;
	uint32_t     a;

	result = rtw_rtos_deinit_semaphore(&sdio_transfer_finished_semaphore);

	stm32_powersave_disable();

	/* Disable SPI and SPI DMA */
	sdio_disable_bus_irq();
	SDIO_ClockCmd(DISABLE);
	SDIO_SetPowerState(SDIO_PowerState_OFF);
	SDIO_DeInit();
	//RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2, DISABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, DISABLE);

	for (a = 0; a < PIN_SDIO_MAX; a++) {
		stm32_gpio_deinit(&atcmd_sdio_pins[ a ]);
	}

	/* Turn off SDIO IRQ */
	NVIC_DisableIRQ((IRQn_Type) SDIO_IRQ_CHANNEL);
	NVIC_DisableIRQ((IRQn_Type) DMA2_3_IRQ_CHANNEL);

	stm32_powersave_enable();

	return result;
}
int setcounter;
int getcounter;

rtw_result_t stm32_sdio_transfer(rtw_sdio_transfer_direction_t direction, rtw_sdio_command_t command, rtw_sdio_transfer_mode_t mode,
								 rtw_sdio_block_size_t block_size, uint32_t argument, /*@null@*/ uint32_t *data, uint16_t data_size,
								 rtw_sdio_response_needed_t response_expected, /*@out@*/ /*@null@*/ uint32_t *response)
{
	uint32_t loop_count = 0;
	rtw_result_t result;
	uint16_t attempts = 0;
	uint32_t sdio_sta;
	uint32_t count, data0, dataremaining;

	rtw_assert("Bad args", !((command == SDIO_CMD_53) && (data == NULL)));

	while (rtw_rtos_get_semaphore(&sdio_transfer_lock, (uint32_t) 1000, RTW_TRUE) != RTW_SUCCESS) {
		rtw_rtos_delay_milliseconds(10);
		printf("fail get lock %d \n", __LINE__);
	}

	if (response != NULL) {
		*response = 0;
	}

	stm32_powersave_disable();

	/* Ensure the bus isn't stuck half way through transfer */
	DMA2_Stream3->CR   = 0;

restart:
	SDIO->ICR = (uint32_t) 0xFFFFFFFF; // clear all SDIO static flags
	sdio_transfer_failed = RTW_FALSE;
	++attempts;

	/* Check if we've tried too many times */
	if (attempts >= (uint16_t) BUS_LEVEL_MAX_RETRIES) {
		result = RTW_SDIO_RETRIES_EXCEEDED;
		goto exit;
	}

	/* Prepare the data transfer register */
	current_command = command;
	if (command == SDIO_CMD_53) {
		sdio_enable_bus_irq();
		/* Dodgy STM32 hack to set the CMD53 byte mode size to be the same as the block size */
		if (mode == SDIO_BYTE_MODE) {
			block_size = find_optimal_block_size(data_size);
			if (block_size < SDIO_512B_BLOCK) {
				argument = (argument & (uint32_t)(~0x1FF)) | block_size;
			} else {
				argument = (argument & (uint32_t)(~0x1FF));
			}
		}

		/* Prepare the SDIO for a data transfer */
		current_transfer_direction = direction;
		sdio_prepare_data_transfer(direction, block_size, (uint8_t *) data, data_size);

		/* Send the command */
		SDIO->ARG = argument;
		SDIO->CMD = (uint32_t)(command | SDIO_Response_Short | SDIO_Wait_No | SDIO_CPSM_Enable);

		getcounter++;
		result = rtw_rtos_get_semaphore(&sdio_transfer_finished_semaphore, (uint32_t) 500, RTW_TRUE);

		if (result != RTW_SUCCESS) {
			printf("fail %d getcounter %d setcounter %d\n", __LINE__, getcounter, setcounter);
			printf("DMA2 LISR %x\n", (uint32_t)(DMA2->LISR) >> 22);
			printf("DMA2 CR %x\n", DMA2_Stream3->CR);
			printf("DMA2 NDTR %x\n", DMA2_Stream3->NDTR);
			printf("SDIO DCTRL %x\n", SDIO->DCTRL);
			printf("SDIO MASK %x\n", SDIO->MASK);
			printf("SDIO STA %x\n", SDIO->STA);
			printf("SDIO CLKCR %x\n", SDIO->CLKCR);
			printf("SDIO FIFOCNT %x\n", SDIO->FIFOCNT);
			printf("SDIO DCOUNT %x\n", SDIO->DCOUNT);
			goto exit;// fiona
		}

		if (sdio_transfer_failed == RTW_TRUE) {
			printf("fail %d\n", __LINE__);
			goto restart;
		}

		/* Check if there were any SDIO errors */
		if ((SDIO->STA & (SDIO_STA_DTIMEOUT | SDIO_STA_CTIMEOUT)) != 0) {
			printf("fail L%d(DTO/CTO)--> try again\n", __LINE__);
			goto restart;
		} else if (((SDIO->STA & (SDIO_STA_CCRCFAIL | SDIO_STA_DCRCFAIL | SDIO_STA_TXUNDERR | SDIO_STA_RXOVERR)) != 0)) {
			printf("fail L%d(CRC/FIFO) --> try again\n", __LINE__);
			rtw_assert("SDIO communication failure", 0);
			goto restart;
		}

		/* Wait till complete */
		loop_count = (uint32_t) CMD53_TIMEOUT_LOOPS;
		do {
			loop_count--;
			if (loop_count == 0 || ((SDIO->STA & SDIO_ERROR_MASK) != 0)) {
				//printf("wait to %s %d sta %x \r\n", __FUNCTION__, __LINE__, SDIO->STA & SDIO_ERROR_MASK);
				//printf("fail (ERROR and ACT on) %x--> try again\n", SDIO->STA & SDIO_ERROR_MASK);
				goto restart;
			}
		} while ((SDIO->STA & (SDIO_STA_TXACT | SDIO_STA_RXACT)) != 0);       // can write data to txfifo OR has data in rxfifo

		if (direction == BUS_READ) {
			memcpy(user_data, dma_data_source, (size_t) user_data_size);
		}
	} else { // other cmds than cmd53
		uint32_t temp_sta;

		/* Send the command */
		SDIO->ARG = argument;
		SDIO->CMD = (uint32_t)(command | SDIO_Response_Short | SDIO_Wait_No | SDIO_CPSM_Enable);

		loop_count = (uint32_t) CMD52_TIMEOUT_LOOPS;
		do {
			temp_sta = SDIO->STA;
			loop_count--;
			if (loop_count == 0 || ((response_expected == RESPONSE_NEEDED) && ((temp_sta & SDIO_ERROR_MASK) != 0))) {
				goto restart;
			}
		} while ((temp_sta & SDIO_FLAG_CMDACT) != 0);
	}

	if (response != NULL) {
		*response = SDIO->RESP1;
	}
	result = RTW_SUCCESS;

exit:
	stm32_powersave_enable();

	rtw_rtos_set_semaphore(&sdio_transfer_lock, RTW_TRUE);

	return result;
}

static void sdio_prepare_data_transfer(rtw_sdio_transfer_direction_t direction, rtw_sdio_block_size_t block_size, /*@unique@*/ uint8_t *data,
									   uint16_t data_size)  /*@modifies dma_data_source, user_data, user_data_size, dma_transfer_size@*/
{
	/* Setup a single transfer using the temp buffer */
	user_data         = data;
	user_data_size    = data_size;
	dma_transfer_size = (uint32_t)(((data_size + (uint16_t) block_size - 1) / (uint16_t) block_size) * (uint16_t) block_size);

	if (direction == BUS_WRITE) {
		dma_data_source = data;
	} else {
		dma_data_source = temp_dma_buffer;
	}

	SDIO->DTIMER = (uint32_t) 0xFFFFFFFF;
	SDIO->DLEN   = dma_transfer_size;

	// DCTRL[3]: DMAEN
	SDIO->DCTRL  = (uint32_t)sdio_get_blocksize_dctrl(block_size) | bus_direction_mapping[(int)direction] | SDIO_TransferMode_Block | SDIO_DPSM_Enable |
				   (1 << 3) | (1 << 11);

	/* DMA2 Stream3 */
	DMA2_Stream3->CR   = 0;
	DMA2->LIFCR        = (uint32_t)(0x3F << 22);    //clear stream3 status
	DMA2_Stream3->FCR  = (uint32_t)(0x00000021 | DMA_FIFOMode_Enable | DMA_FIFOThreshold_Full);
	DMA2_Stream3->PAR  = (uint32_t) &SDIO->FIFO;
	DMA2_Stream3->M0AR = (uint32_t) dma_data_source;
	DMA2_Stream3->NDTR = 0; // periph as fc

}

rtw_result_t stm32_enable_high_speed_sdio(void)
{
	SDIO_InitTypeDef sdio_init_structure;

#ifdef SLOW_SDIO_CLOCK
	sdio_init_structure.SDIO_ClockDiv       = (uint8_t) 4; /* 1 = 16MHz if SDIO clock = 48MHz */
#else
	sdio_init_structure.SDIO_ClockDiv       = (uint8_t) 0; /* 0 = 24MHz if SDIO clock = 48MHz */ /* a value between 0x00 and 0xFF*/
#endif
	sdio_init_structure.SDIO_ClockEdge      = SDIO_ClockEdge_Rising;
	//sdio_init_structure.SDIO_ClockBypass    = SDIO_ClockBypass_Disable;
	sdio_init_structure.SDIO_ClockBypass    = SDIO_ClockBypass_Enable;  // to use 48M clk
	sdio_init_structure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
#ifndef SDIO_1_BIT
	sdio_init_structure.SDIO_BusWide = SDIO_BusWide_4b;
#else
	sdio_init_structure.SDIO_BusWide = SDIO_BusWide_1b;
#endif
	sdio_init_structure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;

	SDIO_DeInit();
	SDIO_Init(&sdio_init_structure);
	SDIO_SetPowerState(SDIO_PowerState_ON);

	SDIO_ClockCmd(ENABLE);
	sdio_enable_bus_irq();
	return RTW_SUCCESS;
}

static rtw_sdio_block_size_t find_optimal_block_size(uint32_t data_size)
{
	if (data_size > (uint32_t) 256) {
		return SDIO_512B_BLOCK;
	}
	if (data_size > (uint32_t) 128) {
		return SDIO_256B_BLOCK;
	}
	if (data_size > (uint32_t) 64) {
		return SDIO_128B_BLOCK;
	}
	if (data_size > (uint32_t) 32) {
		return SDIO_64B_BLOCK;
	}
	if (data_size > (uint32_t) 16) {
		return SDIO_32B_BLOCK;
	}
	if (data_size > (uint32_t) 8) {
		return SDIO_16B_BLOCK;
	}
	if (data_size > (uint32_t) 4) {
		return SDIO_8B_BLOCK;
	}
	if (data_size > (uint32_t) 2) {
		return SDIO_4B_BLOCK;
	}

	return SDIO_4B_BLOCK;
}

static uint32_t sdio_get_blocksize_dctrl(rtw_sdio_block_size_t block_size)
{
	switch (block_size) {
	case SDIO_1B_BLOCK:
		return SDIO_DataBlockSize_1b;
	case SDIO_2B_BLOCK:
		return SDIO_DataBlockSize_2b;
	case SDIO_4B_BLOCK:
		return SDIO_DataBlockSize_4b;
	case SDIO_8B_BLOCK:
		return SDIO_DataBlockSize_8b;
	case SDIO_16B_BLOCK:
		return SDIO_DataBlockSize_16b;
	case SDIO_32B_BLOCK:
		return SDIO_DataBlockSize_32b;
	case SDIO_64B_BLOCK:
		return SDIO_DataBlockSize_64b;
	case SDIO_128B_BLOCK:
		return SDIO_DataBlockSize_128b;
	case SDIO_256B_BLOCK:
		return SDIO_DataBlockSize_256b;
	case SDIO_512B_BLOCK:
		return SDIO_DataBlockSize_512b;
	case SDIO_1024B_BLOCK:
		return SDIO_DataBlockSize_1024b;
	case SDIO_2048B_BLOCK:
		return SDIO_DataBlockSize_2048b;
	default:
		return 0;
	}
}

RTW_RTOS_DEFINE_ISR(sdio_irq)
{
	uint32_t intstatus = SDIO->STA;

	if ((intstatus & (SDIO_STA_CCRCFAIL | SDIO_STA_DCRCFAIL | SDIO_STA_TXUNDERR | SDIO_STA_RXOVERR  | SDIO_STA_STBITERR)) != 0) {
		sdio_transfer_failed = RTW_TRUE;
		SDIO->ICR = (uint32_t) 0xffffffff; // clear all static flags
		rtw_rtos_set_semaphore(&sdio_transfer_finished_semaphore, RTW_TRUE);
		setcounter++;
	} else {
		if ((intstatus & (SDIO_STA_CMDREND | SDIO_STA_CMDSENT)) != 0) {
			if ((SDIO->RESP1 & 0x800) != 0) {
				sdio_transfer_failed = RTW_TRUE;
				rtw_rtos_set_semaphore(&sdio_transfer_finished_semaphore, RTW_TRUE);
				setcounter++;
			} else if (current_command == SDIO_CMD_53) {
				if (current_transfer_direction == BUS_WRITE) {
					DMA2_Stream3->CR = DMA_DIR_MemoryToPeripheral |
									   DMA_Channel_4 | DMA_PeripheralInc_Disable | DMA_MemoryInc_Enable |
									   DMA_PeripheralDataSize_Word | DMA_MemoryDataSize_Word |
									   DMA_Mode_Normal | DMA_Priority_VeryHigh |
									   DMA_MemoryBurst_INC4 | DMA_PeripheralBurst_INC4 | DMA_SxCR_PFCTRL | DMA_SxCR_EN | DMA_SxCR_TCIE;
				} else {
					DMA2_Stream3->CR = DMA_DIR_PeripheralToMemory |
									   DMA_Channel_4 | DMA_PeripheralInc_Disable | DMA_MemoryInc_Enable |
									   DMA_PeripheralDataSize_Word | DMA_MemoryDataSize_Word |
									   DMA_Mode_Normal | DMA_Priority_VeryHigh |
									   DMA_MemoryBurst_INC4 | DMA_PeripheralBurst_INC4 | DMA_SxCR_PFCTRL | DMA_SxCR_EN | DMA_SxCR_TCIE;
				}
			}
			SDIO->ICR = (SDIO_STA_CMDREND | SDIO_STA_CMDSENT); // clear specific static flags
		}

		/* Check whether the external interrupt was triggered */
		if ((intstatus & SDIO_STA_SDIOIT) != 0) {
			SDIO->ICR = SDIO_ICR_SDIOITC; // clear specific static flags
			rtw_rtos_set_semaphore(&host_irq_sema, RTW_TRUE);
		}
	}
}

RTW_RTOS_DEFINE_ISR(sdio_dma_irq)
{
	rtw_result_t result;
	uint32_t ulReturn;

	/* Clear interrupt */
	DMA2->LIFCR = (uint32_t)(0x3F << 22);  // clear DMA2_STREM3 status
	result = rtw_rtos_set_semaphore(&sdio_transfer_finished_semaphore, RTW_TRUE);

	setcounter++;
	/* check result if in debug mode */
	rtw_assert("failed to set dma semaphore", result == RTW_SUCCESS);
}

RTW_RTOS_MAP_ISR(sdio_irq,     SDIO_irq)
RTW_RTOS_MAP_ISR(sdio_dma_irq, DMA2_Stream3_irq)

void SDIO_IRQHandler(void)
{
	/* Process All SDIO Interrupt Sources */
	sdio_irq();
}

void DMA2_Stream3_IRQHandler(void)
{
	sdio_dma_irq();
}

rtw_result_t stm32_powersave_disable(void)
{
	__asm("CPSID i");
	if (stm32f2_clock_needed_counter <= 0) {
		SCB->SCR &= (~((unsigned long)SCB_SCR_SLEEPDEEP_Msk));
		stm32f2_clock_needed_counter = 0;
	}
	stm32f2_clock_needed_counter++;
	__asm("CPSIE i");

	return RTW_SUCCESS;
}

rtw_result_t stm32_powersave_enable(void)
{
	__asm("CPSID i");
	stm32f2_clock_needed_counter--;
	if (stm32f2_clock_needed_counter <= 0) {
		SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
		stm32f2_clock_needed_counter = 0;
	}
	__asm("CPSIE i");

	return RTW_SUCCESS;
}