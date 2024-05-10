
include $(MAKE_INCLUDE_GEN)

.PHONY: all lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#

COMPONENT_CLEAN += ipc
COMPONENT_CLEAN += rpc

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
$(foreach n, $(COMPONENT), $(eval $(call GenerateTargets, $(n),all)))

all:
ifeq ($(CONFIG_IMQ_EN),y)
	make -C ipc all
endif

ifeq ($(CONFIG_RPC_EN),y)
	make -C rpc all
endif
lib:
	make -C rpc lib
#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)
#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
$(foreach n, $(COMPONENT_CLEAN), $(eval $(call GenerateTargets,$(n),clean)))

clean: $(addsuffix -clean, $(COMPONENT_CLEAN))

