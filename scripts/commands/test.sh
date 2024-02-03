#!/bin/bash

#log_debug "All args: $*"
#log_debug "All args count: $#"

single=false
current_sc=""
while [[ $# -gt 0 ]]; do
case "$1" in
    -s)
        single=true
        shift
        ;;
    *)
        current_sc=$1
        shift
        ;;
esac
done

params="$YALDA_ROOT_DIR/bin/yalda --silent run"
scenarios=""

if [ $single == false ]; then

    if [[ "$ARCH" == "x86" || "$ARCH" == "x86_64" ]] ;then
        scenarios+=" $YALDA_ROOT_DIR/tests/scenarios/bios.py"
    elif [[ "$ARCH" == "arm" || "$ARCH" == "arm64" ]] ;then
        if [ "$CONFIG_YALDA_BUILD_UBOOT"=="y" ];then
            scenarios+=" $YALDA_ROOT_DIR/tests/scenarios/uboot.py"
        fi
    fi

    scenarios+=" $YALDA_ROOT_DIR/tests/scenarios/linux_loaded.py"
    scenarios+=" $YALDA_ROOT_DIR/tests/scenarios/linux_payload.py"
    if [ $current_sc!="" ];then
        scenarios+=" $current_sc"
    fi

    scenarios+=" $YALDA_ROOT_DIR/tests/scenarios/done.py"
else
    if [ ${#current_sc} == 0 ]; then
        log_info "Set(ed) flag \"single\" and there is no scenarios"
        exit -3
    else
        scenarios+=" $current_sc"
    fi
fi

eval $YALDA_ROOT_DIR/tests/process_listener.py --run \'"$params"\'  --scenario \'"$scenarios"\'
