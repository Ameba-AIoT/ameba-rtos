
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                                   VARIABLES                                 #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all:
ifneq ($(MBEDTLS_MATTER_ENABLE), y)
ifeq ($(MBEDTLS_V_2_16_9_ENABLE), y)
	make -C mbedtls-2.16.9 all
else ifeq ($(MBEDTLS_V_3_4_1_ENABLE), y)
	make -C mbedtls-3.4.1 all
endif
endif

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean:
	@make -C mbedtls-2.16.9 clean
	@make -C mbedtls-3.4.1 clean
-include $(DEPS)
