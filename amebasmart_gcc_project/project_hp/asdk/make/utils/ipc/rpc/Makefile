
include $(MAKE_INCLUDE_GEN)

.PHONY: all lib clean


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets

all:
	make -C hal all
	make -C framework all
ifeq ($(CONFIG_RPC_TEST),y)
	make -C tests all
endif

lib: CORE_TARGETS
	make -C framework lib

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)




#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C hal clean
	make -C framework clean
	make -C tests clean
-include $(DEPS)
