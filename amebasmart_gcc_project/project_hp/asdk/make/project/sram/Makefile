
include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
#add your include path here
MODULE_IFLAGS += #-I$(BASEDIR)/component/network/coap/include

#set your source code path here
CUSTOMER_DIR = $(ROOTDIR)/make/project/sram

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
#add your source code here
OBJS +=	\
		$(CUSTOMER_DIR)/ram_test.o\

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
RENAME_CODE2SRAM: CORE_TARGETS
COPY_RAM_OBJS: RENAME_CODE2SRAM
all: COPY_RAM_OBJS
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)
