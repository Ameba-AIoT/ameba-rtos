include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all:
	@make -C os all
	@make -C fwlib all
	@make -C hal all
	@make -C swlib all
	@make -C project all
	@make -C at_cmd all
	@make -C test all
	@make -C file_system all
	@make -C audio all
	@make -C utilities_example all

	@make -C xlat_table all
	@make -C utils all
ifeq ($(CONFIG_USB_OTG_EN),y)
	@make -C usb_otg all
endif

ifeq ($(CONFIG_WLAN),y)
	@make -C wlan all
# WIFI INIC AP build
ifeq ($(CONFIG_AS_INIC_AP), y)
ifneq ($(CONFIG_MP_SHRINK),y)
	@make -C network all
endif
	@make -C utilities all

# WIFI INIC NP build
else ifeq ($(CONFIG_AS_INIC_NP),y)
	@make -C api all
endif
endif

ifeq ($(CONFIG_MBED_TLS_ENABLED),y)
	@make -C mbedtls all
endif

ifeq ($(CONFIG_BT), y)
	@make -C bluetooth all
endif

ifeq ($(CONFIG_802154_EN), y)
	@make -C wpan all
endif

	@make -C application all

	@make -C ui all

ifeq ($(CONFIG_RANGE_ENABLED), y)
	@make -C range all
endif

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean:
	@make -C os clean
	@make -C fwlib clean
	@make -C hal clean
	@make -C swlib clean
	@make -C project clean
	@make -C at_cmd clean
	@make -C test clean
	@make -C mbedtls clean
	@make -C network clean
	@make -C utilities clean
	@make -C wps clean
	@make -C usb_otg clean
	@make -C xlat_table clean
	@make -C file_system clean
	@make -C bluetooth clean
	@make -C bluetooth/bt_audio clean
	@make -C wpan clean
	@make -C audio clean
	@make -C utilities_example clean
	@make -C application clean
	@make -C ui clean
	@make -C utils clean
	@make -C wlan clean
	@make -C range clean
-include $(DEPS)
