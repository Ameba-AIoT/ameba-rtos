#include "ameba_soc.h"
#include "os_wrapper.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "gpio_api.h"
#include "st7789v.h"




#define SPI_SCLK_PIN  	 _PA_28  //SCL
#define SPI_MOSI_PIN 	 _PA_29  //SDA
#define RST              _PA_21  //RES
#define A0               _PA_22
#define SPI_CS_PIN		 _PA_31
#define BK               _PA_23

#define SPI_MISO_PIN 	 _PA_30

#define WIDTH		240
#define HEIGHT		320
#define MEM_SIZE	WIDTH * HEIGHT * 2		//rgb565

static ST7789VVBlankCallback *g_callback = NULL;
static void *g_data = NULL;

static spi_t spi_lcd;

static void LCD_WR_REG(u16 data)
{
	GPIO_WriteBit(A0, 0);
	spi_master_write(&spi_lcd, data);
	GPIO_WriteBit(A0, 1);
}

static void LCD_WR_DATA8(u8 data)
{
	spi_master_write(&spi_lcd, data);
}

void spi_tx_done_callback(uint32_t pdata, SpiIrq event)
{
	(void)pdata;

	switch (event) {
	case SpiTxIrq:
		if (g_callback) {
			g_callback->VBlank(g_data);
		}
		break;
	default:
		printf("unknown interrput event!\n");
	}
}

static void LCD_WR_PIX(u8 *pbuf, u16 x0, u16 y0, u16 x1, u16 y1)
{
	LCD_WR_REG(0x2a);
	LCD_WR_DATA8(x0 >> 8);
	LCD_WR_DATA8(x0 & 0xff);
	LCD_WR_DATA8(x1 >> 8);
	LCD_WR_DATA8(x1 & 0xff);
	LCD_WR_REG(0x2b);
	LCD_WR_DATA8(y0 >> 8);
	LCD_WR_DATA8(y0 & 0xff);
	LCD_WR_DATA8(y1 >> 8);
	LCD_WR_DATA8(y1 & 0xff);
	LCD_WR_REG(0x2c);

	spi_master_write_stream_dma(&spi_lcd, (char *) pbuf, (x1 - x0 + 1) * (y1 - y0 + 1) * 2);
}

static void st7789v_config_init(void)
{
	LCD_WR_REG(0x11);
	DelayMs(120);

	LCD_WR_REG(0x36);
	LCD_WR_DATA8(0x0);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);

	LCD_WR_REG(0xB7);
	LCD_WR_DATA8(0x71);

	LCD_WR_REG(0xBB);
	LCD_WR_DATA8(0x3B);

	LCD_WR_REG(0xC0);
	LCD_WR_DATA8(0x2C);

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x13);

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x20);

	LCD_WR_REG(0xC6);
	LCD_WR_DATA8(0x0F);

	LCD_WR_REG(0xD0);
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xD6);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0A);
	LCD_WR_DATA8(0x0D);
	LCD_WR_DATA8(0x0B);
	LCD_WR_DATA8(0x07);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0x39);
	LCD_WR_DATA8(0x16);
	LCD_WR_DATA8(0x16);
	LCD_WR_DATA8(0x1F);
	LCD_WR_DATA8(0x3C);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x03);
	LCD_WR_DATA8(0x01);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x10);
	LCD_WR_DATA8(0x21);
	LCD_WR_DATA8(0x32);
	LCD_WR_DATA8(0x38);
	LCD_WR_DATA8(0x16);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x20);
	LCD_WR_DATA8(0x3D);

	LCD_WR_REG(0x21);

	DelayMs(120);

	LCD_WR_REG(0x29);

	DelayMs(120);
}

void st7789v_init(void)
{
	spi_lcd.spi_idx = MBED_SPI1;
	spi_init(&spi_lcd, SPI_MOSI_PIN, SPI_MISO_PIN, SPI_SCLK_PIN, SPI_CS_PIN);
	spi_format(&spi_lcd, 8, 3, 0);
	spi_frequency(&spi_lcd, 50000000);

	spi_irq_hook(&spi_lcd, (spi_irq_handler) spi_tx_done_callback, (uint32_t)&spi_lcd);

	GPIO_InitTypeDef ResetPin;
	ResetPin.GPIO_Pin = RST;
	ResetPin.GPIO_PuPd = GPIO_PuPd_NOPULL;
	ResetPin.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&ResetPin);

	GPIO_InitTypeDef DC;
	DC.GPIO_Pin = A0;
	DC.GPIO_PuPd = GPIO_PuPd_NOPULL;
	DC.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&DC);
	GPIO_WriteBit(A0, 0);

	GPIO_InitTypeDef BK_PIN;
	BK_PIN.GPIO_Pin = BK;
	BK_PIN.GPIO_PuPd = GPIO_PuPd_NOPULL;
	BK_PIN.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&BK_PIN);
	GPIO_WriteBit(BK, 1);

	/* Reset */
	GPIO_WriteBit(RST, 1);
	DelayMs(100);

	GPIO_WriteBit(RST, 0);
	DelayMs(100);

	GPIO_WriteBit(RST, 1);
	DelayMs(120);

	/* Init LCD */
	st7789v_config_init();
}

void st7789v_get_info(int *width, int *height)
{
	*width = WIDTH;
	*height = HEIGHT;
}

void st7789v_register_callback(ST7789VVBlankCallback *callback, void *data)
{
	g_callback = callback;
	g_data = data;
}

void st7789v_clean_invalidate_buffer(uint8_t *buffer)
{
	DCache_Clean((u32)buffer, MEM_SIZE);
	LCD_WR_PIX(buffer, 0, 0, WIDTH-1, HEIGHT-1);
}

