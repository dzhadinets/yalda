#!/bin/bash

# yalda can be run only via qemu, so the only valid entry point
# amount all components is "qemu"

qemu_component=${YALDA_COMPONENTS["qemu"]}
unset -f qemu_launch
source $qemu_component
qemu_launch $@
