# the share variable is exported from wifi, the following is required
# ccflags-y += -DCONFIG_COMBO_MULTISDIO_EXPORT_FROM_RTW
# KBUILD_EXTRA_SYMBOLS=/lib/firmware/Module.symvers
# please mark the above two lines when testing btrtksdio driver without wifi driver

obj-m += rtkbtsdio.o
rtkbtsdio-y := rtb_sdio_probe.o rtb_sdio.o
PWD := $(shell pwd)
KVER := $(shell uname -r)
KDIR := /lib/modules/$(KVER)/build

all:
	make ${KBUILD_EXTRA_SYMBOLS} -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

