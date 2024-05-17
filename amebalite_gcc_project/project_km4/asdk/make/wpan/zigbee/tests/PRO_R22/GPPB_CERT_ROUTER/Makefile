ifeq ($(CONFIG_802154_ZIGBEE_ROLE_ZC_ZR), y)

DIR_GPPB_ROUTER = $(ZB_STACK_DIR)/tests/PRO_R22/certification/GPPB_CERT_ROUTER/

vpath %c $(DIR_GPPB_ROUTER)

SRCSC = $(DIR_GPPB_ROUTER)/zr.c
OBJSC = $(notdir $(SRCSC:.c=.o))

OBJS += ${OBJSC}

endif


