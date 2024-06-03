
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/bootloader
CFGDIR = $(TARGETDIR)/usrcfg
DRVDIR = $(TARGETDIR)/fwlib/ram_km4
DRVCOM = $(TARGETDIR)/fwlib/ram_common
LOGDIR= $(TARGETDIR)/swlib

MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/non_secure
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/secure
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/common/crashdump/include
MODULE_IFLAGS += -I$(LOGDIR)

vpath %.c $(DIR) $(CFGDIR) $(DRVDIR) $(DRVCOM) $(LOGDIR)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
ifeq ($(CONFIG_MP_SHRINK), y)
CSRC = 	$(DIR)/bootloader_km4_mp.c
else
CSRC = 	$(DIR)/bootloader_km4.c
endif

CSRC += $(DIR)/boot_ota_km4.c \
	$(DIR)/boot_security_km4.c \
	$(CFGDIR)/ameba_boot_trustzonecfg.c \
	$(CFGDIR)/ameba_bootcfg.c \
	$(CFGDIR)/ameba_flashcfg.c \
	$(DRVDIR)/ameba_flashclk.c \
	$(DRVCOM)/ameba_psram.c \
	$(DRVCOM)/ameba_clk.c \
	$(DRVCOM)/ameba_swr.c \
	$(DRVCOM)/ameba_otp.c \
	$(DRVCOM)/ameba_swr_calib.c \
	$(DRVCOM)/ameba_reset.c \
	$(DRVCOM)/ameba_pll.c \
	$(DRVCOM)/ameba_sram_swap.c \
	$(DRVCOM)/ameba_ipc_ram.c \
	$(DRVDIR)/ameba_arch.c \
	$(DRVCOM)/ameba_fault_handle.c \

CSRC += $(LOGDIR)/log.c


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
