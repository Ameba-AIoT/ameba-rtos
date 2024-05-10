
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMPONENT :=
ifeq ($(CONFIG_KERNEL),y)
COMPONENT += freertos
COMPONENT += freertos/freertos_v10.4.3
COMPONENT += stdlib_wrap
endif

COMPONENT_CLEAN :=
ifeq ($(CONFIG_KERNEL),y)
COMPONENT_CLEAN += freertos
COMPONENT_CLEAN += freertos/freertos_v10.4.3
COMPONENT_CLEAN += stdlib_wrap
endif

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = 

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
$(foreach n, $(COMPONENT), $(eval $(call GenerateTargets, $(n),all)))

all: CORE_TARGETS $(addsuffix -all, $(COMPONENT))

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)
#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
$(foreach n, $(COMPONENT_CLEAN), $(eval $(call GenerateTargets,$(n),clean)))

clean: $(addsuffix -clean, $(COMPONENT_CLEAN))
