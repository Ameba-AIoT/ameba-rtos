
include $(MAKE_INCLUDE_GEN)

.PHONY: all lib clean
TARGET_LIB = lib_openthread

#*****************************************************************************#
#                               VARIABLES	                              	  #
#*****************************************************************************#
THREADDIR           = $(BASEDIR)/component/wpan/openthread
TIMER 				= $(BASEDIR)/component/wpan/platform/timer
FLASH 				= $(BASEDIR)/component/wpan/platform/flash
PLATFORM_CONFIG 	= $(BASEDIR)/component/wpan/platform/config
RTK_OT_SUPPORT      = $(BASEDIR)/component/wpan/openthread/rtk_ot_support
RTK_OT_BR			= $(BASEDIR)/component/wpan/openthread/rtk_otbr
SW_RADIO			= $(BASEDIR)/component/wpan/platform/sw_radio

DIR 				= $(THREADDIR)/openthread/src/core
CLI_DIR 			= $(THREADDIR)/openthread/src/cli
EX_UTILS_DIR 		= $(THREADDIR)/openthread/examples/platforms/utils
API_DIR 			= $(DIR)/api
BACKROUTER_DIR 		= $(DIR)/backbone_router
BR_DIR 				= $(DIR)/border_router
COAP_DIR 			= $(DIR)/coap
COMMON_DIR 			= $(DIR)/common
CRYPTO_DIR 			= $(DIR)/crypto
DIAGS_DIR 			= $(DIR)/diags
MAC_DIR 			= $(DIR)/mac
MESHCOP_DIR 		= $(DIR)/meshcop
NET_DIR 			= $(DIR)/net
RADIO_DIR 			= $(DIR)/radio
THREAD_DIR 			= $(DIR)/thread
UTILS_DIR 			= $(DIR)/utils

vpath %.cpp $(THREADDIR) $(shell find $(THREADDIR) -type d)
vpath %.c $(THREADDIR) $(shell find $(THREADDIR) -type d)
#vpath %.cpp $(RTK_OT_SUPPORT) $(shell find $(RTK_OT_SUPPORT) -type d)
#vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
#vpath %.cpp $(CLI_DIR) $(shell find $(CLI_DIR) -type d)
#vpath %.cpp $(EX_UTILS_DIR) $(shell find $(EX_UTILS_DIR) -type d)
#vpath %.c $(DIR) $(shell find $(DIR) -type d)
#vpath %.c $(EX_UTILS_DIR) $(shell find $(EX_UTILS_DIR) -type d)
#vpath %.c $(RTK_OT_SUPPORT) $(shell find $(RTK_OT_SUPPORT) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#

#GLOBAL_CFLAGS += -DOPENTHREAD_CONFIG_CORE_USER_CONFIG_HEADER_ENABLE
CPPFLAGS += -Wno-unused-parameter

#*****************************************************************************#
#                               Include FILE LIST                             #
#*****************************************************************************#
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/amebasmart/fwlib/include
MODULE_IFLAGS += -I$(BASEDIR)/component/component/os/os_dep/
MODULE_IFLAGS += -I$(TIMER)/
MODULE_IFLAGS += -I$(FLASH)/
MODULE_IFLAGS += -I$(PLATFORM_CONFIG)/
MODULE_IFLAGS += -I$(SW_RADIO)/mac/
MODULE_IFLAGS += -I$(SW_RADIO)/phy/common/radio_api/
MODULE_IFLAGS += -I$(THREADDIR)/config/
MODULE_IFLAGS += -I$(THREADDIR)/rtk_otbr/
MODULE_IFLAGS += -I$(THREADDIR)/openthread/src/
MODULE_IFLAGS += -I$(THREADDIR)/openthread/src/core/
MODULE_IFLAGS += -I$(THREADDIR)/openthread/src/core/config/
MODULE_IFLAGS += -I$(THREADDIR)/openthread/include/
MODULE_IFLAGS += -I$(THREADDIR)/openthread/examples/
MODULE_IFLAGS += -I$(THREADDIR)/openthread/examples/platforms/
MODULE_IFLAGS += -I$(THREADDIR)/openthread/examples/platforms/utils/

#MODULE_IFLAGS += -I$(MBEDTLS)/
#MODULE_IFLAGS += -I$(MBEDTLS)/include/
#MODULE_IFLAGS += -I$(MBEDTLS)/include/psa/
#MODULE_IFLAGS += -I$(MBEDTLS)/library/

