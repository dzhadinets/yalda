ifeq ($(KBUILD_OUTPUT),)
KBUILD_OUTPUT=$(PWD)
endif

all: initrd.c
	mkdir -p $(KBUILD_OUTPUT)
	$(CROSS_COMPILE)gcc -static initrd.c -o $(KBUILD_OUTPUT)/init

# Make sure you are configured rdinit=init in kernel parameters in YALDA config
install:
	cd $(KBUILD_OUTPUT); echo init | cpio -o -H newc | gzip > $(INSTALL_MOD_PATH)/initrd.cpio.gz
