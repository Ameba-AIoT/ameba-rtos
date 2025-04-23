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

ifeq ($(CONFIG_WFA_SRC),y)
COMPONENT += wifi-audio-src
endif


#COMPONENT_CLEAN += realmesh
COMPONENT_CLEAN += wifi-audio
COMPONENT_CLEAN += wifi-audio-src

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
$(foreach n, $(COMPONENT), $(eval $(call GenerateTargets, $(n),all)))

all: $(addsuffix -all, $(COMPONENT)) 
ifeq ($(CONFIG_MATTER_EN),y)
	@make -C $(MATTER_BUILDDIR)/make all
endif

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
$(foreach n, $(COMPONENT_CLEAN), $(eval $(call GenerateTargets,$(n),clean)))

clean: $(addsuffix -clean, $(COMPONENT_CLEAN))
ifeq ($(CONFIG_MATTER_EN),y)
	@make -C $(MATTER_BUILDDIR)/make clean
endif

-include $(DEPS)
