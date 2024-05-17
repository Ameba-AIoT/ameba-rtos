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
COMPONENT += at_cmd
COMPONENT += bootloader
COMPONENT += cmsis
COMPONENT += file_system
COMPONENT += libnosys
COMPONENT += os
COMPONENT += project
COMPONENT += target
COMPONENT += test
COMPONENT += audio
COMPONENT += utils
ifeq ($(CONFIG_BT), y)
COMPONENT += bluetooth
COMPONENT_CLEAN += bluetooth
endif


ifeq ($(CONFIG_APP_DEMO),y)
COMPONENT += example
COMPONENT_CLEAN += example
endif



ifeq ($(CONFIG_WLAN),y)
COMPONENT += wlan
COMPONENT_CLEAN += wlan

# AP
ifeq ($(CONFIG_AS_INIC_AP), y)
ifneq ($(CONFIG_MP_INCLUDED), y)
COMPONENT += network
COMPONENT += api
endif

COMPONENT += application
COMPONENT += utilities
COMPONENT += utilities_example
#COMPONENT += wlan_encrypt


# NP
else ifeq ($(CONFIG_AS_INIC_NP),y)
COMPONENT += api


#Single Core
else ifeq ($(CONFIG_SINGLE_CORE_WIFI),y)
ifneq ($(CONFIG_MP_INCLUDED), y)
COMPONENT += network
COMPONENT += api
endif
COMPONENT += application
COMPONENT += utilities
COMPONENT += utilities_example
#COMPONENT += wlan_encrypt

endif
endif

ifeq ($(CONFIG_MBED_TLS_ENABLED),y)
COMPONENT += mbedtls
COMPONENT_CLEAN += mbedtls
endif



COMPONENT_CLEAN += at_cmd

COMPONENT_CLEAN += api
COMPONENT_CLEAN += application
COMPONENT_CLEAN += network
COMPONENT_CLEAN += utilities
COMPONENT_CLEAN += utilities_example
COMPONENT_CLEAN += wlan_encrypt
COMPONENT_CLEAN += wps
COMPONENT_CLEAN += bootloader
COMPONENT_CLEAN += cmsis
COMPONENT_CLEAN += file_system
COMPONENT_CLEAN += libnosys
COMPONENT_CLEAN += os
COMPONENT_CLEAN += pmc_lib
COMPONENT_CLEAN += project
COMPONENT_CLEAN += target
COMPONENT_CLEAN += test
COMPONENT_CLEAN += audio
COMPONENT_CLEAN += utils
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
