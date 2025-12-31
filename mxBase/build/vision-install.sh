#!/bin/bash

# -------------------------------------------------------------------------
#  This file is part of the Vision SDK project.
# Copyright (c) 2025 Huawei Technologies Co.,Ltd.
#
# Vision SDK is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#
#           http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.
# -------------------------------------------------------------------------
# Description: Used to build packaging scripts.
# Author: Vision SDK
# Create: 2025
# History: NA

archName=""
if [[ "$1" == *"aarch64"* ]]; then
    archName="aarch"
else
    archName="x86"
fi
cann_path_value=""
targetdir=""
sdk_home_path=""
quiet="n"
print_version_number="n"
upgrade="n"
info_record_path="${HOME}/log/mindxsdk"
info_record_file="deployment.log"
LOG_SIZE_THRESHOLD=1024000
install_option="n"
transfer_param_install_path="n"
transfer_param_upgrade="n"
print_eula_info="n"
cann_path_flag="n"
upgrade_tmp_path=$(pwd)
upgrade_tmp_source_path=""
acl_dll_file="libascendcl.so"
unset CDPATH
uninstall_flag="n"
choose_gcc=0
isinputchoosegcc="n"
isinputnox11="n"
acl_owner_path=""
sourcedir=$(pwd)
old_version_info=""
new_version_info=""
get_version_info() {
    # 1 old, 2 new
    if [[ "$1" == 1 ]]; then
        old_version_info=$(sed -n 1p $targetdir/mxVision/version.info)
    elif [[ "$1" == 2 ]]; then
        new_version_info=$(sed -n 1p $(pwd)/version.info)
    fi
}

modify_run_permission() {
    path=$1
    find "$path"/ -type d -exec chmod 750 {} +
    find "$path"/ -maxdepth 1 -type d -name include -exec chmod 550 {} +
    find "$path"/ -maxdepth 1 -type d -name lib -exec chmod 550 {} +
    find "$path"/ -maxdepth 1 -type d -name python -exec chmod 550 {} +
    find "$path"/ -type d -name opensource -exec chmod 550 {} +
    find "$path"/ -type f -name 'version.info' -exec chmod 440 {} +
    find "$path"/ -type f -name 'filelist.txt' -exec chmod 440 {} +
    find "$path"/ -type f -name '*.so' -exec chmod 440 {} +
    find "$path"/ -type f -name '*.so.*' -exec chmod 440 {} +
    find "$path"/opensource/bin/ -type f -exec chmod 550 {} +
    find "$path"/python/ -type f -exec chmod 440 {} +

    find "$path"/ -type f -name '*.sh' -exec chmod 500 {} +
    find "$path"/ -type f -name '*.py' -exec chmod 550 {} +
    find "$path"/ -type f -name '*.json' -exec chmod 640 {} +
    find "$path"/ -type f -name '*.cpp' -exec chmod 440 {} +
    find "$path"/ -type f -name '*.h' -exec chmod 440 {} +
    find "$path"/ -type f -name '*.hpp' -exec chmod 440 {} +
    find "$path"/ -type f -name '*.md' -exec chmod 440 {} +
    find "$path"/ -type f -name '*.conf' -exec chmod 640 {} +
    find "$path"/ -type f -name '*.pipeline' -exec chmod 640 {} +
    find "$path"/ -type f -name '*.cfg' -exec chmod 640 {} +
    find "$path"/ -type f -name '*.names' -exec chmod 640 {} +
    find "$path"/ -type f -name '*.txt' -exec chmod 640 {} +

    find "$path"/ -perm /u+x -type f -exec chmod 500 {} +

    find "$path"/ -type f -name streamserver.conf -exec chmod 600 {} +

    # stream server keys permission
    find "$path"/samples/ -type d -name keys -exec chmod 700 {} +
    find "$path"/samples/ -type f -name '*.key' -exec chmod 400 {} +
    find "$path"/samples/ -type f -name '*.crt' -exec chmod 400 {} +
    find "$path"/samples/ -type f -name '*.crl' -exec chmod 400 {} +
    find "$path"/samples/ -type f -name '*.ks' -exec chmod 600 {} +
}

get_acl_path() {
    acl_path_tmp="$1"
    acl_owner_path=""
    if [[ -f "$acl_path_tmp/$acl_dll_file" ]]; then
        acl_owner_path="$acl_path_tmp"
    elif [[ -f "$acl_path_tmp/lib64/$acl_dll_file" ]]; then
        acl_owner_path="$acl_path_tmp/lib64"
    elif [[ -f "$acl_path_tmp/ascend-toolkit/latest/lib64/$acl_dll_file" ]]; then
        acl_owner_path="$acl_path_tmp/ascend-toolkit/latest/lib64"
    elif [[ -f "$acl_path_tmp/nnrt/latest/lib64/$acl_dll_file" ]]; then
        acl_owner_path="$acl_path_tmp/nnrt/latest/lib64"
    fi
}

