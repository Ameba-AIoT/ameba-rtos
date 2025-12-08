
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/fwlib/ram_lp
CFGDIR = $(TARGETDIR)/usrcfg
DIR_COM = $(TARGETDIR)/fwlib/ram_common
DRVDIR = $(TARGETDIR)/fwlib/ram_hp
LOGDIR = $(TARGETDIR)/swlib

MODULE_IFLAGS = -I$(DIR)/../include/
MODULE_IFLAGS += -I$(LOGDIR)
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/common/crashdump/include

MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_LP/

vpath %.c $(DIR) $(DIR_COM) $(CFGDIR) $(DRVDIR) $(LOGDIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =		$(DIR)/ameba_app_start.c \
	$(DIR)/ameba_system.c \
	$(DIR)/ameba_pmc.c \
	$(DIR)/ameba_lpcnp.c \
	$(DIR)/ameba_lpcap.c \
	$(DIR)/ameba_pll.c \
	$(DRVDIR)/ameba_ddrphy.c \
	$(DRVDIR)/ameba_rxi316_dram_init.c\
#	$(DIR)/ameba_clk.c \

CSRC +=	$(CFGDIR)/ameba_sleepcfg.c \

CSRC +=	$(DIR_COM)/ameba_ipc_ram.c \
	$(DIR_COM)/ameba_ipc_api.c \
	$(DIR_COM)/ameba_tim.c \
	$(DIR_COM)/ameba_rtc.c \
	$(DIR_COM)/ameba_reset.c \
	$(DIR_COM)/ameba_thermal.c \
	$(DIR_COM)/ameba_mpu_ram.c \
	$(DIR_COM)/ameba_i2c.c \
	$(DIR_COM)/ameba_otp.c \
	$(DIR_COM)/ameba_swr.c \
	$(DIR_COM)/ameba_swr_calib.c \
	$(DIR_COM)/ameba_adc.c \
	$(DIR_COM)/ameba_comparator.c \
	$(DIR_COM)/ameba_aontimer.c \
	$(DIR_COM)/ameba_wakepin.c \
	$(DIR_COM)/ameba_arch.c \
	$(DIR_COM)/ameba_fault_handle.c \
	$(DIR_COM)/ameba_psram.c \

CSRC += $(LOGDIR)/log.c
CSRC += $(LOGDIR)/sscanf_minimal.c
CSRC += $(LOGDIR)/locks.c

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
