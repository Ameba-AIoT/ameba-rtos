CONFIG_BTUSB_WAKEUP_HOST = n

ifeq ($(CONFIG_BTUSB_WAKEUP_HOST), y)
	EXTRA_CFLAGS += -DCONFIG_BTUSB_WAKEUP_HOST
endif

	obj-m := rtk_btusb.o
	rtk_btusb-y = rtk_coex.o rtk_bt.o
	PWD := $(shell pwd)
	KVER := $(shell uname -r)
	KDIR := /lib/modules/$(KVER)/build
	
prepare:
	@if [ ! -L rtk_coex.c ]; then ln -s ../rtk_coex.c rtk_coex.c; fi
	@if [ ! -L rtk_coex.h ]; then ln -s ../rtk_coex.h rtk_coex.h; fi
	@if [ ! -L rtw_coex_host_api.h ]; then ln -s ../rtw_coex_host_api.h rtw_coex_host_api.h; fi

all: prepare
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	make -C ${KDIR} M=$(PWD) clean
	rm -f rtk_coex.c rtk_coex.h rtw_coex_host_api.h
