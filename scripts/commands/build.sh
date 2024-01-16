#!/bin/bash

ver_in_range()
{
    # is ver_this version in range [ver_low, ver_high]
    local ver_this=$1
    local ver_low=$2
    local ver_high=$3
    [ "$ver_low" == "X" ] && ver_low="0.0.0"
    [ "$ver_high" == "X" ] && ver_high="999.999.999"
    local ver_mid
    ver_mid=$(echo -e "$ver_low\n$ver_high\n$ver_this" | sort -V | sed -n 2p)
    [ "$ver_mid" = "$ver_this" ]
}

series_file_matches_version()
{
    local series_file_name
    series_file_name="$(basename -- $1)"
    local ver_arg=$2

    local ver_pattern="^(([0-9]+)(\.[0-9]+)(|\.[0-9]+)|X)$"
    if [[ "$series_file_name" =~ ^series_([^_]+)(_([^_]+))?$ ]]; then
        local ver_low="${BASH_REMATCH[1]}"
        local ver_high="${BASH_REMATCH[3]}"

        if [[ "$ver_low" == "$ver_high" || ("$ver_low" == "X" && -z "$ver_high") ]]; then
            return 1
        fi

        if [[ ("$ver_low" =~ $ver_pattern) && ("$ver_high" =~ $ver_pattern) ]]; then
            if ver_in_range "$ver_arg" "$ver_low" "$ver_high"; then
                return 0
            fi
        fi
    fi

    return 1
}

get_series_for_version()
{
    # TODO: Add to README
    # Allowed format for series files:
    #   series_4[.0][.0] - for one specific version
    #   series_4[.0][.0]_5[.0][.0] - for version in range 4[.0][.0] ... 5[.0][.0]
    #   series_4[.0][.0]_X - for version greater than 4[.0][.0]
    #   series_X_5[.0][.0] - for version lover than 5[.0][.0]

    # Disallowed format for series files:
    #   series_X
    #   series_X_X
    #   series_4.0.0_4.0.0

    local this_version=$1
    local patches_dir=$2
    local series_file_for_this_version="$patches_dir/series_default"

    if find "$patches_dir" | grep -q "series_.*${this_version}"; then
        series_file_for_this_version="$patches_dir/series_$this_version"
    else
        for series_file in "$patches_dir"/series_*; do
            if series_file_matches_version "$series_file" "$this_version"; then
                series_file_for_this_version="$series_file"
                break
            fi
        done
    fi

    echo "$series_file_for_this_version"
}

function build_initrd
{
    log_info "Building InitRamDisk at $YALDA_INITRD"
    pushd "$YALDA_OUTPUT_DIR" >/dev/null
    find . -print0 | cpio --null -ov --format=newc 2>/dev/null | gzip > "$YALDA_INITRD"
    popd >/dev/null
}

function build_component
{
    local component=$1
    shift
    if [ ! -x "${YALDA_COMPONENTS[$component]}" ]; then
        log_error "Unknown component $component"
    fi
    unset -f do_build
    source ${YALDA_COMPONENTS[$component]}
    do_build $@
}

if [ "$1" == "--initrd" ];then
    build_initrd
elif [ -z "$1" ];then
    if [ -z "$CONFIG_YALDA_PREBUILD_PATH" ]; then
        do_log $CONFIG_YALDA_PREBUILD_PATH
    fi
    for component in ${YALDA_COMPONENTS_ORDER[@]}; do
        log_info "Building component $component"
        build_component $component
    done
    build_initrd
    if [ -z "$CONFIG_YALDA_POSTBUILD_PATH" ]; then
        do_log $CONFIG_YALDA_POSTBUILD_PATH
    fi
else
    build_component $@
fi
