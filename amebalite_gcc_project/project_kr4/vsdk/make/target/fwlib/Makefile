
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/fwlib/ram_kr4
CFGDIR = $(TARGETDIR)/usrcfg
DIR_COM = $(TARGETDIR)/fwlib/ram_common
LOGDIR = $(TARGETDIR)/swlib

MODULE_IFLAGS += -I$(DIR)/../include/
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/common/crashdump/include
MODULE_IFLAGS += -I$(LOGDIR)
MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KR4/


vpath %.c $(DIR) $(DIR_COM) $(CFGDIR) $(LOGDIR)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/ameba_app_start.c \
	$(DIR)/ameba_plic_ram.c \
	$(DIR)/ameba_arch.c \
	$(DIR)/ameba_pinmap.c \
	$(DIR)/ameba_npcap.c \
	$(DIR)/ameba_npcdsp.c \
	$(DIR)/ameba_pmc_ram.c \

CSRC +=	$(CFGDIR)/ameba_pinmapcfg.c \
	$(CFGDIR)/ameba_sleepcfg.c \
	$(CFGDIR)/ameba_flashcfg.c \

CSRC +=	$(DIR_COM)/ameba_ipc_ram.c \
	$(DIR_COM)/ameba_ipc_api.c \
	$(DIR_COM)/ameba_tim.c \
	$(DIR_COM)/ameba_rtc.c \
	$(DIR_COM)/ameba_reset.c \
	$(DIR_COM)/ameba_adc.c \
	$(DIR_COM)/ameba_comparator.c \
	$(DIR_COM)/ameba_thermal.c \
	$(DIR_COM)/ameba_i2c.c \
	$(DIR_COM)/ameba_otp.c \
	$(DIR_COM)/ameba_uart.c \
	$(DIR_COM)/ameba_spi.c \
	$(DIR_COM)/ameba_gdma_ram.c \
	$(DIR_COM)/ameba_ledc.c \
	$(DIR_COM)/ameba_codec.c \
	$(DIR_COM)/ameba_sport.c \
	$(DIR_COM)/ameba_vad.c \
	$(DIR_COM)/ameba_flash_ram.c \
	$(DIR_COM)/ameba_pll.c \
	$(DIR_COM)/ameba_swr_calib.c \
	$(DIR_COM)/ameba_psram.c \
	$(DIR_COM)/ameba_captouch.c \
	$(DIR_COM)/ameba_sram_swap.c \

CSRC +=	$(LOGDIR)/log.c
CSRC += $(LOGDIR)/sscanf_minimal.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
#all: CORE_TARGETS
all: CORE_TARGETS COPY_RAM_OBJS

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