get_acl_versions_path() {
    acl_A300_path="/usr/local/Ascend"
    acl_A500_path_old="/home/data/miniD/driver/lib64"
    acl_A500_path_new="/opt/ascend"

    get_acl_path $ASCEND_HOME_PATH
    if test x"$acl_owner_path" != x; then
        return
    fi

    get_acl_path $acl_A300_path
    if test x"$acl_owner_path" != x; then
        return
    fi

    get_acl_path $acl_A500_path_old
    if test x"$acl_owner_path" != x; then
        return
    fi

    get_acl_path $acl_A500_path_new
    if test x"$acl_owner_path" != x; then
        return
    fi
}

delete_wheel_pkg() {
    mindx="$(python3 -m pip list --no-index | grep -w mindx)"
    if [[ -n "$mindx" ]]; then
        echo y | python3 -m pip uninstall mindx &> /dev/null
        log 'Uninstall wheel package successfully.'
        echo 'Uninstall wheel package successfully.'
    else
        log 'No exists wheel package to uninstall.'
        echo 'No exists wheel package to uninstall.'
    fi
}

save_upgrade_info() {
    path="$1"
    user_ip=$(who am i | awk '{print $NF}' | sed 's/(//g' | sed 's/)//g')
    if [[ -z "${user_ip}" ]]; then
        user_ip=localhost
    fi
    user_name=$(whoami)
    host_name=$(hostname)
    append_text="[$(date "+%Y-%m-%d %H:%M:%S")][$user_ip][$user_name][$host_name]:"
    echo "$append_text" >> "$path"
    append_text=$old_version_info
    append_text+="    ->    "
    append_text+=$new_version_info
    echo "$append_text" >> "$path"
    echo "Upgrade MindX SDK successful." >> "$path"
}

save_install_info() {
    path="$1"
    user_ip=$(who am i | awk '{print $NF}' | sed 's/(//g' | sed 's/)//g')
    if [[ -z "${user_ip}" ]]; then
        user_ip=localhost
    fi
    user_name=$(whoami)
    host_name=$(hostname)
    append_text="[$(date "+%Y-%m-%d %H:%M:%S")][$user_ip][$user_name][$host_name]:"
    echo "$append_text" >> "$path"
    echo "$new_version_info" >> "$path"
    echo "Install MindX SDK successful." >> "$path"
}

deployment_log_rotate() {
    if [ -L "$info_record_path" ]; then
        echo "The directory path of deployment.log cannot be a symlink." >&2
        exit 1
    fi
    if [[ ! -d "$info_record_path" ]]; then
        mkdir -p "$info_record_path"
        chmod 750 "$info_record_path"
    fi
    record_file_path="$info_record_path"/"$info_record_file"
    if [ -L "$record_file_path" ]; then
        echo "The deployment.log cannot be a symlink." >&2
        exit 1
    fi
    if [[ ! -f "$record_file_path" ]]; then
        touch "$record_file_path" 2> /dev/null
    fi
    record_file_path_bk="$info_record_path"/"$info_record_file".bk
    if [ -L "$record_file_path_bk" ]; then
        echo "The deployment.log.bk cannot be a symlink." >&2
        exit 1
    fi
    log_size=$(find $record_file_path -exec ls -l {} \; | awk '{ print $5 }')
    if [[ "${log_size}" -ge "${LOG_SIZE_THRESHOLD}" ]]; then
        mv -f "$record_file_path" "$record_file_path_bk"
        touch "$record_file_path" 2> /dev/null
        chmod 400 "$record_file_path_bk"
    fi
    chmod 600 "$record_file_path"
}

# record --install/--upgrade info 
record_operator_info() {
    deployment_log_rotate
    find "$info_record_path" -type f -exec chmod 750 {} +
    if test x"$install_option" = xy; then
        get_version_info 2
        save_install_info "$record_file_path"
    fi

    if test x"$upgrade" = xy; then
        get_version_info 2
        save_upgrade_info "$record_file_path"
    fi
    find "$info_record_path" -type f -exec chmod 440 {} +
}

find_acl_path_string() {
    valid_acl_path_tmp=$1
    lib64_index=0
    target_lib64_str="lib64"
    OLD_IFS="$IFS"
    IFS=":"
    array=(${LD_LIBRARY_PATH})
    for s in "${array[@]}"; do
        lib64_index=$(awk -v src_str="${s}" -v dst_str="$target_lib64_str" 'BEGIN{print index(src_str,dst_str)}')
        if [[ $lib64_index != 0 ]]; then
            if [[ -f "${s}/libascendcl.so" ]]; then
                echo "valid acl path(${s}), continue the installation..."
                break
            else
                lib64_index=0
            fi
        fi
    done
    if [[ $lib64_index != 0 ]]; then
        valid_acl_path_tmp=1
    fi
    return $valid_acl_path_tmp
}

