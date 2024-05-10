include $(MAKE_INCLUDE_GEN)

.PHONY: codec-omx codec-simple clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets

codec-omx:
	make -C common all
	make -C codec-omx all

codec-simple:
	make -C common all
	make -C codec-simple all

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C common clean
	make -C codec-omx clean
	make -C codec-simple clean

-include $(DEPS)