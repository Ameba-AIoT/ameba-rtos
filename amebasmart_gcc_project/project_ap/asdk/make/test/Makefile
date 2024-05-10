include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all:
ifeq ($(CONFIG_PER_TEST),y)
	@make -C verification all
endif

ifeq ($(CONFIG_DHRYSTONE_TEST),y)
	@make -C coremark all
	@make -C dhrystone all
	@make -C memtester all
	@make -C cpuburn all
endif
#	@make -C vmin_test all

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean:
ifeq ($(CONFIG_PER_TEST),y)
	@make -C verification clean
endif

ifeq ($(CONFIG_DHRYSTONE_TEST),y)
	@make -C coremark clean
	@make -C dhrystone clean
	@make -C memtester clean
	@make -C cpuburn clean
endif
	@make -C vmin_test clean

-include $(DEPS)
