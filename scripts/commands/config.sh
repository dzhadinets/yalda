#!/bin/bash

function yalda_config
{
        pushd $YALDA_ROOT_DIR/build/kconfig >/dev/null
            MENUCONFIG_MODE=single_menu MENUCONFIG_COLOR=mono KCONFIG_CONFIG=$CONFIG_PATH kconfig-mconf -s main
        popd >/dev/null
}

function kernel_config
{
        pushd "$YALDA_KERNEL_SOURCE_DIR" >/dev/null
            make menuconfig O="$YALDA_KERNEL_BUILD_DIR"
        popd >/dev/null
}

function busybox_config
{
        pushd "$YALDA_BUSYBOX_SOURCE_DIR" >/dev/null
            make menuconfig O="$YALDA_BUSYBOX_BUILD_DIR"
        popd >/dev/null
}

#while [[ $# -gt 0 ]]; do
case "$1" in
    reset)
        rm -f $CONFIG_PATH
        shift
    ;;
    kernel)
        kernel_config
        shift
    ;;
    busybox)
        busybox_config
        shift
    ;;
    "")
        yalda_config
#        break
    ;;
    *)
        log_error "Unknown component $1"
    ;;
esac
#done

