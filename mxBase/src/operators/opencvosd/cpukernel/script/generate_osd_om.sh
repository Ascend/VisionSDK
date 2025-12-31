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
# Description: Displays the environment variables related to the Ascend.
# Author: MindX SDK
# Create: 2020
# History: NA


script_path=$(cd $(dirname $0); pwd)
info_record_path="${HOME}/log/mindxsdk"
info_record_file="deployment.log"
log_size_threshold=1024000

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

record_info() {
  cd "${script_path}/../.." || {
    warn "Where is the MindX SDK?"
    exit 255
  }
  ms_deployment_log_rotate
  deployment_path="$info_record_path"/"$info_record_file"
  find "$deployment_path" -type f -exec chmod 750 {} +
  user_ip=$(who am i | awk '{print $NF}' | sed 's/(//g' | sed 's/)//g')
  if [[ -z "${user_ip}" ]]; then
    user_ip=localhost
  fi
  user_name=$(whoami)
  host_name=$(hostname)
  append_text="[$(date "+%Y-%m-%d %H:%M:%S")][$user_ip][$user_name][$host_name] :"
  echo "$append_text" >> "$deployment_path"
  version_info=$(sed -n 1p "$(pwd)"/version.info)
  echo "$version_info" >> "$deployment_path"
  echo "Generate osd om $1." >> "$deployment_path"
  find "$deployment_path" -type f -exec chmod 440 {} +
}

function check_env()
{
    curr_user_id=$(id -u)
    if [[ "$curr_user_id" = "0" ]]; then
      path_env=/usr/local/Ascend/ascend-toolkit/set_env.sh
    else
      curr_user_name=$(id -un)
      path_env="${HOME}/Ascend/ascend-toolkit/set_env.sh"
      if [[ -f "${path_env}" ]]; then
        owner=$(ls -l "${path_env}" | awk '{print $3}')
        if [[ "${curr_user_name}" != "$owner" ]]; then
          echo "Generate failed: Invalid owner of \$HOME/Ascend/ascend-toolkit/set_env.sh, please check."
          record_info "failed"
          exit 1
        fi
      else
        path_env=/usr/local/Ascend/ascend-toolkit/set_env.sh
        echo "\$HOME/Ascend/ascend-toolkit/set_env.sh not exists, use default ${path_env} instead."
      fi
    fi
    if [[ -f "${path_env}" ]]; then
      . "${path_env}"
    else
      echo "Please execute \'. xxx/Ascend/ascend-toolkit/set_env.sh\' to activate environment variables."
      record_info "failed"
      exit 1
    fi

    # set ASCEND_HOME to /usr/local/Ascend when it was not specified by user
    if [ ! "${ASCEND_HOME}" ]; then
      if [[ "$curr_user_id" = "0" ]]; then
        export ASCEND_HOME=/usr/local/Ascend
        echo "Set ASCEND_HOME to the default value: ${ASCEND_HOME}"
      else
        export ASCEND_HOME="${HOME}/Ascend"
      fi
    fi

    # set ASCEND_VERSION to ascend-toolkit/latest when it was not specified by user
    if [ ! "${ASCEND_VERSION}" ]; then
        export ASCEND_VERSION=ascend-toolkit/latest
        echo "Set ASCEND_VERSION to the default value: ${ASCEND_VERSION}"
    fi

    which atc &> /dev/null
    if [ $? -ne 0 ]; then
        echo "Command atc is not available, please confirm whether it was installed correctly, and environment variables PATH and PYTHONPATH are set correctly according to the user guide."
        record_info "failed"
        exit 1
    fi
}

check_env

cd ${script_path}
# copy custom aicpu library and configure file to opp directory
if [[ -d "${ASCEND_HOME}/${ASCEND_VERSION}/opp/vendors" && -f "${ASCEND_HOME}/${ASCEND_VERSION}/opp/vendors/config.ini" ]]; then
    # Adapt from CANN-6.0.0
    if [ -d "${ASCEND_HOME}/${ASCEND_VERSION}/opp/vendors/opencvosd" ]; then
        echo "[Warning] opencvosd directory is already exists."
    fi

    if cat "${ASCEND_HOME}/${ASCEND_VERSION}/opp/vendors/config.ini" |grep "opencvosd" &> /dev/null
    then
        echo "[Warning] opencvosd directory has been set at config.ini."
    else
        sed -i '$ s/$/,opencvosd/' "${ASCEND_HOME}/${ASCEND_VERSION}/opp/vendors/config.ini"
        if [ $? -ne 0 ]; then
            echo "[Error] Failed to set opencvosd directory at config.ini."
        fi
    fi

    if [ ! -d "${ASCEND_HOME}/${ASCEND_VERSION}/opp/vendors/opencvosd/op_impl/cpu" ]; then
        mkdir -p "${ASCEND_HOME}/${ASCEND_VERSION}/opp/vendors/opencvosd/op_impl/cpu"
        if [ $? -ne 0 ]; then
            echo "[Error] Please confirm the directory .../opp/vendors path is correct, and the user has the write permission of it."
            record_info "failed"
            exit 1
        fi
    fi
else
    if [ ! -d "${ASCEND_HOME}/${ASCEND_VERSION}/opp/op_impl/custom/cpu" ]; then
        mkdir -p "${ASCEND_HOME}/${ASCEND_VERSION}/opp/op_impl/custom/cpu"
        if [ $? -ne 0 ]; then
            echo "[Error] Please confirm the directory .../opp/op_impl path is correct, and the user has the write permission of it."
            record_info "failed"
            exit 1
        fi
    fi
fi

if [[ -d "${ASCEND_HOME}/${ASCEND_VERSION}/opp/vendors" && -f "${ASCEND_HOME}/${ASCEND_VERSION}/opp/vendors/config.ini" ]]; then
    # Adapt from CANN-6.0.0
    if [ -d "./cust_aicpu/aicpu_kernel/custom_impl" ]; then
        mv "./cust_aicpu/aicpu_kernel/custom_impl" "./cust_aicpu/aicpu_kernel/impl" >& /dev/null
    fi
    cp -rf cust_aicpu/* "${ASCEND_HOME}/${ASCEND_VERSION}/opp/vendors/opencvosd/op_impl/cpu/"
    if [ $? -ne 0 ]; then
        echo "[Error] Please confirm the directory .../opp/op_impl/cpu/ is correct, and the user has the write permission of it."
        record_info "failed"
        exit 1
    fi
else
    if [ ! -d "./cust_aicpu/aicpu_kernel/custom_impl" ]; then
        cp -rf ./cust_aicpu/aicpu_kernel/impl ./cust_aicpu/aicpu_kernel/custom_impl
    fi
    cp -rf cust_aicpu/* "${ASCEND_HOME}/${ASCEND_VERSION}/opp/op_impl/custom/cpu/"
    if [ $? -ne 0 ]; then
        echo "[Error] Please confirm the directory .../opp/op_impl/custom/cpu/ is correct, and the user has the write permission of it."
        record_info "failed"
        exit 1
    fi
fi

chip_version=$(npu-smi info | awk '{print $3}' | grep -m 1 310)
soc_version=Ascend${chip_version:0:5}

# use atc to generate om
atc --singleop=./config/opencv_op.json --soc_version="${soc_version}" --output=./
ret=$?
if [ ${ret} -eq 0 ]; then
    echo "The model has been successfully converted to om, please get it under ${script_path}."
    record_info "successfully"
    exit 0
else
    record_info "failed"
    exit ${ret}
fi