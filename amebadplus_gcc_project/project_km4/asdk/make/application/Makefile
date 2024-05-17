include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMPONENT :=
#COMPONENT += realmesh
ifeq ($(CONFIG_WIFI_AUDIO),y)
COMPONENT += wifi-audio
endif

#COMPONENT_CLEAN += realmesh
COMPONENT_CLEAN += wifi-audio


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
