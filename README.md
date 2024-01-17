# Yet Another Linux Development Approach

The main thing that YALDA does is automatically prepare an environment for development of specific kernel
(version, toolchain, architecture, and config of the kernel) with ability to launch it and debug the kernel
in the real time.

## Motivation
When you are in a brand new board bring-up it is usually required to develop or tune Linux kernel modules.
The main issue is that to debug it on a remote environment it is required to setup specific set of hardware
and software tools. The second point is when you are developing public user interface of the module you even do not
need a hardware but specific architecture, toolchain, version of the kernel etc. YALDA helps to setup all inclsive 
virtual environment where you can have a fun for the developing but not a configuration

## Prepare environment
YALDA checks all required dependencies basing on the configuration of your build. YALDA utilizes KConfig approach
of self configuration and as a front-end for the user

Ubuntu:
```bash
$ sudo apt install kconfig-frontends
```

Others distros can use:
```bash
$ git clone https://github.com/jameswalmsley/kconfig-frontends
$ cd kconfig-frontends
$ ./bootstrap
$ ./configure
$ make -j$(nproc)
$ sudo make install
```

It is written in BASH so it assumes that bash is installed.

## Installation
Actually installation is not the mandatory step but for developer's convenience it is better to do it
```bash
make install
```
You do not need root privileges to install and use it. Installation procedure just adds <YALDA_PATH>/bin
to your PATH envvar and sources BASH competition add-on for easy usage. But competition requires my
another project is installed [DEVENV](https://gitlab.com/dzhadinets/devenv)

## Concept
The main idea is that you are developing in/out-tree module and your work directory is the root of your sources.
Here it is you will have a Makefile of your module and usually (without YALDA) you call the Makefile of your kernel
with the path to the module. ie (not taking into account if you are using specific toolchain or arch)
```bash
$ make -C $(PATH_TO_YOUR_KERNEL) M=$(PATH_TO_YOUR_MODULE) modules "$(ADDITIONAL_FLAGS)"
```

YALDA simplify this call by calling
```bash
$ yalda "$(ADDITIONAL_FLAGS)"
```
It assumes the current dir as a root path to your modules. And takes into account the toolchain, version and config
of the kernel architecture and many more which are configured for this particular project or uses YALDA's global set
of the kernel and tools.
Moreover it additionally prepares bootloader and simplified rootfs to be launched with your code in isolated environment
So you need to configure YALDA first.

## Quick start
In YALDA's root directory
- call `make all`
- launch `bin/yalda run -S`
- run gdb `bin/yalda debug`

## Configure YALDA
As was mentioned YALDA uses KConfig with ncurces front-end. So you will have fancy UI for doing all required configurations
Hope the structure and internal help of menu items are self explaining. Just call
```bash
$ yalda config
```
![RICS-V configuration](images/config.gif "YALDA configuration")

Finally it creates .yalda directory and generates config file there which is used to get and build all sources and prepares environment.
In order to configure YALDA globally: call configuration inside YALDA's sources directory. The calling of the configuration in
other places will create a local project config and an environment.
The system consist of components and some of them may have own configuration, ie: kernel or busybox. You can call
specific configuration by passing component name as a parameter
```bash
$ yalda config kernel'
```

## Getting the sources
Basing on your project or global YALDA's config the system will obtain sources of all components of the system.
BTW it creates a cache of downloaded packages. So it will not do it againg if you have several projects in parallel
The obtaining of sources ie synchronization of local and remote copy can done for all components in the system at once
or separately
```bash
$ yalda sync [<component name>]
```
If you need specific version of sources or YALDA cannot obtain and track them using your project approach.
Then components can use local path for the sources. Do synchronization  manually and set the path.
Ie for YOCTO it is enough convenient to get and track sources using `devtool` but develop them using YALDA

## Toolchain
Toolchain can be used for components compillation. Make sure you set correct crosscompillation prefix
In case of BOOTLIN's toolchain the system gets latest stable version with glibc basing on the selected achitecture

## Build components
To build everything
```bash
$ yalda build
```
You can build separate component by passing name of the component as a parameter.
You can pass component's make targets and env variables at the end of the build command.
```bash
$ yalda build kernel dtb
```
YALDA uses init ram disk to launch the OS. So there is a final step you need to do if you are modifying
rootfs (ie busybox parts or your additional layer of fs)
```bash
$ yalda build --initrd
```
the `tinyconfig` is used as the default config the kernel

## Build your module
As was mentioned YALDA simplifies complex call of building your module. Internally it calls
```bash
$ KDIR=<path to the kernel build> INSTALL_PATH=<path to rootfs>` make -C <your dir> <other arguments from yalda's call>
```
Moreover it prepares toolchain and platform environment for your project
So you can design you makefile such way to add as much targets as you need.
Basic skeleton for new module could be following
```
├── Makefile
└── module
    ├── hello.c
    └── Makefile
```
Makefile
```make
SRC=$(realpath module)

ifeq ($(KDIR),)
KDIR := $(shell echo /lib/modules/`uname -r`/build)
else
$(info Custom linux from $(KDIR))
endif

all:
	$(MAKE) -C $(KDIR) M=$(SRC) modules

install:
	INSTALL_MOD_PATH=$(INSTALL_PATH) $(MAKE) -C $(KDIR) M=$(SRC) modules_install

clean:
	$(MAKE) -C $(KDIR) M=$(SRC) clean

```
module/Makefile
```make
obj-m := hello.o
```
module/hello.c
```c
#include <linux/module.h>
int init_module(void)
{
	printk("Hello YALDA!\n");
	return 0;
}
void cleanup_module(void)
{
	printk("Goodbye YALDA!\n");
}
MODULE_LICENSE("GPL");
```
The same way you can add any other subdirectories.
If you would like using same-dir makefile you can use combined makefile or kbuild
https://docs.kernel.org/kbuild/makefiles.html
```make
ifneq ($(KERNELRELEASE),)
obj-m := hello.o
else
SRC=$(PWD)

ifeq ($(KDIR),)
KDIR := $(shell echo /lib/modules/`uname -r`/build)
else
$(info Custom linux from $(KDIR))
endif

all:
	$(MAKE) -C $(KDIR) M=$(SRC) modules

install:
	INSTALL_MOD_PATH=$(INSTALL_PATH) $(MAKE) -C $(KDIR) M=$(SRC) modules_install

clean:
	$(MAKE) -C $(KDIR) M=$(SRC) clean
endif
```

## Debug
To debug your out-of-tree module
- configure and build YALDA
- build your module within the environment
- add breakpoint ```b do_init_module```
- load symbols ```lx-symbols <PATH TO THE MODULE>```
- add breakpoint to your module functions. (make sure __init attribute is not used for your module init function)
- enjoy

To debug in-tree modules just launch YALDA with -S parameter. It will prevent starting of the kernel
and will wait till the debugger will be connected. You will have a breakstop on start_kernel function.
Then you can add more breakpoint you need and walk through all internals of the greatest OS
![RICS-V debug](images/debug.gif "YALDA debug")

### IDE intergration
You need to enable it in the YALDA's config and tune commandline parameters (at least remove `--tui`).
Use command like to prevent conflicts with YALDA's logs
```bash
yalda --silent debug
```

## QEMU
YALDA uses QEMU as the system of virtualization. It is powerful and useful thing when you know how to use it
Most important key bindings:
- c-ax (Typing CTRL+a then x) - on the YALDA console will immediately kill the QEMU instance.
- c-ac - activate QEMU console where you will be able to play with VW, devices, power mode etc. Type
  ```
  (qemu) help
  ```
[QEMU Human Monitor Interface, Example Usage](https://web.archive.org/web/20180104171638/http://nairobi-embedded.org/qemu_monitor_console.html)

## Host kernel usage
Also you can use the current host kernel: you need to select `CONFIG_YALDA_RUN_HOST_KERNEL`
Please note that you will probable need root privileges to get your host kernel binary
You will also need to install a kernel debug symbols package for your distribution.

## Help
The system is designed to be self-explaining. Call help
```bash
$ yalda -h|--help
```
Arguments are parsed using getopts with long options support. It supports combination of options ie `-sc2`
Long options arguments are passed using `--cpu=2`

## Tips
See scripts. It is easy
Useful things:
- it mounts ./ path to /mnt/share inside QEMU
- it uses .yalda dir and mounts it to /home and set it as $HOME
- by default YALDA tries to utilize all power that your dev-station has. In order to limit it pass `-c <CPUS>` as an argument
- in order to exit from QEMU use ctrl-a-x

## Known issues
- Dependencies are not checked correctly(disabled now) and it leads to errors. will be reimplemented
- UDD module does not links to stable build
- host kernel usage is unusable for now
