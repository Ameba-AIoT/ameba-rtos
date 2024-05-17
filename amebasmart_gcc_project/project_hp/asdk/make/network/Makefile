
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMPONENT := xip
COMPONENT += sram
ifeq ($(CONFIG_MQTT_EN),y)
COMPONENT += mqtt
endif
#COMPONENT += rtsp

COMPONENT_CLEAN := xip
COMPONENT_CLEAN += sram
COMPONENT_CLEAN += httpc
COMPONENT_CLEAN += httpd
COMPONENT_CLEAN += websocket
COMPONENT_CLEAN += tftp
COMPONENT_CLEAN += rtsp
COMPONENT_CLEAN += http2
COMPONENT_CLEAN += eap
COMPONENT_CLEAN += mqtt

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


