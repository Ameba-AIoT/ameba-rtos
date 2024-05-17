include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMPONENT := xip
COMPONENT += sram
COMPONENT += library
COMPONENT += src

COMPONENT_CLEAN := xip
COMPONENT_CLEAN += sram
COMPONENT_CLEAN += library
COMPONENT_CLEAN += src

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
