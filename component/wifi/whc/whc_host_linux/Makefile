# SPDX-License-Identifier: GPL-2.0
#
# Out-of-tree build for the Realtek WHC Linux host driver.
# Configuration is Kconfig-driven:
#   make <name>_defconfig   # e.g. amebagreen2_sdio_defconfig (see 'make list-defconfigs')
#   make menuconfig         # optional, needs kconfiglib
#   make prepare            # stage firmware headers / BT glue into driver/include
#   make                    # build the module

# $(src) is set when the kernel Kbuild includes this file (IPC in-tree path);
# fall back to $(CURDIR) for a direct out-of-tree `make`.
mod_dir := $(if $(src),$(src),$(CURDIR))

# Config source:
#  - out-of-tree host (direct make, $(src) empty): module-local .config
#    (a defconfig copy or menuconfig output).
#  - in-tree IPC ($(src) set by the SDK kernel Kbuild): config comes from the
#    kernel/SDK Kconfig via auto.conf; never read a stray module .config here.
ifeq ($(src),)
-include $(mod_dir)/.config
endif

# Propagate the driver configs to the kernel Kbuild sub-make.
export CONFIG_AMEBADPLUS CONFIG_AMEBAGREEN2 CONFIG_AMEBALITE CONFIG_AMEBAX
export CONFIG_WHC_HCI_IPC CONFIG_WHC_HCI_SDIO CONFIG_WHC_HCI_SPI CONFIG_WHC_HCI_USB
export CONFIG_NAN CONFIG_P2P CONFIG_MCC CONFIG_BT_INIC CONFIG_IEEE80211R CONFIG_SUPPLICANT_SME
export CONFIG_WHC_CMD_PATH CONFIG_WHC_WIFI_API_PATH CONFIG_WHC_HOST_RTW_CLI
export CONFIG_RMESH CONFIG_FW_DOWNLOAD CONFIG_WHCH

ifeq ($(CONFIG_WHC_HCI_IPC),y)
# ipc build
include $(src)/driver/ipc/Kbuild
else
# non-ipc build
KDIR ?= /lib/modules/$(shell uname -r)/build

M := $(CURDIR)
B := $(abspath $O/$M)

ifeq ($(CONFIG_WHC_HCI_SPI),y)
SUB := driver/spi
else ifeq ($(CONFIG_WHC_HCI_USB),y)
SUB := driver/usb
else ifeq ($(CONFIG_WHC_HCI_SDIO),y)
SUB := driver/sdio
endif

default:
	@if [ -z "$(SUB)" ]; then \
		echo "No interface selected in .config."; \
		echo "Run 'make <name>_defconfig' (see 'make list-defconfigs') or 'make menuconfig' first."; \
		exit 1; \
	fi
	$(MAKE) -C $(KDIR) M=$(M)/$(SUB)
modules modules_install clean help kernelrelease:
	$(MAKE) -C $(KDIR) M=$(M) $@
	$(MAKE) -C $(KDIR) M=$(M)/$(SUB) $@

endif

# --- Kconfig helpers (out-of-tree only) ---
# Defined only for a direct `make` in this dir. When the SDK kernel Kbuild pulls
# this file in for IPC ($(src) set), these targets/vars are skipped so the
# in-tree parse stays clean and config comes purely from the kernel Kconfig.
ifeq ($(src),)

CONFIGS_DIR := $(mod_dir)/configs

.PHONY: menuconfig prepare list-defconfigs saveconfig

# kconfiglib comes from the system (pip install kconfiglib) or, when building
# inside the ameba-rtos tree, the bundled copy under cmake/Kconfig.
KCONFIGLIB_BUNDLED := $(mod_dir)/../../../../cmake/Kconfig
# Emits a shell snippet that sets $PP to the right PYTHONPATH (or errors out).
define kconfig_pyenv
if python3 -c 'import menuconfig' >/dev/null 2>&1; then PP=; \
elif [ -f "$(KCONFIGLIB_BUNDLED)/menuconfig.py" ]; then PP="PYTHONPATH=$(KCONFIGLIB_BUNDLED)"; \
else echo "kconfiglib not found: 'pip install kconfiglib' (or build inside the ameba-rtos tree)"; exit 1; fi
endef

%_defconfig:
	@test -f $(CONFIGS_DIR)/$@ || { echo "No such defconfig: $@ (see 'make list-defconfigs')"; exit 1; }
	cp $(CONFIGS_DIR)/$@ $(mod_dir)/.config
	@echo "Applied $@ -> .config. Next: 'make prepare' then 'make'."

list-defconfigs:
	@ls $(CONFIGS_DIR)

# Interactive configuration UI (writes a full $(mod_dir)/.config).
menuconfig:
	@$(kconfig_pyenv); \
	env $$PP KCONFIG_CONFIG=$(mod_dir)/.config python3 -m menuconfig $(mod_dir)/Kconfig

# Persist the current .config as a reusable defconfig: make saveconfig NAME=foo
saveconfig:
	@test -n "$(NAME)" || { echo "usage: make saveconfig NAME=<name>"; exit 1; }
	@test -f $(mod_dir)/.config || { echo "No .config yet; run 'make menuconfig' first"; exit 1; }
	cp $(mod_dir)/.config $(CONFIGS_DIR)/$(NAME)_defconfig
	@echo "Saved $(CONFIGS_DIR)/$(NAME)_defconfig"

# Stage firmware headers / BT glue. Independent of .config: it copies a fixed
# set of files; Kbuild later compiles only what the selected config needs.
prepare:
	$(mod_dir)/whc_setup.sh

endif  # $(src) empty (out-of-tree helpers)
