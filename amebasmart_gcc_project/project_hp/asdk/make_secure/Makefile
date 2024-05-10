include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMPONENT :=
ifeq ($(CONFIG_TRUSTZONE), y)
COMPONENT += img3
COMPONENT += test
COMPONENT += fwlib
COMPONENT += os
#COMPONENT += mbedtls

COMPONENT_CLEAN := img3
COMPONENT_CLEAN += test
COMPONENT_CLEAN += fwlib
COMPONENT_CLEAN += os
COMPONENT_CLEAN += mbedtls
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
