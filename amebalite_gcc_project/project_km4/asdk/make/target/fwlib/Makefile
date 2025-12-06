
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/fwlib/ram_km4
DIR_COM = $(TARGETDIR)/fwlib/ram_common
CFGDIR = $(TARGETDIR)/usrcfg
LOGDIR = $(TARGETDIR)/swlib

MODULE_IFLAGS = -I$(DIR)/../include/
MODULE_IFLAGS += -I$(LOGDIR)
MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/non_secure
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/secure
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/common/crashdump/include

vpath %.c $(DIR) $(DIR_COM) $(CFGDIR) $(LOGDIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =	\
	$(DIR)/ameba_app_start.c \
	$(DIR)/ameba_arch.c \
	$(DIR)/ameba_npcap.c \
	$(DIR)/ameba_npcdsp.c \
	$(DIR)/ameba_pmc_ram.c \
#	$(DIR)/ameba_pmc.c \

CSRC +=	\
	$(DIR)/ameba_spi.c \
#	$(DIR)/ameba_sdio_host.c \
#	$(DIR)/ameba_sd.c	\
#	$(DIR)/ameba_ddrphy.c \
#	$(DIR)/ameba_rxi316_dram_init.c \
#	$(DIR)/ameba_lcdc.c \
#	$(DIR)/ameba_mipi.c \

CSRC +=	$(DIR_COM)/ameba_i2c.c \
	$(DIR_COM)/ameba_rtc.c \
	$(DIR_COM)/ameba_uart.c \
	$(DIR_COM)/ameba_tim.c \
	$(DIR_COM)/ameba_ups.c \
	$(DIR_COM)/ameba_adc.c \
	$(DIR_COM)/ameba_comparator.c \
	$(DIR_COM)/ameba_ipc_api.c \
	$(DIR_COM)/ameba_captouch.c \
	$(DIR_COM)/ameba_gdma_memcpy.c \
	$(DIR_COM)/ameba_bor.c \
	$(DIR_COM)/ameba_gdma_ram.c \
	$(DIR_COM)/ameba_ipc_ram.c \
	$(DIR_COM)/ameba_mpu_ram.c \
	$(DIR_COM)/ameba_ledc.c \
	$(DIR_COM)/ameba_thermal.c \
	$(DIR_COM)/ameba_otp.c \
	$(DIR_COM)/ameba_reset.c \
	$(DIR_COM)/ameba_codec.c \
	$(DIR_COM)/ameba_sport.c \
	$(DIR_COM)/ameba_vad.c \
	$(DIR_COM)/ameba_vad_handler.c \
	$(DIR_COM)/ameba_flash_ram.c \
	$(DIR_COM)/ameba_pll.c \
	$(DIR_COM)/ameba_clk.c \
	$(DIR_COM)/ameba_swr_calib.c \
	$(DIR_COM)/ameba_psram.c \
	$(DIR_COM)/ameba_fault_handle.c \
	$(DIR_COM)/ameba_swr.c \
#	$(DIR_COM)/ameba_pmc_com.c

CSRC +=	$(LOGDIR)/log.c
CSRC += $(LOGDIR)/sscanf_minimal.c
CSRC += $(LOGDIR)/locks.c

CSRC +=	\
		$(CFGDIR)/ameba_sleepcfg.c \
		$(CFGDIR)/ameba_flashcfg.c 
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
