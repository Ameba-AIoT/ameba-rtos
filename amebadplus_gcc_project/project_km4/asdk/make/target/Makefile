
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMPONENT += fwlib
COMPONENT += hal
COMPONENT += misc

COMPONENT_CLEAN += fwlib
COMPONENT_CLEAN += hal
COMPONENT_CLEAN += misc
COMPONENT_CLEAN += wifi_fw

#ifeq ($(CONFIG_WIFI_FW_EN),y)
#COMPONENT += wifi_fw
#endif
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
$(foreach n, $(COMPONENT), $(eval $(call GenerateTargets, $(n),all)))

all: $(addsuffix -all, $(COMPONENT))

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
$(foreach n, $(COMPONENT_CLEAN), $(eval $(call GenerateTargets,$(n),clean)))

clean: $(addsuffix -clean, $(COMPONENT_CLEAN))

	
