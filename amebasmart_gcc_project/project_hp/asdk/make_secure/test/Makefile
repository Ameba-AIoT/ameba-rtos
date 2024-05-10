
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/soc/amebasmart/verification
MODULE_IFLAGS = -I$(DIR)./
MODULE_IFLAGS += -O0

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

ifeq ($(CONFIG_SEC_VERIFY),y)
CSRC += $(DIR)/secure/log/ameba_log_test_secure.c

ifeq ($(CONFIG_AES_SEC_VERIFY),y)
CSRC += $(DIR)/secure/crypto/ameba_aes_ecb_test_secure.c
CSRC += $(DIR)/secure/crypto/ameba_aes_ecb_sim.c
endif
ifeq ($(CONFIG_CRYPTO_SEC_VERIFY),y)
CSRC += $(DIR)/secure/crypto/ameba_crypto_test_secure.c
#CSRC += $(DIR)/secure/crypto/ameba_crc_test_secure.c
endif
ifeq ($(CONFIG_GDMA_SEC_VERIFY),y)
CSRC += $(DIR)/secure/gdma/ameba_gdma_test_secure.c
#CSRC += $(DIR)/secure/gdma/ameba_gdma_llp_test_secure.c
endif
ifeq ($(CONFIG_EFUSE_SEC_VERIFY),y)
CSRC += $(DIR)/secure/efuse/ameba_efuse_test_secure.c
endif
ifeq ($(CONFIG_CACHE_SEC_VERIFY),y)
CSRC += $(DIR)/secure/cache/rtl8721d_cache_test_secure.c
endif
ifeq ($(CONFIG_RDP_SEC_VERIFY),y)
CSRC += $(DIR)/secure/rdp/rdp_test.c
endif
ifeq ($(CONFIG_WDG_SEC_VERIFY),y)
CSRC += $(DIR)/secure/wdg/ameba_wdg_test_secure.c
endif
ifeq ($(CONFIG_TRNG_SEC_VERIFY),y)
CSRC += $(DIR)/secure/trng/ameba_trng_test_secure.c
endif
ifeq ($(CONFIG_LEDC_SEC_VERIFY),y)
CSRC += $(DIR)/secure/ledc/ameba_ledc_test_secure.c
endif
ifeq ($(CONFIG_IRDA_SEC_VERIFY),y)
CSRC += $(DIR)/IR/protocol/ir_nec_protocol.c
CSRC += $(DIR)/secure/ir/ameba_ir_test_secure.c
endif
ifeq ($(CONFIG_TIMER_SEC_VERIFY),y)
CSRC += $(DIR)/secure/timer/ameba_timer_test_secure.c
endif
ifeq ($(CONFIG_LCDC_SEC_VERIFY),y)
CSRC += $(DIR)/secure/lcdc/ameba_lcdc_test_secure.c
endif
ifeq ($(CONFIG_UART_SEC_VERIFY),y)
CSRC += $(DIR)/secure/uart/amebad2_uart_secure_test.c
CSRC += $(DIR)/secure/uart/amebad2_hal_uart_secure_test.c
endif
ifeq ($(CONFIG_SPI_SEC_VERIFY),y)
CSRC += $(DIR)/secure/spi_communication/amebad2_spi_secure_test.c
CSRC += $(DIR)/secure/spi_communication/amebad2_hal_spi_secure_test.c
endif
ifeq ($(CONFIG_I2C_SEC_VERIFY),y)
CSRC += $(DIR)/secure/i2c/amebad2_i2c_secure_test.c
CSRC += $(DIR)/secure/i2c/amebad2_hal_i2c_secure_test.c
endif
ifeq ($(CONFIG_EDDSA_SEC_VERIFY),y)
CSRC += $(DIR)/secure/eddsa/ameba_eddsa_test_secure.c
endif
ifeq ($(CONFIG_ECDSA_SEC_VERIFY),y)
CSRC += $(DIR)/secure/ecdsa/ameba_ecdsa_test_secure.c
endif
ifeq ($(CONFIG_RSA_SEC_VERIFY),y)
CSRC += $(DIR)/secure/rsa/ameba_rsa_test_secure.c
endif
ifeq ($(CONFIG_TZ_SEC_VERIFY),y)
CSRC += $(DIR)/secure/trustzone/ameba_trustzone_test_secure.c
endif
ifeq ($(CONFIG_AUDIO_SEC_VERIFY),y)
CSRC += $(DIR)/secure/audio/ameba_audio_test_secure.c
CSRC += $(DIR)/secure/vad/ameba_vad_test_secure.c
CSRC += $(DIR)/secure/vad/ameba_bt_vad_test_secure.c
endif
endif
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
