include $(MAKE_INCLUDE_GEN)

.PHONY: lite standard clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets

lite:
	make -C lite all

standard:
	make -C standard all

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C lite clean
	make -C standard clean

-include $(DEPS)