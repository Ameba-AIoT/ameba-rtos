
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/verification
MODULE_IFLAGS = -I$(DIR)./

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
ifeq ($(CONFIG_SIMULATION),y)
CSRC += $(DIR)/simulation/ameba_simulation.c
endif

ifeq ($(CONFIG_ADC_VERIFY),y)
CSRC = $(DIR)/adc/ameba_adc_test.c
endif

ifeq ($(CONFIG_EFUSE_VERIFY),y)
CSRC += $(DIR)/efuse/ameba_efuse_test.c
endif

ifeq ($(CONFIG_GDMA_VERIFY),y)
CSRC += $(DIR)/gdma/ameba_gdma_test.c
CSRC += $(DIR)/gdma/ameba_gdma_llp_test.c
endif

ifeq ($(CONFIG_GPIO_VERIFY),y)
CSRC += $(DIR)/gpio/ameba_gpio_test.c
endif

ifeq ($(CONFIG_I2C_VERIFY),y)
CSRC += $(DIR)/i2c/rtl8195a_i2c_test_sal.c
CSRC += $(DIR)/i2c/hal_i2c_test.c
endif

ifeq ($(CONFIG_PMC_VERIFY),y)
CSRC += $(DIR)/pmc/pmc_test_km0.c
endif

ifeq ($(CONFIG_SPI_COM_VERIFY),y)
CSRC += $(DIR)/spi_communication/rtl8721d_ssi_test.c
CSRC += $(DIR)/spi_communication/hal_ssi_test.c
endif

ifeq ($(CONFIG_SPIC_VERIFY),y)
CSRC += $(DIR)/spi_flash/rtl8721d_spi_flash_test.c
CSRC += $(DIR)/spi_flash/flash_runcode_test.c
CSRC += $(DIR)/spi_flash/rtl8721d_flash_N25Q00AA_patch.c
endif

ifeq ($(CONFIG_TIMER_VERIFY),y)
CSRC += $(DIR)/timer/ameba_timer_test.c
CSRC += $(DIR)/timer/ameba_rtc_test.c
CSRC += $(DIR)/timer/ameba_pwm_test.c 
CSRC += $(DIR)/timer/ameba_timer_capture_test.c
endif

ifeq ($(CONFIG_UART_VERIFY),y)
CSRC += $(DIR)/uart/ameba_uart_test.c
CSRC += $(DIR)/uart/hal_uart_test.c
CSRC += $(DIR)/uart/ameba_uart_cyclic_dma_test.c
endif

ifeq ($(CONFIG_WDG_VERIFY),y)
CSRC += $(DIR)/wdg/ameba_wdg_test.c
endif

ifeq ($(CONFIG_CAPTOUCH_VERIFY),y)
CSRC += $(DIR)/captouch/ameba_captouch_test.c
endif

ifeq ($(CONFIG_QDEC_VERIFY),y)
CSRC += $(DIR)/qdecoder/rtl8721d_qdec_test.c
endif

ifeq ($(CONFIG_IPC_VERIFY),y)
CSRC += $(DIR)/ipc/ipc_test.c
endif

ifeq ($(CONFIG_MPU_VERIFY),y)
CSRC += $(DIR)/mpu/mpu_test.c
endif

ifeq ($(CONFIG_CACHE_VERIFY),y)
CSRC += $(DIR)/cache/rtl8721d_cache_test.c
endif

ifeq ($(CONFIG_RSIP_VERIFY),y)
CSRC += $(DIR)/rsip/rsip_test.c
endif

ifeq ($(CONFIG_DHRYSTONE_TEST),y)
CSRC += $(DIR)/dhrystone/dhrystone_test.c
endif

ifeq ($(CONFIG_PERFORMANCE_VERIFY),y)
CSRC += $(DIR)/platform/ameba_performance_test.c
endif

ifeq ($(CONFIG_SBOOT_VERIFY),y)
CSRC += $(DIR)/sboot/sboot_test.c
endif

ifeq ($(CONFIG_WFE_VERIFY),y)
CSRC += $(DIR)/wfe/wfe_test.c
endif

ifeq ($(CONFIG_THERMAL_VERIFY),y)
CSRC += $(DIR)/thermal/ameba_thermal_test.c
endif

ifeq ($(CONFIG_CHIP_RST_VERIFY),y)
CSRC += $(DIR)/chip_reset/ameba_reset_test.c
endif

ifeq ($(CONFIG_DBG_TIMER_VERIFY),y)
CSRC += $(DIR)/timer/ameba_dbg_timer_test.c
endif

ifeq ($(CONFIG_SPINAND_VERIFY),y)
CSRC += $(DIR)/spi_nand/ameba_spinand_test.c
endif

ifeq ($(CONFIG_LOGUART_VERIFY),y)
CSRC += $(DIR)/loguart/ameba_loguart_test.c
CSRC += $(DIR)/loguart/hal_loguart_test.c
endif

ifeq ($(CONFIG_KEYSCAN_VERIFY),y)
CSRC += $(DIR)/keyscan/ameba_keyscan_test.c
endif

ifeq ($(CONFIG_FLASH_AVL),y)
CSRC += $(DIR)/mbedtls/km0_sync.c
endif

ifeq ($(CONFIG_WIFI_VERIFY),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halmac
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halcommon
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halbtc
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phl
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/rtl8721da
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/rtl8721da_rom
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/wifi_fw/amebdplus/include
CSRC += $(DIR)/wifi/amebadplus_wifi_test.c
CSRC += $(DIR)/wifi/amebadplus_wifi_test_lbk.c
endif
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CLEAN_OBJS CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