MODULE_IFLAGS += -I$(THREADDIR)/openthread/cli/

#MODULE_IFLAGS += -I$(THREADDIR)/openthread/third_party/mbedtls/
#MODULE_IFLAGS += -I$(THREADDIR)/openthread/third_party/mbedtls/repo/include/
#MODULE_IFLAGS += -I$(THREADDIR)/openthread/third_party/mbedtls/repo/include/psa/
#MODULE_IFLAGS += -I$(THREADDIR)/openthread/third_party/mbedtls/repo/library/

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
ifeq ($(CONFIG_802154_THREAD_BORDER_ROUTER_EN),y)
GLOBAL_CFLAGS += -DOPENTHREAD_FTD
include Makefile_br
endif

ifeq ($(CONFIG_802154_THREAD_FTD_EN),y)
GLOBAL_CFLAGS += -DOPENTHREAD_FTD
endif

ifeq ($(CONFIG_802154_THREAD_MTD_EN),y)
GLOBAL_CFLAGS += -DOPENTHREAD_MTD
endif

ifeq ($(CONFIG_802154_THREAD_RCP_EN),y)
GLOBAL_CFLAGS += -DOPENTHREAD_RADIO
include Makefile_rcp
include Makefile_spinel
else
include Makefile_td
endif

#OpenThread platform
CSRC += $(RTK_OT_SUPPORT)/rtk_ot_alarm.c \
		$(RTK_OT_SUPPORT)/rtk_ot_entropy.c \
		$(RTK_OT_SUPPORT)/rtk_ot_flash.c \
		$(RTK_OT_SUPPORT)/rtk_ot_logging.c \
		$(RTK_OT_SUPPORT)/rtk_ot_misc.c \
		$(RTK_OT_SUPPORT)/rtk_ot_system.c \
		$(RTK_OT_SUPPORT)/rtk_ot_crypto.c \
		$(RTK_OT_SUPPORT)/rtk_ot_uart.c \
		$(RTK_OT_SUPPORT)/rtk_ot_settings.c \
		$(RTK_OT_SUPPORT)/rtk_ot_test.c \
		$(RTK_OT_SUPPORT)/rtk_ot_cli_uart.c

ifeq ($(CONFIG_802154_THREAD_RADIO_RCP_EN),y)	
include Makefile_spinel
CPPSRC += $(THREADDIR)/openthread/src/lib/spinel/radio_spinel.cpp
#OpenThread radio
CPPSRC += $(RTK_OT_SUPPORT)/radio/rcp_radio/rtk_ot_radio.cpp
CPPSRC += $(RTK_OT_SUPPORT)/radio/rcp_radio/rtk_ot_hdlc_interface.cpp
else
#OpenThread radio
CSRC += $(RTK_OT_SUPPORT)/radio/rtk_ot_sw_radio.c
endif

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
#CPPOBJS = $(notdir $(CPPSRC:.cpp=.o))
#COBJS = $(notdir $(CSRC:.c=.o))
CPPOBJS= $(addprefix ./,$(patsubst $(THREADDIR)/%.cpp,%.o,$(CPPSRC)))
COBJS+= $(addprefix ./,$(patsubst $(THREADDIR)/%.c,%.o,$(CSRC)))
OBJS = $(CPPOBJS)
OBJS += $(COBJS)

DIR_CPPOBJS= $(addprefix O_DIR,$(CPPOBJS))
DIR_COBJS+= $(addprefix O_DIR,$(COBJS))
OPENTHREAD_DIR_OBJS+=$(DIR_CPPOBJS)
OPENTHREAD_DIR_OBJS+=$(DIR_COBJS)
$(OPENTHREAD_DIR_OBJS):
	$(MKDIR) -p $(patsubst O_DIR%/,%,$(dir $(lastword $@)))

#	$(COPY) -f $(addprefix $(THREADDIR)/,$(patsubst ./%.o,%.o,$@)) $@

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
#all: $(OPENTHREAD_DIR_OBJS) CORE_TARGETS COPY_RAM_OBJS

#library method
lib:$(shell cd $(THREADDIR)/openthread && git apply ./../tcplp_dup_name.ptach && cd -)	$(OPENTHREAD_DIR_OBJS) CORE_TARGETS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))
#library method end
#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS) $(ASM_OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.ii
	$(REMOVE) *.s
	$(REMOVE) *.d
	$(REMOVEDIR) src
	$(REMOVEDIR) openthread

-include $(DEPS)