acl_invalid_msg() {
    if [[ -f /usr/local/Ascend/ascend-toolkit/set_env.sh ]]; then
        print_warning "Warning: invalid acl path in LD_LIBRARY_PATH(${LD_LIBRARY_PATH}), please execute '. /usr/local/Ascend/ascend-toolkit/set_env.sh' first."
    else
        if [[ -f "$cann_path_value/ascend-toolkit/set_env.sh" ]]; then
            print_warning "Warning: invalid acl path in LD_LIBRARY_PATH(${LD_LIBRARY_PATH}), please execute '. $cann_path_value/ascend-toolkit/set_env.sh' first."
        else
            print_warning "Warning: invalid acl path in LD_LIBRARY_PATH(${LD_LIBRARY_PATH})."
        fi
    fi
}

check_acl_environment() {
    if [[ -z "${LD_LIBRARY_PATH}" ]]; then
        acl_invalid_msg
        log "Install package failed."
        exit 1
    elif [[ "${LD_LIBRARY_PATH}" == *lib64* ]]; then
        valid_acl_path=0
        find_acl_path_string $valid_acl_path
        valid_acl_path=$?
        if [[ "$valid_acl_path" == 0 ]]; then
            acl_invalid_msg
            log "Install package failed."
            exit 1
        fi
    else
        acl_invalid_msg
        log "Install package failed."
        exit 1
    fi
}

chmod_whl() {
    chd_value=$1
    if [[ -d "$install_path/mindx" ]]; then
        find "$install_path/mindx" -type d -exec chmod $chd_value {} + &> /dev/null
        find "$install_path/mindx" -type f -exec chmod $chd_value {} + &> /dev/null
    fi
}

