
include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/wifi/wifi_fw/amebalite/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/wifi/wifi_fw/common

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(WIFIFWDIR)/amebalite/ram
DIR_COMMON= $(WIFIFWDIR)/common
DIR_TEST = $(WIFIFWDIR)/amebalite/test/src
TARGET_LIB = lib_wifi_fw

vpath %.c $(DIR)
vpath %.c $(DIR_COMMON)
vpath %.c $(DIR_TEST)

#*****************************************************************************#
#                                Source FILE LIST                             #
#*****************************************************************************#
ifeq ($(CONFIG_AS_INIC_NP),y)
CSRC =$(DIR)/wififw_init.c \
       $(DIR)/wififw_txpkt.c \
       $(DIR)/wififw_pwrctrl.c \
	   $(DIR_COMMON)/wififw_csi_common.c \
       $(DIR)/wififw_interrupt_handle.c \
	   $(DIR_COMMON)/wififw_interrupt_ctrl_common.c \
       $(DIR)/wififw_ps_state.c \
	   $(DIR_COMMON)/wififw_ps_state_common.c \
	   $(DIR_COMMON)/wififw_ps_bcnhdl_common.c \
	   $(DIR_COMMON)/wififw_hioe_common.c \
       $(DIR)/wififw_wowlan.c     \
	   $(DIR)/wififw_lpsofld.c \
	   $(DIR)/wififw_ra_new.c \
	   $(DIR_COMMON)/wififw_phy_common.c \
	   $(DIR)/wififw_periodcca.c \
	   $(DIR)/wififw_ps_twt.c \
	   $(DIR)/wififw_h2c.c \
	   $(DIR)/wififw_ps_dbg.c \
	   $(DIR_COMMON)/wififw_h2c_common.c     \
	   $(DIR_COMMON)/wififw_c2h_common.c \
	   $(DIR_COMMON)/wififw_gtimer_common.c \
	   $(DIR_COMMON)/wififw_btcoex_mailbox_common.c \
	   $(DIR_COMMON)/wififw_btcoex_tdma_common.c \
	   $(DIR_COMMON)/wififw_task_common.c \
	   $(DIR_COMMON)/wififw_ra_hl.c \
	   $(DIR_COMMON)/wififw_ra_ll.c \
	   $(DIR_COMMON)/wififw_interrupt_handle_com.c \
	   $(DIR_COMMON)/wififw_ra_debug.c \
	   $(DIR_COMMON)/wififw_wowlan_common.c \
	   $(DIR_COMMON)/wififw_keepalive_common.c \
	   $(DIR_COMMON)/wififw_disconchk_common.c \
	   $(DIR_COMMON)/wififw_media_common.c \
	   $(TARGET_LIB)_git_version.c
ifeq ($(CONFIG_WIFI_FW_VERIFY),y)
CSRC +=	$(DIR_TEST)/wififw_serial_io_test.c \
       $(DIR_TEST)/wififw_buf_test.c \
       $(DIR_TEST)/wififw_ddma_test.c \
       $(DIR_TEST)/wififw_efuse_test.c \
       $(DIR_TEST)/wififw_int_test.c \
       $(DIR_TEST)/wififw_iopath_test.c \
       $(DIR_TEST)/wififw_reg_test.c    \
       $(DIR_TEST)/wififw_fwlbk_test.c \
	   $(DIR_TEST)/wififw_coex_test.c \
       $(DIR_TEST)/wififw_test.c \
	   $(DIR_TEST)/wififw_loopback_test.c \
       $(DIR_TEST)/wififw_util_test.c \
	   $(DIR_TEST)/wififw_phycfg_test.c
endif
endif

ifeq ($(CONFIG_SINGLE_CORE_WIFI),y)
CSRC =$(DIR)/wififw_init.c \
       $(DIR)/wififw_txpkt.c \
       $(DIR)/wififw_pwrctrl.c \
	   $(DIR_COMMON)/wififw_csi_common.c \
       $(DIR)/wififw_interrupt_handle.c \
	   $(DIR_COMMON)/wififw_interrupt_ctrl_common.c \
       $(DIR)/wififw_ps_state.c \
	   $(DIR_COMMON)/wififw_ps_state_common.c \
	   $(DIR_COMMON)/wififw_ps_bcnhdl_common.c \
	   $(DIR_COMMON)/wififw_hioe_common.c \
       $(DIR)/wififw_wowlan.c     \
	   $(DIR)/wififw_lpsofld.c \
	   $(DIR)/wififw_ra_new.c \
	   $(DIR_COMMON)/wififw_phy_common.c \
	   $(DIR)/wififw_periodcca.c \
	   $(DIR)/wififw_ps_twt.c \
	   $(DIR)/wififw_h2c.c \
	   $(DIR)/wififw_ps_dbg.c \
	   $(DIR_COMMON)/wififw_h2c_common.c     \
	   $(DIR_COMMON)/wififw_c2h_common.c \
	   $(DIR_COMMON)/wififw_gtimer_common.c \
	   $(DIR_COMMON)/wififw_btcoex_mailbox_common.c \
	   $(DIR_COMMON)/wififw_btcoex_tdma_common.c \
	   $(DIR_COMMON)/wififw_task_common.c \
	   $(DIR_COMMON)/wififw_ra_hl.c \
	   $(DIR_COMMON)/wififw_ra_ll.c \
	   $(DIR_COMMON)/wififw_interrupt_handle_com.c \
	   $(DIR_COMMON)/wififw_ra_debug.c \
	   $(DIR_COMMON)/wififw_wowlan_common.c \
	   $(DIR_COMMON)/wififw_keepalive_common.c \
	   $(DIR_COMMON)/wififw_disconchk_common.c \
	   $(DIR_COMMON)/wififw_media_common.c \
	   $(TARGET_LIB)_git_version.c
ifeq ($(CONFIG_WIFI_FW_VERIFY),y)
CSRC +=	$(DIR_TEST)/wififw_serial_io_test.c \
       $(DIR_TEST)/wififw_buf_test.c \
       $(DIR_TEST)/wififw_ddma_test.c \
       $(DIR_TEST)/wififw_efuse_test.c \
       $(DIR_TEST)/wififw_int_test.c \
       $(DIR_TEST)/wififw_iopath_test.c \
       $(DIR_TEST)/wififw_reg_test.c    \
       $(DIR_TEST)/wififw_fwlbk_test.c \
	   $(DIR_TEST)/wififw_coex_test.c \
       $(DIR_TEST)/wififw_test.c \
	   $(DIR_TEST)/wififw_loopback_test.c \
       $(DIR_TEST)/wififw_util_test.c \
	   $(DIR_TEST)/wififw_phycfg_test.c
endif
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
all: CORE_TARGETS COPY_RAM_OBJS
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
