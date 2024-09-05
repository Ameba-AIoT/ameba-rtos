
include $(MAKE_INCLUDE_GEN)

.PHONY: all lib lib_mp clean


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS
	make -C api all
ifeq ($(CONFIG_AS_INIC_NP), y)
ifneq ($(CONFIG_AMEBALITE), y)
	echo > wifi_feature_disable/wifi_intf_drv_to_app_ext_noused.c
endif
	make -C inic all
endif
	make -C wifi_feature_disable all

# only NP and Single Core will run make lib/lib_mp
lib: CORE_TARGETS
	make -C driver lib

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)




#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C option clean
	make -C wifi_feature_disable clean
	make -C inic_ipc clean
	make -C driver clean
-include $(DEPS)
