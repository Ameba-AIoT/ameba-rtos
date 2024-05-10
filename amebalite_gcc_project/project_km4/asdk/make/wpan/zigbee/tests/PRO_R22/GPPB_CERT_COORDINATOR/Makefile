ifeq ($(CONFIG_802154_ZIGBEE_ROLE_ZC_ZR), y)

DIR_GPPB_COOR = $(ZB_STACK_DIR)/tests/PRO_R22/certification/GPPB_CERT_COORDINATOR/

vpath %c $(DIR_GPPB_COOR)

SRCSC = $(DIR_GPPB_COOR)/on_off_output_zc.c
OBJSC = $(notdir $(SRCSC:.c=.o))

OBJS += ${OBJSC}

endif


