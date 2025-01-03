
#*****************************************************************************#
#                            Basic Configurations                             #
#*****************************************************************************#

ABS_ROOTDIR		= $(shell pwd)
IMAGETOOL = python $(ABS_ROOTDIR)/elf2tinf/imagetool.py imagetool
PREPENDTOOL = python $(ABS_ROOTDIR)/elf2tinf/imagetool.py prepend
TINFBINDIR = $(ABS_ROOTDIR)/tinfbin

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
          else if [ -x /bin/bash ]; then echo /bin/bash; \
          else echo sh; fi ; fi)

Q:=@
ECHO:=@echo

export ABS_ROOTDIR Q ECHO


.PHONY: all clean

all: apps

apps: linker_apps

#*****************************************************************************#
#                      RULES TO LINKER IMAGE                                  #
#*****************************************************************************#
linker_apps:
	@echo "========= linker apps start ========="
	$(Q)$(IMAGETOOL) $(TINFBINDIR)

	@if [ -f $(TINFBINDIR)/dynamic_app.bin ]; then \
		$(PREPENDTOOL) $(TINFBINDIR)/dynamic_app.bin 0x60600020; \
		echo "$(TINFBINDIR)/dynamic_app.bin Build Success"; \
	else \
		echo "$(TINFBINDIR)/dynamic_app.bin Build Failed"; \
	fi

	@if [ -f $(TINFBINDIR)/km0_km4_ca32_app.bin ]; then \
		mv $(TINFBINDIR)/km0_km4_ca32_app.bin $(TINFBINDIR)/km0_km4_ca32_app_tmp.bin; \
		cat $(TINFBINDIR)/km0_km4_ca32_app_tmp.bin $(TINFBINDIR)/dynamic_app_prepend.bin > $(TINFBINDIR)/km0_km4_ca32_app.bin; \
	fi

	@echo "========== Image manipulating end =========="

clean:
	@if [ -d $(TINFBINDIR) ]; then \
		rm -f $(TINFBINDIR)/dynamic_app.bin; \
		rm -f $(TINFBINDIR)/*_prepend.bin; \
		echo "====== Remove $(TINFBINDIR)/* Success ======";\
	else echo "$(TINFBINDIR) no exist"; fi

