
include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
LWIPDIR = $(BASEDIR)/component/lwip/lwip_v2.1.2
NETWORK = $(BASEDIR)/component/network
WLANDRV = $(BASEDIR)/component/wifi/driver
LWIPPORT = $(LWIPDIR)/port/realtek/freertos

MODULE_IFLAGS               += -I$(BASEDIR)/component/network/coap/include

vpath ethernetif.c $(LWIPPORT)
vpath sys_arch.c $(LWIPPORT)
vpath %.c $(LWIPDIR)/src/core \
		$(LWIPDIR)/src/api \
		$(LWIPDIR)/src/core/ipv4 \
		$(LWIPDIR)/src/core/ipv6 \
		$(LWIPDIR)/src/netif \
		$(LWIPDIR)/src/apps/mdns

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC += \
		$(LWIPDIR)/src/core/tcp_in.c \
		$(LWIPDIR)/src/core/tcp_out.c \
		$(LWIPDIR)/src/core/def.c \
		$(LWIPDIR)/src/core/ipv4/igmp.c \
		$(LWIPDIR)/src/core/pbuf.c \
		$(LWIPDIR)/src/core/inet_chksum.c \
		$(LWIPDIR)/src/api/api_msg.c \
		$(LWIPDIR)/src/api/netbuf.c \
		$(LWIPDIR)/port/realtek/freertos/sys_arch.c \
		$(LWIPDIR)/port/realtek/freertos/ethernetif.c \

CSRC +=	\
		$(LWIPDIR)/src/core/timeouts.c \
		$(LWIPDIR)/src/core/raw.c \
		$(LWIPDIR)/src/core/mem.c \
		$(LWIPDIR)/src/core/memp.c \
		$(LWIPDIR)/src/core/netif.c \
		$(LWIPDIR)/src/core/ip.c \
		$(LWIPDIR)/src/core/ipv4/ip4.c \
		$(LWIPDIR)/src/core/ipv4/ip4_addr.c \
		$(LWIPDIR)/src/core/ipv4/ip4_frag.c \
		$(LWIPDIR)/src/core/ipv6/ip6.c \
		$(LWIPDIR)/src/core/ipv6/ip6_addr.c \
		$(LWIPDIR)/src/core/ipv6/ip6_frag.c \
		$(LWIPDIR)/src/core/tcp.c \
		$(LWIPDIR)/src/core/udp.c \
		$(LWIPDIR)/src/api/sockets.c \
		$(LWIPDIR)/src/api/tcpip.c \
		$(LWIPDIR)/src/netif/ethernet.c \
		$(LWIPDIR)/src/core/ipv4/etharp.c \
		$(LWIPDIR)/src/core/ipv6/ethip6.c \
		$(LWIPDIR)/src/api/netdb.c \
		$(LWIPDIR)/src/api/api_lib.c \
		$(LWIPDIR)/src/api/netifapi.c \
		$(LWIPDIR)/src/apps/mdns/mdns.c

#ifeq ($(CONFIG_HIGH_TP),y)
#OBJS += $(OBJS_CONFIG) 
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
NETWORK_RENAME_CODE2SRAM: CORE_TARGETS
COPY_RAM_OBJS: NETWORK_RENAME_CODE2SRAM
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
