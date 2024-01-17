obj-$(CONFIG_MYDEBUG_MODULE) += my_module.o
my_module-objs := mydebug_module.o mydebug_module_fops.o

ifeq ($(KERNELRELEASE),)

THIS_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
ifeq ($(BUILD_DIR),)
BUILD_DIR := $(PWD)
else
BUILD_DIR := $(shell realpath $(BUILD_DIR))
$(info Custom build dir $(BUILD_DIR))
endif

ifeq ($(KDIR),)
KDIR := $(shell echo /lib/modules/`uname -r`/build)
else
$(info Custom linux from $(KDIR))
endif

CFLAGS_my_module.o := -ggdb3 -DDEBUG -O0

include $(BUILD_DIR)/.config
export
export CONFIG_MYDEBUG_MODULE=m
CFLAGS_my_module.o += -include $(BUILD_DIR)/include/generated/autoconf.h

all: $(BUILD_DIR)/.config
	make -C $(KDIR) M=$(BUILD_DIR) src=$(THIS_DIR) EXTRA_CFLAGS="$(CFLAGS_my_module.o)" modules

$(BUILD_DIR):
	mkdir -p "$@"

$(BUILD_DIR)/.config: $(THIS_DIR)/Kconfig
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR); $(KDIR)/scripts/kconfig/conf --alldefconfig $(THIS_DIR)/Kconfig

.PHONY: config
config:
	@cd $(BUILD_DIR); $(KDIR)/scripts/kconfig/mconf $(THIS_DIR)/Kconfig
	@cd $(BUILD_DIR); $(KDIR)/scripts/kconfig/conf --syncconfig $(THIS_DIR)/Kconfig

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(THIS_DIR) EXTRA_CFLAGS="$(CFLAGS_my_module.o)" clean
	rm -rf $(KBUILD_OUTPUT)

install:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(THIS_DIR) INSTALL_MOD_PATH=$(INSTALL_PATH) modules_install
endif
