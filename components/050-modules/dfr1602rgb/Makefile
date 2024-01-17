obj-$(CONFIG_LCD1602)    += dfr1602rgb.o
dfr1602rgb-objs := module.o display.o

ifeq ($(KERNELRELEASE),)

THIS_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
ifeq ($(BUILD_DIR),)
BUILD_DIR := $(PWD)
else
BUILD_DIR := $(shell realpath $(BUILD_DIR))
endif

MODULE_CFLAGS := -ggdb3 -DDEBUG -O1

ifeq ($(KDIR),)
KDIR := $(shell echo /lib/modules/`uname -r`/build)
else
$(info Custom linux from $(KDIR))
endif

export CONFIG_LCD1602=m

all:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(THIS_DIR) EXTRA_CFLAGS="$(MODULE_CFLAGS)" modules

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(THIS_DIR) clean

install:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(THIS_DIR) INSTALL_MOD_PATH=$(INSTALL_PATH) modules_install
	cp demo.sh $(INSTALL_PATH)/usr/bin

endif

