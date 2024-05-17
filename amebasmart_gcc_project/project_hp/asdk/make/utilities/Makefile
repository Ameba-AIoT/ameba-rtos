
include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/httplite
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/xml
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/cJSON
.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIRNET = $(BASEDIR)/component/network
DIRAPP = $(BASEDIR)/component/network/iperf

vpath %.c $(DIRAPP)
vpath %.c $(DIRNET) $(shell find $(DIRNET) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC +=  \
	$(DIRAPP)/iperf.c \
	$(DIRNET)/ping/ping_test.c \
	$(DIRNET)/httplite/http_client.c \
	$(DIRNET)/xml/xml.c \
	$(DIRNET)/cJSON/cJSON.c \
	#$(DIRNET)/webserver/webserver.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)
