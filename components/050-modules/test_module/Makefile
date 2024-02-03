obj-m += yalda_test_module.o

ifeq ($(KERNELRELEASE),)

THIS_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
ifeq ($(BUILD_DIR),)
BUILD_DIR := $(PWD)
else
BUILD_DIR := $(shell realpath $(BUILD_DIR))
endif

ifeq ($(KDIR),)
KDIR := $(shell echo /lib/modules/`uname -r`/build)
else
$(info Custom linux from $(KDIR))
endif

CFLAGS_FOR_DEBUG := -ggdb3 -DDEBUG -O0

all: install

build:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(THIS_DIR) EXTRA_CFLAGS="$(CFLAGS_FOR_DEBUG)" modules

clean:
	make -C $(KDIR) M=$(BUILD_DIR) src=$(THIS_DIR) clean

install: build
	make -C $(KDIR) M=$(BUILD_DIR) src=$(THIS_DIR) INSTALL_MOD_PATH=$(INSTALL_PATH) modules_install
endif
