include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMPONENT :=
COMPONENT_CLEAN :=
#COMPONENT += bootloader
COMPONENT += at_cmd
COMPONENT += target
COMPONENT += cmsis
COMPONENT += audio
#COMPONENT += media
COMPONENT += utilities_example
COMPONENT += file_system
COMPONENT += project
COMPONENT += application
COMPONENT += cmsis-dsp
COMPONENT += test
COMPONENT += os
COMPONENT += utils
ifeq ($(CONFIG_BT), y)
COMPONENT += bluetooth
COMPONENT_CLEAN += bluetooth
endif
ifeq ($(CONFIG_WLAN),y)
# AP
ifeq ($(CONFIG_AS_INIC_AP), y)
ifneq ($(CONFIG_MP_SHRINK),y)
COMPONENT += network
endif
COMPONENT += utilities

# NP
else ifeq ($(CONFIG_AS_INIC_NP),y)
COMPONENT += wlan

# Single core
else ifeq ($(CONFIG_SINGLE_CORE_WIFI),y)
COMPONENT += wlan
COMPONENT += utilities
ifneq ($(CONFIG_MP_SHRINK),y)
COMPONENT += network
endif
endif
endif
ifeq ($(CONFIG_MBED_TLS_ENABLED),y)
COMPONENT += mbedtls
COMPONENT_CLEAN += mbedtls
endif
ifeq ($(CONFIG_GUI_EN),y)
COMPONENT += ui
COMPONENT_CLEAN += ui
endif
ifeq ($(CONFIG_LINKKIT_AWSS),y)
COMPONENT += linkkit
endif
ifeq ($(CONFIG_USB_OTG_EN),y)
COMPONENT += usb_otg
COMPONENT_CLEAN += usb_otg
endif

COMPONENT_CLEAN += network
COMPONENT_CLEAN += utilities
COMPONENT_CLEAN += wlan
COMPONENT_CLEAN += wps

#COMPONENT_CLEAN := application
COMPONENT_CLEAN += test
COMPONENT_CLEAN += os
COMPONENT_CLEAN += at_cmd
COMPONENT_CLEAN += bootloader
COMPONENT_CLEAN += target
COMPONENT_CLEAN += cmsis
COMPONENT_CLEAN += cmsis-dsp
COMPONENT_CLEAN += audio
COMPONENT_CLEAN += utilities_example
COMPONENT_CLEAN += file_system
COMPONENT_CLEAN += project
#COMPONENT_CLEAN += media
COMPONENT_CLEAN += pmc_lib
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