python_version() {
    python_version_full=$(python3 --version)
    python_version_patch=${python_version_full#*Python}
    python_version_patch=${python_version_patch// /}
    python_version_minor=${python_version_patch%.*}
}

install_whl() {
    python_version
    if test x"$python_version_minor" = x3.9; then
        install_path="${HOME}/.local/lib/python3.9/site-packages/"
        if test x"$quiet" = xn; then
            echo "Find python3.9, wheel package will be installed."
        fi
    elif test x"$python_version_minor" = x3.10; then
        install_path="${HOME}/.local/lib/python3.10/site-packages/"
        if test x"$quiet" = xn; then
            echo "Find python3.10, wheel package will be installed."
        fi
    elif test x"$python_version_minor" = x3.11; then
        install_path="${HOME}/.local/lib/python3.11/site-packages/"
        if test x"$quiet" = xn; then
            echo "Find python3.11, wheel package will be installed."
        fi
    else
        echo "MxVison only support python3.9 or python3.10 or python3.11, wheel package will not be installed."
        return 127
    fi
    python_path="$sourcedir/python"
    whl_file_name=$(find $python_path -maxdepth 1 -type f -name '*.whl')
    if test x"$quiet" = xn; then
        echo "Begin to install wheel package(${whl_file_name##*/})."
    fi
    if [[ -f "$whl_file_name" ]]; then
        chmod_whl 700
        cd $python_path
        if test x"$quiet" = xy; then
            python3 -m pip install --no-index --upgrade --force-reinstall --no-dependencies "${whl_file_name##*/}" --user > /dev/null 2>&1
        else
            python3 -m pip install --no-index --upgrade --force-reinstall --no-dependencies "${whl_file_name##*/}" --user
        fi
        if test $? -ne 0; then
            echo "Install wheel package failed."
        else
            if test x"$quiet" = xn; then
                echo "Install wheel package successfully."
            fi
            # rm *.whl if install successfully.
            rm -f "$whl_file_name"
            chmod_whl 550
        fi
    else
        echo "There is no wheel package to install."
    fi
    cd - > /dev/null
}

install_op() {
    unset ASCEND_CUSTOM_OPP_PATH
    export LD_LIBRARY_PATH=/usr/lib64:${LD_LIBRARY_PATH}
    if [[ -z "${ASCEND_HOME_PATH}" ]]; then
        echo "Install dsl op failed, please set ASCEND_HOME_PATH first."
        log "Install dsl op failed, please set ASCEND_HOME_PATH first."
    else
        op_install_path=${ASCEND_HOME_PATH}
        if [[ -L "$op_install_path/opp/vendors/customize_vision" ]]; then
            echo "Install dsl op failed, ($op_install_path/opp/vendors/customize_vision) cannot be a symlink."
            log "Install dsl op failed, ($op_install_path/opp/vendors/customize_vision) cannot be a symlink."
            return
        fi

        if [[ -d "$op_install_path/opp/vendors/customize_vision" ]]; then
            acl_user="$(ls -ld "$op_install_path/opp/vendors/customize_vision" | awk -F " " '{print $3}')"
            if test x"$(whoami)" != x"$acl_user"; then
                echo "Install dsl op failed, cann owner is not current user."
                log "Install dsl op failed, cann owner is not current user."
                return
            fi
        fi

        unset DISPLAY
        bash "$sourcedir"/operators/operatorsdsl/build_out/custom_opp_*.run --nox11 > /dev/null 2>&1

        if [[ ! -d "$op_install_path/opp/vendors/customize_vision" ]]; then
            echo "Install dsl op failed, cannot find the customize_vision."
            log "Install dsl op failed, cannot find the customize_vision."
        else
            find "$op_install_path/opp/vendors/customize_vision/" -type d -exec chmod 750 {} +
            find "$op_install_path/opp/vendors/customize_vision/" -type f -name '*.py' -exec chmod 500 {} +
            find "$op_install_path/opp/vendors/customize_vision/" -type f -name '*.json' -exec chmod 640 {} +
            find "$op_install_path/opp/vendors/customize_vision/" -type f -name '*.so' -exec chmod 440 {} +
            if test x"$quiet" = xn; then
                echo "Install dsl op success."
                log "Install dsl op success."
            fi
        fi
    fi

    if [[ -d "$sourcedir/operators/operatorsdsl" ]]; then
        rm -rf "$sourcedir/operators/operatorsdsl"
    fi

    if [[ -f "$sourcedir/filelist.txt" ]]; then
        sed -i '/operatorsdsl*/d' "$sourcedir/filelist.txt"
    fi
}

install_ascendc() {
    export LD_LIBRARY_PATH=/usr/lib64:${LD_LIBRARY_PATH}
    op_install_path="$sourcedir/operators/ascendc"
    if [[ ! -d $op_install_path ]]; then
        mkdir "$op_install_path"
    fi
    cd "$sourcedir"/operators
    unset DISPLAY
    bash "$sourcedir"/operators/operatorascendc/custom_opp_*.run --install-path=${op_install_path} --nox11 > /dev/null 2>&1

    if [[ ! -d "$sourcedir/operators/ascendc/vendors/customize" ]]; then
        echo "Install op ERROR: deploy ascendc op failed."
        log "Install op ERROR: deploy ascendc op failed."
    fi

    if [[ -d "$sourcedir/operators/operatorascendc" ]]; then
        rm -rf "$sourcedir/operators/operatorascendc"
    fi

    find "$sourcedir/operators/ascendc/" -type d -exec chmod 750 {} +
    find "$sourcedir/operators/ascendc/" -type f -name '*.py' -exec chmod 500 {} +
    find "$sourcedir/operators/ascendc/" -type f -name '*.json' -exec chmod 640 {} +
    find "$sourcedir/operators/ascendc/" -type f -name '*.so' -exec chmod 440 {} +

    if [[ -d "$sourcedir/operators/packages" ]]; then
        rm -rf "$sourcedir/operators/packages"
    fi

    if [[ -d "$sourcedir/operators/ascendc/vendors/customize/op_api/include" ]]; then
        rm -rf "$sourcedir/operators/ascendc/vendors/customize/op_api/include"
    fi

    if [[ -d "$sourcedir/operators/ascendc/vendors/customize/op_impl/ai_core/tbe/customize_impl" ]]; then
        rm -rf "$sourcedir/operators/ascendc/vendors/customize/op_impl/ai_core/tbe/customize_impl"
    fi

    if [[ -d "$sourcedir/operators/ascendc/vendors/customize/op_proto/inc" ]]; then
        rm -rf "$sourcedir/operators/ascendc/vendors/customize/op_proto/inc"
    fi

    if [[ -d "$sourcedir/operators/ascendc/vendors/customize/op_api/include" ]]; then
        rm -rf "$sourcedir/operators/ascendc/vendors/customize/op_api/include"
    fi

    if [[ -f "$sourcedir/operators/help.info" ]]; then
        rm -rf "$sourcedir/operators/help.info"
    fi

    if [[ -f "$sourcedir/operators/install.sh" ]]; then
        rm -rf "$sourcedir/operators/install.sh"
    fi

    if [[ -f "$sourcedir/operators/upgrade.sh" ]]; then
        rm -rf "$sourcedir/operators/upgrade.sh"
    fi

    if [[ -d "$sourcedir/operators/ascendc/vendors/customize/bin" ]]; then
        rm -rf "$sourcedir/operators/ascendc/vendors/customize/bin"
    fi

    cd "$sourcedir"
    if [[ -f "$sourcedir/filelist.txt" ]]; then
        sed -i '/operatorascendc*/d' "$sourcedir/filelist.txt"
        find operators/ascendc/* | sort >> filelist.txt
    fi
}

get_sdk_path() {
    sdk_home_path="$1"
    # xxx/mxVision(mxVision-x.x.x) -> xxx, remove 'mxVision-x.x.x' first
    pkg_name="mxVision-"*"."*"."*
    sdk_home_path=$(echo "$sdk_home_path" | cut -d"\"" -f 2 | sed "s/\(.*\)\/$pkg_name$/\1/")
    if [ $? -ne 0 ]; then
        sdk_home_path=$(echo "$sdk_home_path" | sed "s/\(.*\)\/mxVision$/\1/")
    fi

    # delete end '/'
    if [[ ${sdk_home_path: -1} = "/" ]]; then
        sdk_home_path=${sdk_home_path%?}
    fi
}

judge_package_exists() {
    echo "Check install path ("$sdk_home_path")."
    if [[ -f "$sdk_home_path"/mxVision/bin/uninstall.sh ]]; then
        # get old version info
        get_version_info 1
    else
        echo "There is no package to upgrade, please check it."
        exit 1
    fi
}

check_path() {
    file_path="$1"
    if [[ ! -f "${file_path}" ]]; then
        echo "The file: ${file_path} is not exist."
        log "The file: ${file_path} is not exist."
        exit 1
    fi
    if [[ -L "${file_path}" ]]; then
        echo "The file: ${file_path} is a link, invalid file."
        log "The file: ${file_path} is a link, invalid file."
        exit 1
    fi
    current_user=$(whoami)
    file_owner=$(stat -c %U "${file_path}")
    if [[ "${current_user}" != "${file_owner}" ]]; then
        echo "The owner of file: ${file_path} is different with current user, invalid file."
        log "The owner of file: ${file_path} is different with current user, invalid file."
        exit 1
    fi
}

print_warning() {
    echo -e "\033[31m$1 \033[0m"
}

check_platform() {
    plat="$(uname -m)"
    result="$(echo $plat | grep $archName)"

    if test x"$result" = x""; then
        print_warning "Warning: Platform($plat) mismatch for $0, please check it."
    fi
}

log() {
    deployment_log_rotate
    record_file_path="$info_record_path"/"$info_record_file"
    chmod 640 "$record_file_path"
    user_ip=$(who am i | awk '{print $NF}' | sed 's/(//g' | sed 's/)//g')
    if [[ -z "${user_ip}" ]]; then
        user_ip=localhost
    fi
    user_name=$(whoami)
    host_name=$(hostname)
    append_text="[$(date "+%Y-%m-%d %H:%M:%S")][$user_ip][$user_name][$host_name]:"
    {
        echo -n "$append_text "
        echo "$1"
    } >> "$record_file_path"
    chmod 440 "$record_file_path"
}

upgrade_package_content() {
    # except config directory for now.
    find "$sdk_home_path/mxVision/" -type d -exec chmod 750 {} +
    find "$sdk_home_path/mxVision/" -type f -exec chmod 750 {} +

    # dir
    cp -rf $upgrade_tmp_path/bin/* $sdk_home_path/mxVision/bin/
    cp -rf $upgrade_tmp_path/lib/* $sdk_home_path/mxVision/lib/
    cp -rf $upgrade_tmp_path/include/* $sdk_home_path/mxVision/include/
    cp -rf $upgrade_tmp_path/opensource/* $sdk_home_path/mxVision/opensource/
    cp -rf $upgrade_tmp_path/operators/* $sdk_home_path/mxVision/operators/
    cp -rf $upgrade_tmp_path/python/* $sdk_home_path/mxVision/python/
    cp -rf $upgrade_tmp_path/toolkit/* $sdk_home_path/mxVision/toolkit/
    cp -rf $upgrade_tmp_path/samples/* $sdk_home_path/mxVision/samples/

    # file
    cp -rf $upgrade_tmp_path/version.info $sdk_home_path/mxVision/
    cp -rf $upgrade_tmp_path/set_env.sh $sdk_home_path/mxVision/
    cp -rf $upgrade_tmp_path/filelist.txt $sdk_home_path/mxVision/

    version_content=$(head -n +1 "$upgrade_tmp_path/version.info")
    version_number=$(echo "$version_content" | cut -d ":" -f 2)
    package_name="mxVision-$version_number"

    # rename package name
    if [[ ! -d "$sdk_home_path/$package_name" ]]; then
        cd $sdk_home_path > /dev/null
        mv $sdk_home_path/mxVision-* $sdk_home_path/$package_name
        rm mxVision
        ln -snf $package_name mxVision
        cd - > /dev/null
    fi

    modify_run_permission $sdk_home_path/mxVision
}

install_package_content() {
    # except config directory for now.
    origin_path=$(pwd)
    version_content=$(head -n +1 "$origin_path/version.info")
    version_number=$(echo "$version_content" | cut -d ":" -f 2)
    package_name="mxVision-$version_number"
    new_dir="$targetdir/$package_name"
    mkdir -p $new_dir

    # dir
    cp -rf $origin_path/bin $new_dir/
    cp -rf $origin_path/lib $new_dir/
    cp -rf $origin_path/include $new_dir/
    cp -rf $origin_path/opensource $new_dir/
    cp -rf $origin_path/operators $new_dir/
    cp -rf $origin_path/python $new_dir/
    cp -rf $origin_path/toolkit $new_dir/
    cp -rf $origin_path/samples $new_dir/
    cp -rf $origin_path/config $new_dir/

    # file
    cp -rf $origin_path/version.info $new_dir/
    cp -rf $origin_path/set_env.sh $new_dir/
    cp -rf $origin_path/filelist.txt $new_dir/

    cd $targetdir > /dev/null
    # rename package name
    ln -snf $package_name mxVision
    cd - > /dev/null
    modify_run_permission $targetdir/mxVision
}

print_quiet() {
    if test x"$quiet" = xn; then
        echo "$1"
    fi
}

check_target_dir() {
    if [[ "${USER_PWD}" =~ [^a-zA-Z0-9_./-] ]]; then
        log "Current path contains invalid char, please check the package path. $1 package failed."
        echo "Current path contains invalid char, please check the package path. $1 package failed."
        exit 1
    fi
    if [[ "$targetdir" =~ [^a-zA-Z0-9_./-] ]]; then
        log "SDK install dir contains invalid char, please check the installed path. $1 package failed."
        echo "SDK install dir contains invalid char, please check the installed path. $1 package failed."
        exit 1
    fi
}

check_process() {
    process="$1"
    failed=0
    for pid in $(pgrep "$process"); do
        failed=1
    done
    echo $failed
}

check_streamserver() {
    current_user=$(whoami)
    process_id=$(pgrep -u "$current_user" -f "python3 streamserverSourceCode/main.py")
    if [ -n "$process_id" ]; then
        echo 1
    else
        echo 0
    fi
}

uninstall_op() {
    if test x"$ASCEND_HOME_PATH" != x; then
        op_file_path="$ASCEND_HOME_PATH/opp/vendors/customize_vision"
        if [[ -L "$op_file_path" ]]; then
            echo "($op_file_path) cannot be a symlink, uninstall dsl op failed."
            log "($op_file_path) cannot be a symlink, uninstall dsl op failed."
            return
        fi
        if [[ ! -d "$op_file_path" ]]; then
            log 'No exists dsl op to uninstall.'
            echo 'No exists dsl op to uninstall.'
            return
        fi
        acl_user="$(ls -ld "$op_file_path/" | awk -F " " '{print $3}')"
        print_quiet "The owner of file($op_file_path) is $acl_user."
        if test x"$(whoami)" != x"$acl_user"; then
            echo "Cann owner is not current user, uninstall dsl op failed."
            log "Cann owner is not current user, uninstall dsl op failed."
            return
        fi
        rm -rf $op_file_path
        log 'Uninstall op files successfully.'
        echo 'Uninstall op files successfully.'
    else
        log 'Uninstall dsl op failed, please set ASCEND_HOME_PATH first.'
        echo "Uninstall dsl op failed, please set ASCEND_HOME_PATH first."
    fi
}

uninstall_pkg() {
    check_target_dir "Uninstall"
    run_path_suffix="$targetdir/mxVision"-*.*.*
    if [ -L $targetdir/mxVision ] || [ -d $run_path_suffix ]; then
        new_version_info=$(sed -n 1p $targetdir/mxVision/version.info)
        # check exist services
        if [[ $(check_process "mxStreammanager") -ne 0 ]] || [[ $(check_process "mxmfCommander") -ne 0 ]] || [[ $(check_streamserver) -ne 0 ]]; then
            echo "Some services(mxStreammanager/mxmfCommander/StreamServer) are still running. Uninstallation aborted."
            log "Uninstall SDK package failed."
            echo "Uninstall SDK package failed."
            return
        fi
        # delete whl
        delete_wheel_pkg

        # delete sdk pkg
        find "$targetdir" -maxdepth 1 -type d -name mxVision-* -exec chmod u+w -R {} \;
        rm -rf "$targetdir"/mxVision* > /dev/null

        # delete op install files
        uninstall_op

        # record uninstall info
        deployment_log_rotate
        find "$record_file_path" -type f -exec chmod 640 {} +
        user_ip=$(who am i | awk '{print $NF}' | sed 's/(//g' | sed 's/)//g')
        if [[ -z "${user_ip}" ]]; then
            user_ip=localhost
        fi
        user_name=$(whoami)
        host_name=$(hostname)
        append_text="[$(date "+%Y-%m-%d %H:%M:%S")][$user_ip][$user_name][$host_name]:"
        echo "$append_text" >> "$record_file_path"
        echo "$new_version_info" >> "$record_file_path"
        echo "Uninstall SDK package successful." >> "$record_file_path"
        find "$record_file_path" -type f -exec chmod 440 {} +
        echo "Uninstall SDK package successful."
    else
        echo "No exists SDK package to uninstall."
        log "No exists SDK package to uninstall."
    fi
}

get_acl_owner() {
    if test x"$cann_path_flag" = x"y"; then
        if [[ -d "$cann_path_value" ]]; then
            get_acl_path $cann_path_value
            if [[ -f "$acl_owner_path/$acl_dll_file" ]]; then
                acl_user="$(ls -l "$acl_owner_path/" | sed -n "/$acl_dll_file$/p" | awk -F " " '{print $3}')"
                print_quiet "The owner of file($acl_owner_path/$acl_dll_file) is $acl_user."
            else
                echo "Please enter correct cann path."
                log "Please enter correct cann path."
                exit 127
            fi
        else
            echo "The directory("$cann_path_value") is not exists, please check it."
            log "The directory("$cann_path_value") is not exists, please check it."
            exit 127
        fi
    else
        # get default cann path owner
        get_acl_versions_path
        if [[ -d "$acl_owner_path" ]]; then
            acl_user="$(ls -l "$acl_owner_path/" | sed -n "/$acl_dll_file$/p" | awk -F " " '{print $3}')"
            print_quiet "The owner of file($acl_owner_path/$acl_dll_file) is $acl_user."
            return
        fi

        echo "Please use the '--cann-path=' parameter to specify the cann path."
        log "Please use the '--cann-path=' parameter to specify the cann path."
        exit 127
    fi
}

check_acl_owner() {
    get_acl_owner
    if test x"$(whoami)" != x"$acl_user"; then
        echo "Installation failed, cann owner is not current user."
        log "Installation failed, cann owner is not current user."
        exit 127
    fi
}

check_package_exists() {
    path="$1"
    run_path_suffix="$path/mxVision"-*.*.*
    if [ -L "$path/mxVision" ] || [ -d $run_path_suffix ]; then
        log "Package has been installed, no need to install."
        echo "Package has been installed in path($path). Please uninstall or upgrade the software."
        exit 1
    fi
}

script_total_param="$*"
parse_script_args() {
    while [ -n "$*" ]; do
        case "$1" in
            --quiet)
                check_platform
                quiet=y
                print_eula_info=y
                shift
                ;;
            --check)
                exit 0
                ;;
            --nox11)
                isinputnox11=y
                shift
                ;;
            --version)
                print_version_number=y
                targetdir=$(pwd)
                shift
                ;;
            --install)
                check_platform
                print_eula_info=y
                install_option=y
                if test x"$transfer_param_install_path" != xy; then
                    targetdir="${USER_PWD}"
                fi
                sdk_home_path="${targetdir}/mxVision"
                shift
                ;;
            --install-path=*)
                transfer_param_install_path=y
                targetdir="${1#*=}"
                # if relative path, to absolute.
                if [[ $targetdir != '/'* ]]; then
                    targetdir="${USER_PWD}/$targetdir"
                fi
                targetdir=$(readlink -f $targetdir)
                sdk_home_path="${targetdir}/mxVision"
                shift
                ;;
            --install-path)
                echo "Installation failed, --install-path must be assigned a value, use --install-path="
                log "Installation failed, --install-path must be assigned a value, use --install-path="
                exit 1
                ;;
            --uninstall)
                uninstall_flag=y
                targetdir="${USER_PWD}"
                shift
                ;;
            --cann-path=*)
                cann_path_flag=y
                cann_path_value="${1#*=}"
                shift
                ;;
            --cann-path)
                echo "Installation failed, --cann-path must be assigned a value, use --cann-path="
                log "Installation failed, --cann-path must be assigned a value, use --cann-path="
                exit 1
                ;;
            --upgrade)
                check_platform
                transfer_param_upgrade=y
                print_eula_info=y
                upgrade=y
                if test x"$transfer_param_install_path" != xy; then
                    targetdir="${USER_PWD}"
                fi
                sdk_home_path="${targetdir}/mxVision"
                shift
                ;;
            --choose-gcc=*)
                isinputchoosegcc=y
                choose_gcc=$(echo $1 | cut -d"=" -f2)
                shift
                ;;
            *)
                shift
                ;;
        esac
    done
}

check_script_args() {
    if [ $1 -lt 3 ]; then
        echo "Installation failed, please check input parameters. Use --help for more help."
        log "Installation failed, please check input parameters. Use --help for more help."
        exit 1
    fi
    # params should not be null
    if [[ -z "$script_total_param" ]]; then
        echo "Installation failed, please check input parameters."
        log "Installation failed, please check input parameters."
        exit 1
    fi

    # --install-path should not be together with --uninstall
    if test x"$uninstall_flag" = x"y" && test x"$transfer_param_install_path" = x"y"; then
        echo "Installation failed, --uninstall cannot be used with --install-path."
        log "Installation failed, --uninstall cannot be used with --install-path."
        exit 1
    fi
    # --version should not be together with  --install/--upgrade
    if test x"$print_version_number" = x"y"; then
        if test x"$install_option" = x"y" || test x"$transfer_param_upgrade" = x"y"; then
            echo "Installation failed, --version cannot be used with --install/--upgrade."
            log "Installation failed, --version cannot be used with --install/--upgrade."
            exit 1
        fi
    fi
    # --install should not be together with --upgrade/--uninstall
    if test x"$install_option" = x"y"; then
        if test x"$uninstall_flag" = x"y" || test x"$transfer_param_upgrade" = x"y"; then
            echo "Installation failed, --install cannot be used with --upgrade/--uninstall."
            log "Installation failed, --install cannot be used with --upgrade/--uninstall."
            exit 1
        fi
    fi
    # --install-path should be together with --install/--upgrade
    if test x"$transfer_param_install_path" = x"y"; then
        if test x"$install_option" = x"n" && test x"$transfer_param_upgrade" = x"n"; then
            echo "Missing --install/--upgrade parameter. Please specify the --install/--upgrade parameter."
            log "Missing --install/--upgrade parameter. Please specify the --install/--upgrade parameter."
            exit 1
        fi
    fi
    # --quiet should be together with --install/--upgrade
    if test x"$quiet" = x"y"; then
        if test x"$install_option" = x"n" && test x"$transfer_param_upgrade" = x"n"; then
            echo "Missing --install/--upgrade parameter. Please specify the --install/--upgrade parameter."
            log "Missing --install/--upgrade parameter. Please specify the --install/--upgrade parameter."
            exit 1
        fi
    fi
    # --choose-gcc/--nox11 should be together with --install/--upgrade, but be ignored when uninstalling
    if test x"$uninstall_flag" = x"n"; then
        if test x"$isinputchoosegcc" = x"y" || test x"$isinputnox11" = x"y"; then
            if test x"$transfer_param_upgrade" = x"n" && test x"$install_option" = x"n"; then
                echo "Missing --install/--upgrade parameter. Please specify the --install/--upgrade parameter."
                log "Missing --install/--upgrade parameter. Please specify the --install/--upgrade parameter."
                exit 1
            fi
        fi
    fi
    # --choose-gcc should be 0 or 1
    if test x"$choose_gcc" != x"1" && test x"$choose_gcc" != x"0"; then
        echo "The parameter of choose-gcc is invalid, please try again."
        log "The parameter of choose-gcc is invalid, please try again."
        exit 1
    fi
    # --cann-path should be together with --install and should be the same as ASCEND_HOME_PATH,
    # but be ignored when uninstalling
    if test x"$uninstall_flag" = x"n" && test x"$cann_path_flag" = x"y"; then
        if test x"$install_option" = x"n" && test x"$transfer_param_upgrade" = x"n"; then
            echo "Missing --install/--upgrade parameter. Please specify the --install/--upgrade parameter."
            log "Missing --install/--upgrade parameter. Please specify the --install/--upgrade parameter."
            exit 1
        fi
        if test x"$ASCEND_HOME_PATH" != x; then
            get_acl_path $ASCEND_HOME_PATH
            tmp_ascend_path=$acl_owner_path
            get_acl_path $cann_path_value
            if [[ ! "$tmp_ascend_path" -ef "$acl_owner_path" ]]; then
                echo "'--cann-path=' is different with ASCEND_HOME_PATH: $ASCEND_HOME_PATH. Please check."
                log "'--cann-path=' is different with ASCEND_HOME_PATH: $ASCEND_HOME_PATH. Please check."
                exit 1
            fi
        fi
    fi
    if test x"$install_option" = x"y"; then
        check_package_exists $targetdir
        delete_wheel_pkg
        check_target_dir
    fi
    if test x"$install_option" = x"y" || test x"$quiet" = x"y"; then
        check_acl_owner
        check_acl_environment
    fi
    if test x"$upgrade" = xy; then
        get_sdk_path "$sdk_home_path"
        judge_package_exists
        check_target_dir
    fi
}

# parse script args
parse_script_args "$@"

# check script args
check_script_args "$#"

# handle --uninstall
if test x"$uninstall_flag" = xy; then
    uninstall_pkg
    exit 0
fi

# handle --version
if test x"$print_version_number" = xy; then
    versioninformation="version.info"
    cat "$versioninformation"
    exit 0
fi

if test x"$transfer_param_install_path" = xn && test x"$upgrade" = xn && test x"$print_version_number" = xn; then
    targetdir="${USER_PWD}"
fi

# handle upgrade
if test x"$upgrade" = xy; then
    echo "Found an existing installation."
    echo -n "Do you want to upgrade to a newer version provided by this package? [Y/n] "
    unset doupgrade
    IFS= read -n1 doupgrade
    doupgrade=${doupgrade,,}
    echo ""
    while [[ x"$doupgrade" != "xy" ]] && [[ x"$doupgrade" != "xn" ]] && [[ x"$doupgrade" != "x" ]]; do
        echo -n "Please answer \"y\" or \"n\". Default is \"Y\" if you press enter directly: "
        IFS= read -n1 doupgrade
        doupgrade=${doupgrade,,}
        echo ""
    done
    if [[ x"$doupgrade" == "xn" ]]; then
        log "Upgrade cancelled, nothing changed."
        echo "Upgradation cancelled. Nothing changed."
        exit 0
    fi
fi

# --version do not have to show info.
if test x"$upgrade" = xy || test x"$install_option" = xy; then
    if test x"$choose_gcc" = x"1"; then
        echo 'Installing the compiler version of gcc4.8.5 ...'
        log 'Installing the compiler version of gcc4.8.5 ...'
    else
        echo 'Installing the compiler version of devtoolset-7 ...'
        log 'Installing the compiler version of devtoolset-7 ...'
    fi
    install_whl
    install_op
    install_ascendc
    if test x"$upgrade" = xy; then
        upgrade_package_content
    fi
    if test x"$install_option" = xy; then
        install_package_content
    fi
    if test x"$quiet" = xn; then
        echo "The installation is successfully, please execute '. $targetdir/mxVision/set_env.sh' to activate environment variables."
        echo "Tips: If want to support the OSD features, please execute the operators script($targetdir/mxVision/operators/opencvosd/generate_osd_om.sh) to generate model file."
    fi
fi

record_operator_info
