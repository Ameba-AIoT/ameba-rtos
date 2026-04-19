CONFIG_BTUSB_WAKEUP_HOST = n

ifeq ($(CONFIG_BTUSB_WAKEUP_HOST), y)
	EXTRA_CFLAGS += -DCONFIG_BTUSB_WAKEUP_HOST
endif

	obj-m := rtk_btusb.o
	rtk_btusb-y = rtk_coex.o rtk_bt.o
	PWD := $(shell pwd)
	KVER := $(shell uname -r)
	KDIR := /lib/modules/$(KVER)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	make -C ${KDIR} M=$(PWD) clean
