
include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/coap/include

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
LWIPDIR = $(BASEDIR)/component/lwip/lwip_v2.1.2
LWIPAPI = $(BASEDIR)/component/lwip/api
NETWORK = $(BASEDIR)/component/network

vpath %.c $(NETWORK)/sntp \
		$(NETWORK)/websocket \
		$(NETWORK)/coap \
		$(LWIPDIR)/src/core/ipv4 \
		$(LWIPDIR)/src/core/ipv6 \
		$(LWIPDIR)/src/core \
		$(NETWORK)/dhcp \
		$(LWIPDIR)/src/api \
		$(NETWORK)/httpc \
		$(NETWORK)/httpd \
		$(NETWORK)/mDNS \
		$(LWIPAPI)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC +=	\
		$(NETWORK)/sntp/sntp.c \
		$(NETWORK)/websocket/wsserver_tls.c \
		$(NETWORK)/websocket/wsclient_tls.c \
		$(NETWORK)/coap/sn_coap_ameba_port.c \
		$(NETWORK)/coap/sn_coap_builder.c \
		$(NETWORK)/coap/sn_coap_header_check.c \
		$(NETWORK)/coap/sn_coap_parser.c \
		$(NETWORK)/coap/sn_coap_protocol.c \
		$(LWIPDIR)/src/core/ipv4/dhcp.c \
		$(LWIPDIR)/src/core/ipv6/dhcp6.c \
		$(LWIPDIR)/src/core/dns.c \
		$(NETWORK)/dhcp/dhcps.c \
		$(LWIPDIR)/src/core/init.c \
		$(LWIPDIR)/src/core/ipv4/autoip.c \
		$(LWIPDIR)/src/core/ipv4/icmp.c \
		$(LWIPDIR)/src/core/ipv6/icmp6.c \
		$(LWIPDIR)/src/core/ipv6/inet6.c \
		$(LWIPDIR)/src/core/ipv6/mld6.c \
		$(LWIPDIR)/src/core/ipv6/nd6.c \
		$(LWIPDIR)/src/api/err.c \
		$(NETWORK)/httpc/httpc_tls.c\
		$(NETWORK)/httpd/httpd_tls.c\
		$(NETWORK)/mDNS/mDNSPlatform.c\
		$(LWIPDIR)/src/core/stats.c \
		$(LWIPDIR)/src/core/sys.c \

CSRC += $(LWIPAPI)/lwip_netconf.c

#ifeq ($(CONFIG_HIGH_TP),y)
#OBJS = 
#endif

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
