#!/bin/bash

function clean_initrd
{
    log_info "Cleaning initrd"
}

function clean_component
{
    local component=$1
    shift
    if [ ! -x "${YALDA_COMPONENTS[$component]}" ]; then
        log_error "Unknown component $component"
    fi
    unset -f do_clean
    source ${YALDA_COMPONENTS[$component]}
    do_clean $@
}

if [ "$1" == "--initrd" ];then
    clean_initrd
elif [ -z "$1" ];then
    for component in ${YALDA_COMPONENTS_ORDER[@]}; do
        log_info "Cleaning component $component"
        clean_component $component
    done
    clean_initrd
else
    clean_component $@
fi
