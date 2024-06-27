include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COMPONENT :=
COMPONENT_CLEAN :=
COMPONENT += cmsis
COMPONENT += bootloader
COMPONENT += test
COMPONENT += os
COMPONENT += at_cmd
COMPONENT += target
COMPONENT += project
COMPONENT += utils

COMPONENT_CLEAN += cmsis
COMPONENT_CLEAN += bootloader
COMPONENT_CLEAN += test
COMPONENT_CLEAN += os
COMPONENT_CLEAN += at_cmd
COMPONENT_CLEAN += target
COMPONENT_CLEAN += project
COMPONENT_CLEAN += pmc_lib
COMPONENT_CLEAN += utils

ifeq ($(CONFIG_APP_DEMO),y)
COMPONENT += example
COMPONENT_CLEAN += example
endif

ifeq ($(CONFIG_WLAN),y)
# NP
ifeq ($(CONFIG_AS_INIC_NP),y)

COMPONENT += wlan


COMPONENT_CLEAN += wlan
endif
endif

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
$(foreach n, $(COMPONENT), $(eval $(call GenerateTargets, $(n),all)))

all: $(addsuffix -all, $(COMPONENT))

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#

$(foreach n, $(COMPONENT_CLEAN), $(eval $(call GenerateTargets,$(n),clean)))

clean: $(addsuffix -clean, $(COMPONENT_CLEAN))

-include $(DEPS)
