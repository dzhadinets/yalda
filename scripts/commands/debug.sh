#!/bin/bash

unset -f gdb_launch
source ${YALDA_COMPONENTS["gdb"]}
gdb_launch "$@"
