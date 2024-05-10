
include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/httplite
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/xml
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/cJSON
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/iperf3
.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIRNET = $(BASEDIR)/component/network

vpath %.c $(DIRNET) $(shell find $(DIRNET) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC +=  \
	$(DIRNET)/iperf/iperf.c \
	$(DIRNET)/ping/ping_test.c \
	$(DIRNET)/httplite/http_client.c \
	$(DIRNET)/xml/xml.c \
	$(DIRNET)/cJSON/cJSON.c \
	#$(DIRNET)/webserver/webserver.c

CSRC += $(DIRNET)/iperf3/dscp.c \
	$(DIRNET)/iperf3/iperf_api.c \
	$(DIRNET)/iperf3/iperf_auth.c \
	$(DIRNET)/iperf3/iperf_client_api.c \
	$(DIRNET)/iperf3/iperf_error.c \
	$(DIRNET)/iperf3/iperf_locale.c \
	$(DIRNET)/iperf3/iperf_sctp.c \
	$(DIRNET)/iperf3/iperf_server_api.c \
	$(DIRNET)/iperf3/iperf_tcp.c \
	$(DIRNET)/iperf3/iperf_udp.c \
	$(DIRNET)/iperf3/iperf_util.c \
	$(DIRNET)/iperf3/net.c \
	$(DIRNET)/iperf3/tcp_info.c \
	$(DIRNET)/iperf3/tcp_window_size.c \
	$(DIRNET)/iperf3/timer.c \
	$(DIRNET)/iperf3/units.c \
	$(DIRNET)/iperf3/iperf3.c \
	#$(DIRNET)/iperf3/t_uuid.c \
	#$(DIRNET)/iperf3/t_timer.c \
	#$(DIRNET)/iperf3/t_units.c


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
