
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/fwlib/ram_hp
DIR_COM = $(TARGETDIR)/fwlib/ram_common
LOGDIR = $(TARGETDIR)/swlib
CFGDIR = $(TARGETDIR)/usrcfg

MODULE_IFLAGS = -I$(DIR)/../include/
MODULE_IFLAGS += -I$(LOGDIR)
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/common/crashdump/include

MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_HP/non_secure
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_HP/secure

vpath %.c $(DIR) $(DIR_COM) $(CFGDIR) $(LOGDIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =	\
	$(DIR)/ameba_app_start.c \
	$(DIR)/ameba_pmc.c \
	$(DIR)/ameba_system.c \
	$(DIR)/ameba_pinmap.c \

CSRC +=	\
	$(DIR)/ameba_spi.c \
	$(DIR)/ameba_lcdc.c \
	$(DIR)/ameba_mipi.c \
	$(DIR)/ameba_sdio_host.c \
	$(DIR)/ameba_sd.c	\
	$(DIR)/ameba_ddrphy.c \
	$(DIR)/ameba_rxi316_dram_init.c \
	$(DIR)/ameba_codec.c \
	$(DIR)/ameba_sport.c \
    $(DIR)/ameba_audio_clock.c \
    $(DIR)/ameba_ipc_toap.c \
	$(DIR)/ameba_vad.c \
	$(DIR)/ameba_vad_bt.c \
	$(DIR)/ameba_vad_handler.c \

CSRC +=	$(DIR_COM)/ameba_i2c.c \
	$(DIR_COM)/ameba_rtc.c \
	$(DIR_COM)/ameba_uart.c \
	$(DIR_COM)/ameba_tim.c \
	$(DIR_COM)/ameba_adc.c \
	$(DIR_COM)/ameba_comparator.c \
	$(DIR_COM)/ameba_flash_ram.c \
	$(DIR_COM)/ameba_pll.c \
	$(DIR_COM)/ameba_ir.c \
	$(DIR_COM)/ameba_ipc_api.c \
	$(DIR_COM)/ameba_captouch.c \
	$(DIR_COM)/ameba_gdma_memcpy.c \
	$(DIR_COM)/ameba_bor.c \
	$(DIR_COM)/ameba_gdma_ram.c \
	$(DIR_COM)/ameba_ipc_ram.c \
	$(DIR_COM)/ameba_mpu_ram.c \
	$(DIR_COM)/ameba_ledc.c \
	$(DIR_COM)/ameba_reset.c \
	$(DIR_COM)/ameba_thermal.c \
	$(DIR_COM)/ameba_otp.c \
	$(DIR_COM)/ameba_clk.c \
	$(DIR_COM)/ameba_aontimer.c \
	$(DIR_COM)/ameba_wakepin.c \
	$(DIR_COM)/ameba_arch.c \
	$(DIR_COM)/ameba_fault_handle.c \
	$(DIR_COM)/ameba_psram.c \


CSRC += $(LOGDIR)/log.c
CSRC += $(LOGDIR)/sscanf_minimal.c
CSRC += $(LOGDIR)/locks.c

CSRC +=	\
		$(CFGDIR)/ameba_pinmapcfg.c \
		$(CFGDIR)/ameba_flashcfg.c \
		$(CFGDIR)/ameba_sleepcfg.c \

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
CORE_TARGETS: $(OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)
