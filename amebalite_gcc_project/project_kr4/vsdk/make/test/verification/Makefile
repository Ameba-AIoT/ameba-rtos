
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
ifeq ($(CONFIG_EQC),y)
MODULE_IFLAGS += -I$(DIR)/eqc
MODULE_IFLAGS += -I$(TARGETDIR)/fwlib/include
endif

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
CSRC += $(DIR)/gdma/rtl8195a_gdma_test.c
CSRC += $(DIR)/gdma/rtl8195a_gdma_llp_test.c
endif

ifeq ($(CONFIG_GPIO_VERIFY),y)
CSRC += $(DIR)/gpio/ameba_gpio_test.c
endif

ifeq ($(CONFIG_I2C_VERIFY),y)
CSRC += $(DIR)/i2c/rtl8195a_i2c_test_sal.c
CSRC += $(DIR)/i2c/hal_i2c_test.c
endif

ifeq ($(CONFIG_PMC_VERIFY),y)
CSRC += $(DIR)/pmc/pmc_test_kr4.c
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
CSRC += $(DIR)/uart/rtl8721d_uart_test.c
CSRC += $(DIR)/uart/hal_uart_test.c
CSRC += $(DIR)/uart/rtl8721d_uart_cyclic_dma_test.c
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

ifeq ($(CONFIG_LOGUART_VERIFY),y)
CSRC += $(DIR)/loguart/ameba_loguart_test.c
CSRC += $(DIR)/loguart/hal_loguart_test.c
endif

ifeq ($(CONFIG_WIFI_VERIFY),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halmac
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halcommon
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halbtc
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phl
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phy_g6
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phydm
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/rtl8720e
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/rtl8720e_rom
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/wifi_fw/amebalite/include
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phy_g6/bb
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phy_g6/rf
CSRC += $(DIR)/wifi/amebalite_wifi_test.c
CSRC += $(DIR)/wifi/amebalite_wifi_test_lbk.c
endif

ifeq ($(CONFIG_LEDC_VERIFY),y)
MODULE_IFLAGS += -I$(DIR)/ledc
CSRC += $(DIR)/ledc/ameba_ledc_test.c
endif

ifeq ($(CONFIG_AUDIO_VERIFY),y)
CSRC += $(DIR)/audio/rtl8721d_audio_test.c
CSRC += $(DIR)/vad/ameba_vad_test.c
endif

ifeq ($(CONFIG_ECDSA_VERIFY),y)
CSRC += $(DIR)/ecdsa/ecdsa_test.c
endif

ifeq ($(CONFIG_CRYPTO_VERIFY),y)
CSRC += $(DIR)/crypto/rtl8721d_crypto_test.c
CSRC += $(DIR)/crypto/cryptoSim.c
#CSRC += $(DIR)/crypto/rtl8721d_aes_ecb_test.c
endif

ifeq ($(CONFIG_MEMSHARE_VERIFY),y)
CSRC += $(DIR)/memory_share/ameba_memory_share_test.c
endif

ifeq ($(CONFIG_Flash_AVL),y)
CSRC += $(DIR)/flash_avl/kr4_sync.c
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
