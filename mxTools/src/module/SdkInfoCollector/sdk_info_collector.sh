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
# Description: Information Collection Environment Configuration.
# Author: MindX SDK
# Create: 2020
# History: NA

sdk_info_collector_version=HUAWEI-MindX-sdk-01
sdk_info_collector_date="2020/11/11"
mx_sdk_home=$(echo $MX_SDK_HOME)
current_path=$(cd "$(dirname "$0")" || { echo -e "\033[31mFailed to check path/to/sdk_info_collector.sh\033[0m" ; exit ; } ; pwd)
output_folder="mindx_sdk_info"
output_folder_path="$current_path/$output_folder"
info_record_path="${HOME}/log/mindxsdk"
info_record_file="deployment.log"
file_mode=0440
dir_mode=0750
log_size_threshold=1024000
python_exec_ret=0
shell_exec_ret=0

function init_file_path()
{
    echo "Init file path"
    cd "$current_path"
    rm -rf "$output_folder"*
    mkdir "$output_folder"
    chmod 0750 $output_folder 2>/dev/null
}

function get_ascend_info()
{
    echo "Collect ascend info"
    echo "ascend info:" > "$output_folder_path"/ascend-info.txt
    npu-smi info >> "$output_folder_path"/ascend-info.txt || true
    chmod $file_mode "$output_folder_path"/ascend-info.txt 2>/dev/null
}

function get_os_info() {
    echo "Collect os system architecture info"
    echo "os system architecture:" >> "$output_folder_path"/os-info.txt
    uname -m  >> "$output_folder_path"/os-info.txt || true
    chmod $file_mode "$output_folder_path"/os-info.txt 2>/dev/null
}

function get_net_info()
{
    echo "Collect net info"
    echo "system time:" >> "$output_folder_path"/net-info.txt
    date >> "$output_folder_path"/net-info.txt
    echo "system startup time:" >> "$output_folder_path"/net-info.txt
    uptime >> "$output_folder_path"/net-info.txt || true
    echo "pcie settings:" >> "$output_folder_path"/net-info.txt
    lspci -tv >> "$output_folder_path"/net-info.txt || true
    echo "statistics on packets received and sent over the network port:" >> "$output_folder_path"/net-info.txt
    ifconfig -a >> "$output_folder_path"/net-info.txt || true
    echo "network port link status:" >> "$output_folder_path"/net-info.txt
    ip link show >> "$output_folder_path"/net-info.txt || true
    chmod $file_mode "$output_folder_path"/net-info.txt 2>/dev/null
}

function get_sdk_env_info() {
    echo "Collect env info"
    echo "env info:" > "$output_folder_path"/env-info.txt
    echo "MX_SDK_HOME=${MX_SDK_HOME}" >> "$output_folder_path"/env-info.txt
    echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}" >> "$output_folder_path"/env-info.txt
    echo "PYTHONPATH=${PYTHONPATH}" >> "$output_folder_path"/env-info.txt
    chmod $file_mode "$output_folder_path"/env-info.txt 2>/dev/null
}

function print() {
    case "$1" in
      31) echo -e "\033[31m$2\033[0m" ;; # red
      32) echo -e "\033[32m$2\033[0m" ;; # green
      *) echo $2 ;;
    esac
}

function get_sdk_version() {
    echo "Collect MindX-sdk version info"
    file="$mx_sdk_home"/version.info
    if [ -L ${file} ] || [ ! -O ${file} ]; then
      print 31 "The version.info is a link or the owner is different with user, skip sdk version info log collection."
      shell_exec_ret=1
      return
    fi
    if [ -e "$file" ]
    then
      cp "$file" "$output_folder_path" 2>/dev/null
      chmod $file_mode "$output_folder_path"/version.info 2>/dev/null || true
    else
      print 31 "$file. No such file."
    fi
}

