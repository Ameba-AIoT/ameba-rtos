
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(WIFIFWDIR)/amebasmart/ram
DIR_TEST = $(WIFIFWDIR)/amebasmart/test/src
DIR_COMMON= $(WIFIFWDIR)/common
TARGET_LIB = lib_wifi_fw
TARGET_DIR = $(ROOTDIR)/lib/application/

MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_LP/

vpath %.c $(DIR)
vpath %.c $(DIR_COMMON)
vpath %.c $(DIR_TEST)

#*****************************************************************************#
#                                Source FILE LIST                             #
#*****************************************************************************#
CSRC = $(TARGET_LIB)_git_version.c \

CSRC +=$(DIR_COMMON)/wififw_btcoex_mailbox_common.c \
	   $(DIR)/wififw_btcoex_tdma.c \
       $(DIR)/wififw_init.c \
	   $(DIR)/wififw_h2c.c \
       $(DIR)/wififw_txpkt.c \
       $(DIR)/wififw_pwrctrl.c \
	   $(DIR_COMMON)/wififw_csi_common.c \
       $(DIR)/wififw_interrupt_handle.c \
	   $(DIR)/wififw_interrupt_ctrl.c \
	   $(DIR)/wififw_media.c \
	   $(DIR)/wififw_gtimer.c \
       $(DIR)/wififw_ps_state.c \
	   $(DIR)/wififw_ps_bcnhdl.c \
	   $(DIR_COMMON)/wififw_hioe_common.c \
       $(DIR)/wififw_wowlan.c     \
	   $(DIR)/wififw_disconchk.c     \
	   $(DIR)/wififw_keepalive.c     \
	   $(DIR)/wififw_c2h.c     \
	   $(DIR)/wififw_lpsofld.c \
	   $(DIR_COMMON)/wififw_phy_common.c \
	   $(DIR)/wififw_periodcca.c \
	   $(DIR)/wififw_ra_new.c \
	   $(DIR)/wififw_ps_twt.c \
	   $(DIR_COMMON)/wififw_task_common.c \
	   $(DIR)/wififw_ps_dbg.c \
	   $(DIR_COMMON)/wififw_ra_debug.c \
	   $(DIR_COMMON)/wififw_ra_hl.c \
	   $(DIR_COMMON)/wififw_ra_ll.c \
	   $(DIR_COMMON)/wififw_interrupt_handle_com.c \

ifeq ($(CONFIG_WIFI_FW_VERIFY),y)
CSRC +=	$(DIR_TEST)/wififw_serial_io_test.c \
       $(DIR_TEST)/wififw_buf_test.c \
       $(DIR_TEST)/wififw_ddma_test.c \
       $(DIR_TEST)/wififw_efuse_test.c \
       $(DIR_TEST)/wififw_int_test.c \
       $(DIR_TEST)/wififw_iopath_test.c \
       $(DIR_TEST)/wififw_reg_test.c    \
       $(DIR_TEST)/wififw_rom_test.c \
       $(DIR_TEST)/wififw_fwlbk_test.c \
	   $(DIR_TEST)/wififw_coex_test.c \
       $(DIR_TEST)/wififw_test.c \
	   $(DIR_TEST)/wififw_loopback_test.c \
       $(DIR_TEST)/wififw_util_test.c \
	   $(DIR_TEST)/wififw_phycfg_test.c
endif

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Dependency	                              #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))
	$(REMOVE) $(TARGET_LIB)_git_version.*

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

#*****************************************************************************#
#              GENERATE GIT VERSION FILE                                      #
#*****************************************************************************#
$(TARGET_LIB)_git_version.c:
	$(call GEN_GIT_VER, $(TARGET_LIB))

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)
