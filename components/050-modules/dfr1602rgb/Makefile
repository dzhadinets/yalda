obj-$(CONFIG_LCD1602)    += dfr1602rgb.o
dfr1602rgb-objs := module.o display.o

ifeq ($(KERNELRELEASE),)

MODULE_CFLAGS := -ggdb3 -DDEBUG

ifeq ($(KDIR),)
	KDIR := $(shell echo /lib/modules/`uname -r`/build)
else
$(info Custom linux from $(KDIR))
	MODULE_CFLAGS += -O0
endif

export CONFIG_LCD1602=m

all:
	make -C $(KDIR) M=$(PWD) EXTRA_CFLAGS="$(MODULE_CFLAGS)" modules

clean:
	make -C $(KDIR) M=$(PWD) clean

install:
	make -C $(KDIR) M=$(PWD) modules_install

endif