function get_3rdparty_version() {
    echo "Collect 3rdparty version info"
    file_3rd="$mx_sdk_home"/opensource/3rdparty.txt
    if [ -L ${file} ] || [ ! -O ${file} ]; then
      print 31 "The 3rdparty.txt is a link or the owner is different with user, skip 3rdparty version info log collection."
      shell_exec_ret=1
      return
    fi
    if [ -e "$file_3rd" ]
    then
      cp "$file_3rd" "$output_folder_path" 2>/dev/null
      chmod $file_mode "$output_folder_path"/3rdparty.txt 2>/dev/null || true
    else
      print 31 "$file_3rd. No such file."
    fi
}

function get_python3_version() {
    echo "Collect python3 version info"
    python_type="$(python3 --version)"
    acquire=$?
    if [ "$acquire" == 0 ]
    then
        echo "python version is $python_type"
    else
        print 31 "Python does not exist or the version is too early."
    fi
}

function tar_folder() {
    print 32 "Begin to tar $output_folder folder......"
    cd "$current_path"
    current_time=$(date +%Y%m%d_%H%M%S)
    tar_file_name="$output_folder"_"$current_time"
    tar -zcf "$tar_file_name".tar.gz "$output_folder"
    chmod $file_mode "$tar_file_name".tar.gz 2>/dev/null
    [ -n "$output_folder" ] && rm -rf "$output_folder"
    print 32 "End to tar $output_folder folder......"
    if [ ${python_exec_ret} -eq 0 ] && [ ${shell_exec_ret} -eq 0 ]; then
        print 32 "The collected information is saved in $tar_file_name.tar.gz"
    else
        print 31 "Some logs collected failed, logs that are successfully collected are saved in $tar_file_name.tar.gz"
    fi
}

function show_version() {
  echo "version: $sdk_info_collector_version $sdk_info_collector_date"
}

function show_help() {
    echo "-------------------------------------------------------------------------------"
    echo "  --help|-h|-H"
    echo "  --version|-v|-V"
    echo "-------------------------------------------------------------------------------"
    echo
    echo "-------------------------------------------------------------------------------"
    echo "  -p|-P  path to save sdk logs"
    echo "  -p logs_path | -P logs_path : logs_path must be absolute path."
    echo "-------------------------------------------------------------------------------"
    echo
    echo "-------------------------------------------------------------------------------"
    echo "  -r|-R  range of sdk log"
    echo "  default : set to obtain all log information."
    echo "  -r xd | -R xd: set to obtain log information within x days, x must be an integer."
    echo "  -r full | -R full: set to obtain all log information."
    echo "-------------------------------------------------------------------------------"
    echo
    echo "-------------------------------------------------------------------------------"
    echo "  -t|-T  type of sdk log"
    echo "  default : set to obtain all log information."
    echo "  -t d | -T d: set to obtain debug(-1) and above type log information."
    echo "  -t i | -T i: set to obtain info(0) and above type log information."
    echo "  -t w | -T w: set to obtain warn(1) and above type log information."
    echo "  -t e | -T e: set to obtain error(2) and above type log information."
    echo "  -t f | -T f: set to obtain fatal(3) and above type log information."
    echo "  -t full | -T full: set to obtain all type log information."
    echo "-------------------------------------------------------------------------------"
    echo
    echo "-------------------------------------------------------------------------------"
    echo "  -x|-X  device log"
    echo "  default : not to obtain device log files."
    echo "  -x n | -X n: set to obtain the number of n device log files, n must be an integer."
    echo "  -x n1,n2,n3 | -X n1,n2,n3: set to obtain the number of n1, n2, n3 device log files, n1, n2, n3 must be an integer."
    echo "  -x -1 | -X -1: set to obtain all device log information."
    echo "-------------------------------------------------------------------------------"
    echo
    echo "-------------------------------------------------------------------------------"
    echo "  NOTE:"
    echo "  This tool will create collect info files in the current Mindx-sdk directory."
    echo "  Please send files to HUAWEI support engineer."
    echo "-------------------------------------------------------------------------------"
    echo
}

