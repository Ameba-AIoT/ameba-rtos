include $(MAKE_INCLUDE_GEN)

.PHONY: lib clean


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
lib: CORE_TARGETS
	make -C Bundle  lib
	make -C Common  lib
	make -C Eq      lib
	make -C wrapper lib

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C Bundle  clean
	make -C Common  clean
	make -C Eq      clean
	make -C wrapper clean
-include $(DEPS)
