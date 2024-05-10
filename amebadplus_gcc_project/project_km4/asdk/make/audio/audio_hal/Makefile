include $(MAKE_INCLUDE_GEN)

.PHONY: all test lib clean

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
test: CORE_TARGETS
	#make -C test all

lib: CORE_TARGETS
	make -C src lib

all: CORE_TARGETS
	make -C src all
	#make -C test all
#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C src clean
	#make -C test clean
-include $(DEPS)
