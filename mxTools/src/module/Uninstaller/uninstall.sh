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
# Description: SDK uninstallation tool.
# Author: MindX SDK
# Create: 2020
# History: NA

# Simple log helper functions
tag="Uninst  "
info_record_path="${HOME}/log/mindxsdk"
info_record_file="deployment.log"
log_size_threshold=1024000

info() { echo -e "\033[1;34m[INFO ][$tag] $1\033[1;37m" ; }
warn() { echo >&2 -e "\033[1;31m[WARN ][$tag] $1\033[1;37m" ; }

CUR_PATH=$(cd "$(dirname "$0")" || { warn "Failed to check path/to/build.sh" ; exit ; } ; pwd)

check_services() {
  info "Checking services..."
  current_user=$(whoami)
  process_id=$(pgrep -u "$current_user" -f "python3 streamserverSourceCode/main.py")
  if [ -n "$process_id" ]; then
    warn "StreamServer is still running. Uninstallation aborted."
    info 'Uninstall MindX SDK failed.'
    record_uninstall_info "failed"
    exit 255
  else
    info "No service is running. Uninstallation going."
  fi
}

del_whl() {
  mindx=$(python3 -m pip list --no-index|grep mindx)
  if [[ -n "$mindx" ]]; then
    echo y | python3 -m pip uninstall mindx &>/dev/null
    info 'Uninstall wheel package successfully.'
  fi
}

del_op()
{
  uninstall_op_flag=""
  if test x"$ASCEND_HOME_PATH" != x; then
      op_file_path="$ASCEND_HOME_PATH/opp/vendors/customize_vision"
      if [[ -L "$op_file_path" ]];then
        echo "($op_file_path) cannot be a symlink, uninstall dsl op failed."
        return
      fi
      if [[ -d "$op_file_path" ]];then
        acl_user="$(ls -ld "$op_file_path/" | awk -F " " '{print $3}')"
      fi
      if test x"$(whoami)" != x"$acl_user"; then
        echo "Cann owner is not current user, uninstall dsl op failed."
        return
      fi
      rm -rf $op_file_path
      echo 'Uninstall op files successfully.'
      uninstall_op_flag="yes"
  else
      echo "Uninstall dsl op failed, please set ASCEND_HOME_PATH first."
  fi
}

get_run_path() {
  pwd_path="$(pwd)"
  if [[ "$pwd_path" = *'mxManufacture' ]] || [[ "$pwd_path" = *'mxManufacture-'*'.'*'.'* ]];then
    suffix='mxManufacture'
  elif [[ "$pwd_path" = *'mxVision' ]] || [[ "$pwd_path" = *'mxVision-'*'.'*'.'* ]]; then
    suffix='mxVision'
  fi
  cd ..
  del_path="$(pwd)"/"$suffix"
}

ms_deployment_log_rotate() {
  if [ -L "$info_record_path" ]; then
    echo "The directory path of deployment.log cannot be a symlink." >&2
    exit 1
  fi
  if [[ ! -d "$info_record_path" ]];then
    mkdir -p "$info_record_path"
    chmod 750 "$info_record_path"
  fi
  record_file_path="$info_record_path"/"$info_record_file"
  if [ -L "$record_file_path" ]; then
    echo "The deployment.log cannot be a symlink." >&2
    exit 1
  fi
  if [[ ! -f "$record_file_path" ]];then
    touch "$record_file_path" 2>/dev/null
  fi
  record_file_path_bk="$info_record_path"/"$info_record_file".bk
  if [ -L "$record_file_path_bk" ]; then
    echo "The deployment.log.bk cannot be a symlink." >&2
    exit 1
  fi
  log_size=$(find "$record_file_path" -exec ls -l {} \; | awk '{ print $5 }')
  if [[ "${log_size}" -ge "${log_size_threshold}" ]];then
    mv -f "$record_file_path" "$record_file_path_bk"
    touch "$record_file_path" 2>/dev/null
    chmod 400 "$record_file_path_bk"
  fi
  chmod 600 "$record_file_path"
}

record_uninstall_info() {
  cd "${CUR_PATH}/.." || {
    warn "Where is the MindX SDK?"
    exit 255
  }
  ms_deployment_log_rotate
  deployment_path="$info_record_path"/"$info_record_file"
  find "$deployment_path" -type f -exec chmod 640 {} +
  user_ip=$(who am i | awk '{print $NF}' | sed 's/(//g' | sed 's/)//g')
  if [[ -z ${user_ip} ]]; then
    user_ip=localhost
  fi
  user_name=$(whoami)
  host_name=$(hostname)
  append_text="[$(date "+%Y-%m-%d %H:%M:%S")][$user_ip][$user_name][$host_name] :"
  echo "$append_text" >> "$deployment_path"
  version_info=$(sed -n 1p "$(pwd)"/version.info)
  if [[ ${uninstall_op_flag} == "yes" ]]; then
     echo "Uninstall dsl op successfully" >> "$deployment_path"
  fi
  echo "$version_info" >> "$deployment_path"
  echo "Uninstall MindX SDK $1." >> "$deployment_path"
  find "$deployment_path" -type f -exec chmod 440 {} +
}

real_delete() {
  cd "${CUR_PATH}/.." || {
    warn "Where is the MindX SDK?"
    exit 255
  }
  del_whl
  get_run_path
  del_op
  if [[ -f "$del_path"/filelist.txt ]] && [[ -f "$del_path"/version.info ]];then
    chmod u+w -R $del_path/
    # Add '*' to delete mxManufacture/mxManufacture-x.x.x(mxVision/mxVision-x.x.x)
    del_path_tmp=""$del_path"*"
    record_uninstall_info "successful"
    rm -rf $del_path_tmp
    info 'Uninstall MindX SDK package successfully.'
  else
    info 'Cannot uninstall MindX SDK package.'
  fi
}

check_services
real_delete
