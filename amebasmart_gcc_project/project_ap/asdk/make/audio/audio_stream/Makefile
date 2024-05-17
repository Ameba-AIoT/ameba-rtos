
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
lib: CORE_TARGETS
	make -C core lib
	make -C local lib
	make -C passthrough lib
	make -C services lib

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C core clean
	make -C local clean
	make -C passthrough clean
	make -C services clean
-include $(DEPS)
