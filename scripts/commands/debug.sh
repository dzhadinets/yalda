#!/bin/bash

function debug_component
{
    unset -f do_launch
    source ${YALDA_COMPONENTS["gdb"]}
    do_launch $@
}

debug_component $@
