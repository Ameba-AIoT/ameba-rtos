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

all: CORE_TARGETS
	make -C src all
	make -C coap all

lib: CORE_TARGETS
	make -C libs lib
	make -C xip lib

test: CORE_TARGETS
	make -C tests test

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)




#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
	make -C src clean
	make -C xip clean
	make -C libs clean
	make -C coap clean
-include $(DEPS)
