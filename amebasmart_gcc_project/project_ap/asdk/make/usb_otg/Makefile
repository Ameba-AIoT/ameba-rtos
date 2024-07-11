
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMPONENT :=
ifeq ($(CONFIG_USB_DEVICE_EN),y)
COMPONENT += device
endif
ifeq ($(CONFIG_USB_HOST_EN),y)
COMPONENT += host
endif
ifeq ($(CONFIG_USB_DRD_EN),y)
COMPONENT += drd
endif

COMPONENT_CLEAN := device
COMPONENT_CLEAN += host
COMPONENT_CLEAN += drd

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

-include $(DEPS)

