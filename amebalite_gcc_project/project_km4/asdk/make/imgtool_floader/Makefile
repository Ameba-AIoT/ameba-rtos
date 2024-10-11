
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
FL_DIR = $(BASEDIR)/component/soc/common/flash_download/imgtool_floader
RAM_COM_DIR = $(TARGETDIR)/fwlib/ram_common
RAM_HP_DIR = $(TARGETDIR)/fwlib/ram_km4
FTL_DIR = $(BASEDIR)/component/soc/common/flash_wrapper/src
SWLIB_DIR = $(TARGETDIR)/swlib

MODULE_IFLAGS += -I$(TARGETDIR)/app/xmodem \
		 -I$(TARGETDIR)/fwlib/include \
		 -I$(SWLIB_DIR) \
		 -I$(USB_COM_DIR) \
		 -I$(FL_DIR) \
		 -I$(FTL_DIR)/../inc \
		 -I$(FREERTOSDIR)/include \
                 -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/non_secure \
                 -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/secure

GLOBAL_CFLAGS += -DCONFIG_DOWNLOAD_IND_EN

vpath %.c $(FL_DIR) $(RAM_COM_DIR) $(RAM_HP_DIR) $(SWLIB_DIR) $(FTL_DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = 	$(RAM_COM_DIR)/ameba_arch.c \
	$(RAM_COM_DIR)/ameba_codec.c \
	$(RAM_COM_DIR)/ameba_ipc_ram.c \
	$(RAM_COM_DIR)/ameba_otp.c \
	$(RAM_COM_DIR)/ameba_reset.c \
	$(RAM_COM_DIR)/ameba_tim.c \
	$(SWLIB_DIR)/log.c \
	$(FL_DIR)/loguart_intf.c \
	$(FL_DIR)/floader_debug.c \
	$(FL_DIR)/floader_fb.c \
	$(FL_DIR)/floader_intf.c \
	$(FL_DIR)/floader.c \
	$(FTL_DIR)/ameba_nor_ftl.c

#	$(RAM_HP_DIR)/ameba_flashclk.c
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS

	
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
