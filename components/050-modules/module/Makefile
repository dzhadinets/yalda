obj-$(CONFIG_MYDEBUG_MODULE) += my_module.o

my_module-objs := mydebug_module.o mydebug_module_fops.o

CFLAGS_my_module.o := -ggdb3 -DDEBUG

ifeq ($(KERNELRELEASE),)

ifeq ($(KDIR),)
	KDIR := $(shell echo /lib/modules/`uname -r`/build)
else
	$(info Custom linux from $(KDIR))
endif

CFLAGS_my_module.o += -O0

ifeq ($(OUT),)
	KVERSION = $(shell uname -r)
	INSTALL_PATH="/lib/modules/$(KVERSION)/"
else
	INSTALL_PATH="$(OUT)/"
endif

include .config
export
export CONFIG_MYDEBUG_MODULE=m
CFLAGS_my_module.o += -include $(PWD)/include/generated/autoconf.h

all: .config
	make -C $(KDIR) M=$$PWD EXTRA_CFLAGS="$(CFLAGS_my_module.o)" modules

.config: Kconfig
	$(KDIR)/scripts/kconfig/conf --alldefconfig Kconfig

.PHONY: config
config:
	$(KDIR)/scripts/kconfig/mconf Kconfig
	$(KDIR)/scripts/kconfig/conf --syncconfig Kconfig

clean:
	rm .config*
	rm -rf include/
	make -C $(KDIR) M=$(PWD) clean

install:
	make -C $(KDIR) M=$(PWD) modules_install
endif
