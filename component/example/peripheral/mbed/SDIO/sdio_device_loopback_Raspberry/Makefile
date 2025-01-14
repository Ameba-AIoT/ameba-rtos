
EXTRA_CFLAGS += -I$(src)/inc
EXTRA_CFLAGS += -Wno-error=date-time

MODULE_NAME = rtk_sdio

rtk_core :=	src/sdio_ops.o \
		src/sdio_intf.o \
		src/sdio_chardev.o \
		src/sdio_loopback.o

#rtk_core +=	src/sdio_fwdl.o

$(MODULE_NAME)-y += $(rtk_core)

obj-m := $(MODULE_NAME).o

BASEDIR := /lib/modules/$(shell uname -r)
KERNELDIR ?= $(BASEDIR)/build
PWD :=$(shell pwd)

RTKDIR := $(subst $(BASEDIR)/,,$(DRIVERDIR))

.PHONY: all
all: clean modules

.PHONY:modules
modules:
	#$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) modules
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules

.PHONY:clean
clean:
	#$(MAKE) -C $(KERNELDIR) SUBDIRS=$(PWD) clean
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) clean

.PHONY:install
install:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) INSTALL_MOD_DIR=$(RTKDIR) modules_install
