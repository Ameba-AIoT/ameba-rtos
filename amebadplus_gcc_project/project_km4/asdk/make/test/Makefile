
include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
COMPONENT :=  
ifneq ($(CONFIG_PER_TEST)_$(CONFIG_PER_TEST),)
COMPONENT += verification
COMPONENT_CLEAN += verification
endif
ifeq ($(CONFIG_DHRYSTONE_TEST),y)
COMPONENT += coremark
COMPONENT += dhrystone
COMPONENT_CLEAN := coremark
COMPONENT_CLEAN += dhrystone
endif
ifeq ($(CONFIG_MEMORY_VERIFY),y)
COMPONENT += memtester
COMPONENT_CLEAN := memtester
endif
ifeq ($(CONFIG_RTLIB_VERIFY),y)
COMPONENT += std_lib
COMPONENT += apple
COMPONENT_CLEAN += std_lib
COMPONENT_CLEAN += apple
endif

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
$(foreach n, $(COMPONENT), $(eval $(call GenerateTargets, $(n),all)))

COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS $(addsuffix -all, $(COMPONENT))

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
$(foreach n, $(COMPONENT_CLEAN), $(eval $(call GenerateTargets,$(n),clean)))

clean: $(addsuffix -clean, $(COMPONENT_CLEAN))

-include $(DEPS)
