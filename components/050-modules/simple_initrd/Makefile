THIS_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
ifeq ($(BUILD_DIR),)
BUILD_DIR := $(PWD)
else
BUILD_DIR := $(shell realpath $(BUILD_DIR))
endif

all: initrd.c
	@mkdir -p $(BUILD_DIR)
	$(CROSS_COMPILE)gcc -static $(THIS_DIR)/initrd.c -o $(BUILD_DIR)/init

# Make sure you are configured rdinit=init in kernel parameters in YALDA config
# BUG/NOTE: $(INSTALL_PATH)/.. - is bad solution
install:
	cd $(BUILD_DIR); echo init | cpio -o -H newc | gzip > $(INSTALL_PATH)/../initrd.cpio.gz
