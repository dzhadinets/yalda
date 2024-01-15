#!/bin/bash

SQ_YALDA_BIN=${YALDA_ROOT_DIR}/bin/sq-yalda
YALDA_CMD_TEST_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)
YALDA_TEST_CONFIG_DIR=${YALDA_CMD_TEST_DIR}/../components/tests/config

skip_config_list=()
skip_component_list=()
# skip_config_list=("x86_default" "x86-64_default")
# skip_component_list=("gdb" "rootfs" "kernel" "qemu")

function save_test_result
{
    local config_test_result_file=$1
    local step=$2
    local result=$3

    local result_str="Success"
    if [ $result != 0 ]; then
        result_str="Failed"
    fi


    if [ ! -e "$config_test_result_file" ]; then
        touch "$config_test_result_file"
    fi
    echo -e "${step} - ${result_str}" >> ${config_test_result_file}
}

function list_file_name_in_dir
{
    local dir_path="$1"

    local file_name_list=()
    shopt -s nullglob
    local file_path_list=($dir_path/*)
    for file_path in "${file_path_list[@]}"; do
        local file_name=${file_path##*/}
        file_name_list+=(${file_name})
    done

    echo ${file_name_list[*]}
}

if [ "$#" -ne 0 ]; then
    if [ -n "$1" ]; then
        config_list=$1
        if [ -n "$2" ]; then
            component_list=$2
        else
            component_list=${!YALDA_COMPONENTS[@]}
        fi
    fi
else
    yalda_config_list=$(list_file_name_in_dir ${YALDA_TEST_CONFIG_DIR})
    yalda_component_list=${!YALDA_COMPONENTS[@]}

    config_list=()
    if [ ! ${#skip_config_list[@]} -eq 0 ]; then
        for yalda_config_name in ${yalda_config_list[@]}; do
            if [[ ! "${skip_config_list[*]}" =~ "${yalda_config_name}" ]]; then
                config_list+=("${yalda_config_name}")
            fi
        done
    else
        config_list=${yalda_config_list}
    fi

    component_list=()
    if [ ! ${#skip_component_list[@]} -eq 0 ]; then
        for yalda_component_name in ${yalda_component_list[@]}; do
            if [[ ! "${skip_component_list[*]}" =~ "${yalda_component_name}" ]]; then
                component_list+=(${yalda_component_name})
            fi
        done
    else
        component_list=${yalda_component_list}
    fi
fi
log_info "Test:\n\t\t configs:[${config_list[@]}]\n\t\t components:[${component_list[@]}]"

set +e

# teporary directory for all yalda tests
YALDA_TEST_TMP=$(mktemp -d -t "yalda_testXXX")
log_info "Test directory: ${YALDA_TEST_TMP}"

for config_name in ${config_list[@]}; do
    YALDA_TEST_CONFIG_FILE=${YALDA_TEST_CONFIG_DIR}/${config_name}
    if [ ! -f "${YALDA_TEST_CONFIG_FILE}" ]; then
        log_error "Config file does not exist: ${YALDA_TEST_CONFIG}"
        exit -1
    fi
    # directory for specific config test results
    YALDA_TEST_CONFIG_RESULT_DIR=${YALDA_TEST_TMP}/${config_name}
    do_log mkdir -p ${YALDA_TEST_CONFIG_RESULT_DIR}/.yalda
    do_log cp ${YALDA_TEST_CONFIG_FILE} ${YALDA_TEST_CONFIG_RESULT_DIR}/.yalda/config

    pushd $YALDA_TEST_CONFIG_RESULT_DIR >/dev/null

    retcode=0
    # sync components
    for component_name in ${component_list[@]}; do
        if [ $retcode != 0 ];then
            break
        fi
        $SQ_YALDA_BIN sync ${component_name}
        retcode=$?
        save_test_result ${YALDA_TEST_CONFIG_RESULT_DIR}/result.txt "${component_name} sync" $retcode
    done

    # build components
    for component_name in ${component_list[@]}; do
        if [ $retcode != 0 ];then
            break
        fi
        $SQ_YALDA_BIN build ${component_name}
        retcode=$?
        # if build is done save component name in file
        save_test_result ${YALDA_TEST_CONFIG_RESULT_DIR}/result.txt "${component_name} build" $retcode
    done
    if [ $retcode == 0 ];then
        $SQ_YALDA_BIN build --initrd
        retcode=$?
        save_test_result ${YALDA_TEST_CONFIG_RESULT_DIR}/result.txt "initrd build" $retcode
    fi

    # add additional steps if its needed
    if [ $retcode == 0 ];then
        $SQ_YALDA_BIN test
        retcode=$?
        save_test_result ${YALDA_TEST_CONFIG_RESULT_DIR}/result.txt "run test" $retcode
    fi

    popd >/dev/null

done

# check result
failed_test_count=0
for config_name in ${config_list[@]}; do
    filename=${YALDA_TEST_TMP}/${config_name}/result.txt

    while read line; do
        IFS="-" read -ra my_array <<< "$line"

        component="${my_array[0]}"
        result="${my_array[1]}"

        if [ $result == "Failed" ];then
            log_info "Failed: config ${config_name} component \"${component}\""
            failed_test_count=$((failed_test_count + 1))
        fi

    done < $filename
done

rm -rf ${YALDA_TEST_RESULT_TMP}

set -e

if [[ ${failed_test_count} == 0 ]]; then
    log_info "All tests passed."
    exit 0
else
    log_info "Tests failed with ${failed_test_count} errors"
    exit 1
fi
