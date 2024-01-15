## Tested on versions of components:
- kernel = v5.14
- busybox = 1_36_0
- uboot = v2023.04-rc5

## To debug U-boot:
- Run: gdb-multiarch $YALDA_UBOOT_BUILD_DIR/u-boot -ex "target remote localhost:1234"
- Run: yalda run

## Links:
- https://u-boot.readthedocs.io/en/latest/board/emulation/qemu-mips.html?highlight=debug#how-to-debug-u-boot
- http://winfred-lu.blogspot.com/2011/12/arm-u-boot-on-qemu.html
