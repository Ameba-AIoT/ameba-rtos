
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMPONENT := fatfs
#COMPONENT += dct
COMPONENT += littlefs
COMPONENT += kv
COMPONENT += vfs
ifeq ($(CONFIG_BT),y)
COMPONENT += ftl
endif

#COMPONENT_CLEAN := dct
COMPONENT_CLEAN += fatfs

COMPONENT_CLEAN += littlefs
COMPONENT_CLEAN += kv
COMPONENT_CLEAN += vfs
ifeq ($(CONFIG_BT),y)
COMPONENT_CLEAN += ftl
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