handle_arguments() {
  logs_path=""
  logs_range="full"
  logs_type="full"
  device="-2"
  while getopts ":HhVvR:r:T:t:P:p:X:x:-:" opt; do
    case "$opt" in
      H|h)
        show_help
        exit 0 ;;
      V|v)
        show_version
        exit 0 ;;
      R|r)
        logs_range="$OPTARG" ;;
      T|t)
        logs_type="$OPTARG" ;;
      P|p)
        logs_path="$OPTARG" ;;
      X|x)
        device="$OPTARG" ;;
      -)
        case "${OPTARG%=*}" in
          help)
            show_help
            exit 0 ;;
          version)
            show_version
            exit 0 ;;
          *)
            print 31 "Unknown option ${OPTARG%=*}."; exit 3;
            ;;
        esac ;;
      *)
        print 31 "Unknown option $OPTARG."; exit 3;
        ;;
    esac
  done
}

function chmod_mode() {
    chmod $file_mode $output_folder/logs/*error* 2>/dev/null || true
    chmod $file_mode $output_folder/logs/*info* 2>/dev/null || true
    chmod $file_mode $output_folder/logs/*warn* 2>/dev/null || true
    chmod $file_mode $output_folder/npu-slog/device*/device*log 2>/dev/null || true
}

function collect_info() {
    if ! ( [ -e "$mx_sdk_home"/bin/sdk_info_collector.sh ] && [ -e "$mx_sdk_home"/bin/sdk_info_collector.py ] )
    then
      print 31 "Please export a valid MX_SDK_HOME path."; exit 1;
    fi
    if [ ! -d "$logs_path" ]
    then
      print 31 "Please set the path that saves sdk logs."; exit 2;
    fi
    umask 0027
    init_file_path
    get_ascend_info
    get_os_info
    get_net_info
    get_sdk_env_info
    get_sdk_version
    get_3rdparty_version
    get_python3_version
    args="-r $logs_range -t $logs_type -p $logs_path -x $device"
    python3 sdk_info_collector.py $args
    python_exec_ret=$?
    chmod_mode
    tar_folder
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

function record_usage_info() {
  user_ip=$(who am i | awk '{print $NF}' | sed 's/(//g' | sed 's/)//g')
  if [[ -z ${user_ip} ]]; then
    user_ip=localhost
  fi
  user_name=$(whoami)
  host_name=$(hostname)
  ms_deployment_log_rotate
  path="$info_record_path"/"$info_record_file"
  time=$(date "+%Y-%m-%d %H:%M:%S")
  if [ ${python_exec_ret} -eq 0 ] && [ ${shell_exec_ret} -eq 0 ]; then
      mesasge="[$time] : \"sdk_info_collector.sh\" runs successfully by user: $user_name with hostname: $host_name at: $user_ip."
  else
      mesasge="[$time] : \"sdk_info_collector.sh\" runs failed by user: $user_name with hostname: $host_name at: $user_ip."
  fi
  find "$path" -type f -exec chmod 640 {} +
  if [[ -f "$path" ]]; then
    echo "$mesasge" >> "$path"
  else
    # not exists, create it.
    echo "$mesasge" > "$path"
  fi
  find "$path" -type f -exec chmod 440 {} +
}

function abnormal_exit_record_usage_info() {
  user_ip=$(who am i | awk '{print $NF}' | sed 's/(//g' | sed 's/)//g')
  if [[ -z ${user_ip} ]]; then
    user_ip=localhost
  fi
  user_name=$(whoami)
  host_name=$(hostname)
  ms_deployment_log_rotate
  path="$info_record_path"/"$info_record_file"
  time=$(date "+%Y-%m-%d %H:%M:%S")
  mesasge="[$time] : Part of \"sdk_info_collector.sh\" runs successfully by user: $user_name with hostname: $host_name at: $user_ip."
  find "$path" -type f -exec chmod 640 {} +
  if [[ -f "$path" ]]; then
    echo "$mesasge" >> "$path"
  else
    # not exists, create it.
    echo "$mesasge" > "$path"
  fi
  find "$path" -type f -exec chmod 440 {} +
}

function abnormal_exit() {
  abnormal_exit_record_usage_info
  exit 1
}

trap 'abnormal_exit' SIGINT SIGQUIT SIGTERM
handle_arguments "$@"
collect_info
record_usage_info