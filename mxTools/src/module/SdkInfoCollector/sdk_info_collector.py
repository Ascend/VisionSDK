#!/usr/bin/env python
# coding=utf-8

"""
-------------------------------------------------------------------------
 This file is part of the Vision SDK project.
Copyright (c) 2025 Huawei Technologies Co.,Ltd.

Vision SDK is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:

          http://license.coscl.org.cn/MulanPSL2

THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.
-------------------------------------------------------------------------
Description: Collect SDK logs, Ascend chip logs, and resource usage for fault locating.
Author: MindX SDK
Create: 2020
History: NA
"""

# 1. Import related packages
import argparse
import sys
import os
import time
import re
import datetime
import subprocess
import getpass
import pwd
from mindx.sdk.base import log

G_DEVICE_ID_NONE = '-2'
G_DEVICE_ID_ALL = '-1'
G_RED_FONT = 31
G_GREEN_FONT = 32
G_YELLOW_FONT = 33
FILE_MAX_NUM = 1500
ONE_YEAR_DAYS = 365
ONE_DAY = 1


# 2. Information collection class
class SdkInfoCollector:
    # 2.1 Define private attributes
    __log_type_name = ''  # log type name
    __log_type_index = ''  # log type index

    # 2.2 Initial member
    def __init__(self):
        self.__log_type_name = ['debug', 'info', 'warn', 'error', 'fatal', 'all_type']
        self.__log_type_index = {'d': 0, 'i': 1, 'w': 2, 'e': 3, 'f': 4, 'full': 5}

    # 2.3 Define methods
    # 2.3.1 Method of collecting chip log information
    @staticmethod
    def copy_file_path(file_path, dir_path, file_name):
        if os.path.exists(file_path) and not os.path.islink(file_path):
            if not os.path.exists(os.path.join(dir_path, file_name)):
                subprocess.run(['cp', '-rfp', file_path, dir_path])

    @staticmethod
    def count_days(str1, str2):
        num = datetime.datetime.strptime(str1, '%Y%m%d') - datetime.datetime.strptime(str2, '%Y%m%d')
        return num.days

    @staticmethod
    def check_file_size(file_path):
        file_max_size = 20 * 1024 * 1024
        if os.path.getsize(file_path) > file_max_size:
            log.error("The file in logs_path is too large.")
            return False
        return True

    @staticmethod
    def check_all_logs_num(sdk_log_root_path=None):
        file_num = 0
        for _, _, files in os.walk(sdk_log_root_path):
            file_num += len(files)
            if file_num > FILE_MAX_NUM:
                return False
        return True

    def get_log_range_files(self, sdk_log_root_path, log_range):
        file_list = []
        if log_range == 'full':
            for file in os.listdir(sdk_log_root_path):
                file_list.append(os.path.join(sdk_log_root_path, file))
            return file_list
        for file in os.listdir(sdk_log_root_path):
            if file.find('-') != -1:
                tem_list = []
                prefix_name = file.split('-')[0]
                log_time = prefix_name[prefix_name.rfind('.') + 1:]
                if not str(log_time).isnumeric():
                    continue
                tem_list.append(file)
                tem_list.append(int(log_time))
                file_list.append(tem_list)

        # Sort by the second field
        def take_second_elem(elem):
            return elem[1]
        log_files = []
        if not file_list:
            return log_files
        file_list.sort(key=take_second_elem, reverse=True)
        last_time = file_list[0][1]
        for file in file_list:
            if self.count_days(str(last_time), str(file[1])) <= int(log_range[:log_range.find('d')]):
                log_files.append(os.path.join(sdk_log_root_path, file[0]))
        log.info("Found {} satisfied log files about {} days ago."
                 .format(len(log_files), log_range[:log_range.find('d')]))
        return log_files

    # 2.3.2 Method of collecting config information
    def copy_dir_list(self, mx_sdk_home=None, output_folder_path=None, dir_name_list=None):
        log.info("Begin to collect config information.")
        if mx_sdk_home == '../':
            mx_sdk_home = os.path.abspath(os.path.join(os.getcwd(), '..'))
        if dir_name_list is None:
            dir_name_list = ['config', 'pipeline']
        dir_path_list = []
        for root, dirs, _ in os.walk(mx_sdk_home):
            for dir_low in dirs:
                output_dir_path = os.path.join(root, dir_low)
                if output_dir_path.find('opensource') == -1 and \
                        output_dir_path.find('bin') == -1 and dir_low in dir_name_list:
                    dir_path_list.append(os.path.join(root, dir_low))
        if not os.path.exists(output_folder_path):
            os.makedirs(output_folder_path)
        for dir_low in dir_path_list:
            current_dir = mx_sdk_home[mx_sdk_home.rfind('/') + 1:]
            output_dir_path = os.path.join(output_folder_path, dir_low[dir_low.find(current_dir):dir_low.rfind('/')])
            if not os.path.exists(output_dir_path):
                os.makedirs(output_dir_path)
            subprocess.run(['cp', '-rfp', dir_low, output_dir_path])
        log.info("End to collect config information.")

    def collect_log_info(self, roots_path, files_name, device_ids, output_dir_path):
        for device_id in device_ids:
            if roots_path.find(device_id) != -1 or roots_path.find('host') != -1:
                dir_path = os.path.join(output_dir_path, roots_path[roots_path.rfind('/') + 1:])
                if not os.path.exists(dir_path):
                    os.makedirs(dir_path)
                file_path = os.path.join(roots_path, files_name)
                self.copy_file_path(file_path, dir_path, files_name)

    def get_ascend_log_info(self, device_log_root_path, output_folder_path=None, device_id=None):
        if device_id == G_DEVICE_ID_NONE:
            log.warning("Default not to collect log of devices.")
            return
        if not os.path.exists(device_log_root_path):
            log.error("Ascend log path {} does not exist, please check it and try again!"
                      .format(os.path.relpath(device_log_root_path)))
            return
        output_dir_path = os.path.join(output_folder_path, 'npu-slog')
        if not os.path.exists(output_dir_path):
            os.makedirs(output_dir_path)
        log.info("Begin to collect npu log information.")
        if device_id != G_DEVICE_ID_ALL:
            device_ids = device_id.split(',')
            for root, _, files in os.walk(device_log_root_path):
                for file in files:
                    self.collect_log_info(root, file, device_ids, output_dir_path)
        else:
            for root, _, files in os.walk(device_log_root_path):
                for file in files:
                    dir_path = os.path.join(output_dir_path, root[root.rfind('/') + 1:])
                    os.makedirs(dir_path, exist_ok=True)
                    file_path = os.path.join(root, file)
                    self.copy_file_path(file_path, dir_path, file)
        log.info("End to collect npu log information.")

    # 2.3.3 Method of collecting mindx_sdk_info log information
    # 2.3.3.1 Collect log information based on the specified log days. The default number is the latest day.
    def get_path_logs(self, output_folder_path, path_roots, path_files):
        dir_path = os.path.join(output_folder_path, path_roots[path_roots.rfind('/') + 1:])
        if not os.path.exists(dir_path):
            os.makedirs(dir_path)
        file_path = os.path.join(path_roots, path_files)
        if not os.path.islink(file_path) and os.path.exists(file_path) and self.check_file_size(file_path):
            self.copy_file_path(file_path, dir_path, path_files)

    # 2.3.3.2 Collect all log information.
    def get_all_logs(self, sdk_log_root_path=None, output_folder_path=None):
        for root, _, files in os.walk(sdk_log_root_path):
            for file in files:
                self.get_path_logs(output_folder_path, root, file)

    # 2.3.3.3 Collect information based on the specified log type. The default log level is error.
    def get_log_type_files(self, sdk_log_root_path, log_type):
        log_files = []
        if log_type == 'full':
            for file in os.listdir(sdk_log_root_path):
                log_files.append(os.path.join(sdk_log_root_path, file))
            return log_files
        for file in os.listdir(sdk_log_root_path):
            start_index = self.__log_type_index.get(log_type)
            for i in range(start_index, len(self.__log_type_name)):
                # match sdk-info format
                if (file.lower()).find('.' + self.__log_type_name[i] + '.') != -1:
                    log_files.append(os.path.join(sdk_log_root_path, file))
                    break
        log.info('Found {} satisfied log.{}.above files.'
                .format(len(log_files), self.__log_type_name[self.__log_type_index.get(log_type)]))
        return log_files

    # 2.3.3.4 Collect information based on the specified log time and type.
    def get_specified_log_info(self, sdk_log_root_path=None, output_folder_path=None, log_range=None, log_type=None):
        if sdk_log_root_path.endswith('/'):
            sdk_log_root_path = sdk_log_root_path[:-1]
        if not os.path.exists(sdk_log_root_path):
            log.error("Specified log path {} does not exist, please check it and try again!"
                      .format(os.path.relpath(sdk_log_root_path)))
            return
        log.info("Begin to check the number of log files.")
        if not self.check_all_logs_num(sdk_log_root_path):
            log.error("Specified log path file number is beyond max file number: {} !".format(FILE_MAX_NUM))
            return
        log.info("End to check the number of log files.")
        log.info("Begin to collect all log information.")
        self.get_all_logs(sdk_log_root_path, output_folder_path)
        log.info("End to collect all log information.")
        if log_range == 'full' and log_type == 'full':
            return
        log.info("Begin to collect specified log range and type information.")
        output_folder_path = os.path.join(output_folder_path, 'logs.' + log_range + '.' +
                                          self.__log_type_name[self.__log_type_index.get(log_type)] + '.above')
        if not os.path.exists(output_folder_path):
            os.makedirs(output_folder_path)
        # Obtain log files generated before log_range
        log_range_file_list = self.get_log_range_files(sdk_log_root_path, log_range)
        # Obtain log files of log_type and above
        log_type_file_list = self.get_log_type_files(sdk_log_root_path, log_type)
        file_intersections = list(set(log_range_file_list).intersection(log_type_file_list))
        log.info("Found {} satisfied log.{}.abvoe files about {} days ago.".
                  format(len(file_intersections), self.__log_type_name[self.__log_type_index.get(log_type)], log_range))
        for file in file_intersections:
            if os.path.isfile(file):
                res = subprocess.run(['cp', '-rfp', file, output_folder_path])
                if res.returncode != 0:
                    log.error("copy {} to {} failed, please check it and try again!"
                              .format(os.path.relpath(file), os.path.relpath(output_folder_path)))
        log.info("End to collect specified log range and type information")


#3. Collects information based on the specified log time and type, collects config information, and
# collects chip logs (optional).
def collect_info(**kwargs):
    mx_sdk_home = kwargs.get('mx_sdk_home')  # home path
    output_folder_path = kwargs.get('output_folder_path')  # save dir
    log_range = kwargs.get('log_range')  # log time
    log_type = kwargs.get('log_type')  # log type
    device_id = kwargs.get('device_id')  # number of device log
    sdk_log_root_path = kwargs.get('logs_path')  # log dir
    ascend_path = 'ascend'
    log_path = 'log'
    user_home = kwargs.get('user_home')
    device_log_root_path = os.path.join(user_home, ascend_path, log_path)
    sdk_info_collector = SdkInfoCollector()
    sdk_info_collector.get_ascend_log_info(device_log_root_path=device_log_root_path,
                                        output_folder_path=output_folder_path,
                                        device_id=device_id)
    sdk_info_collector.copy_dir_list(mx_sdk_home=mx_sdk_home,
                                   output_folder_path=output_folder_path)
    sdk_info_collector.get_specified_log_info(sdk_log_root_path=sdk_log_root_path,
                                           output_folder_path=output_folder_path,
                                           log_range=log_range, log_type=log_type)


# 4. Check the validity of input parameters
# 4.1 Check whether the specified log time is valid
def check_range_param(log_range):
    if log_range.lower() == 'full':
        return True
    pattern = re.compile(r'(?P<days>^[0-9]{1,3})d$', re.I)  # find all digit and alpha d
    res = pattern.match(log_range)
    if res is None:
        return False
    count = int(res.groupdict()['days'])
    if count > ONE_YEAR_DAYS or count < ONE_DAY:
        return False
    return True


# 4.2 Check whether the specified log type is valid
def check_type_param(log_type):
    if log_type.lower() in {'d', 'i', 'w', 'e', 'f', 'full'}:
        return True
    return False


# 4.3 Check whether the specified device ID is valid
def check_device_param(device_id=G_DEVICE_ID_NONE):
    if device_id == G_DEVICE_ID_NONE or device_id == G_DEVICE_ID_ALL:
        return True
    return all([str(device_id).isnumeric() for device_id in device_id.split(",")])


# 4.4 Check the validity of input parameters and show relative message
def check_input_param(**dict_args):
    for _, value in dict_args.items():
        if value is None:
            log.error('Please execute \'bash sdk_info_collector.sh\' to collect sdk information.')
            return False
    if os.path.realpath(dict_args.get('logs_path')) != dict_args.get('logs_path'):
        log.error('logs_path(-p) only support absolute path,'
                  ' please execute \'bash sdk_info_collector.sh\' to collect sdk information.')
        return False

    if not os.path.isdir(dict_args.get('logs_path')):
        log.error('Please execute \'bash sdk_info_collector.sh\' to collect sdk information.')
        return False

    if not os.path.isdir(dict_args.get('mx_sdk_home')):
        log.error('Please execute \'bash sdk_info_collector.sh\' to collect sdk information.')
        return False

    if not os.path.isdir(dict_args.get('output_folder_path')):
        log.error('Please execute \'bash sdk_info_collector.sh\' to collect sdk information.')
        return False

    if not check_range_param(dict_args.get('log_range')):
        log.error("Parameter of range({}) is illegal, please cheak it and input again!"
                  .format(dict_args.get('log_range')))
        log.info (
            'You can set the range parameter as follows (default to get basic software, hardware, and '
            'configuration files): \n'
            'default : set to obtain all log information.\n'
            '-r xd : set to obtain log information within x days, x must be an integer.\n'
            '-r full : set to obtain all log information.')
        return False
    if not check_type_param(dict_args.get('log_type')):
        log.error("Parameter of type({}) is illegal, please cheak it and input again!"
                  .format(dict_args.get('log_type')))
        log.info (
            'You can set the type parameter as follows (default to get basic software, hardware, and '
            'configuration files): \n'
            'default : set to obtain all log information.\n'
            '-t d : set to obtain debug(-1) and above type log information.\n'
            '-t i : set to obtain info(0) and above type log information.\n'
            '-t w : set to obtain warn(1) and above type log information.\n'
            '-t e : set to obtain error(2) and above type log information.\n'
            '-t f : set to obtain fatal(3) and above type log information.\n'
            '-t full : set to obtain all log information.')
        return False
    if not check_device_param(dict_args.get('device_id')):
        log.error("Parameter of device id({}) is illegal, please cheak it and input again!"
                  .format(dict_args.get('device_id')))
        log.info(
            'You can set the device id parameter as follows (default to get host log files): \n'
            'default : not to obtain device log files.\n'
            '-x n : set to obtain the number of n device log files, n must be an integer.\n'
            '-x n1,n2,n3 : set to obtain the number of n1, n2, n3 device log files, n1, n2, n3 must be an integer.\n'
            '-x -1 : set to obtain all device log information.')
        return False
    return True


def check_valid_path(path, name, allow_link = False):
    if not path or not os.path.exists(path):
        log.error(f"{name} must exists!")
        return False
    if not allow_link and os.path.islink(os.path.abspath(path)):
        log.error(f"{name} cannot be a soft link!")
        return False
    if not os.access(path, mode=os.R_OK):
        log.error(f"Please check if {name} is readable.")
        return False
    current_user = getpass.getuser()
    file_owner = pwd.getpwuid(os.stat(path).st_uid).pw_name
    if current_user != file_owner:
        log.error(f"The owner of: {name} is not same with current user.")
        return False
    return True


def check_file_size(file_path, size):
    conf_file_size = os.path.getsize(file_path)
    if conf_file_size > 0 and conf_file_size / 1024 / 1024 < size:
        return True
    return False


def get_config_log_dir(sdk_home, user_home):
    log_config_file = f"{sdk_home}/config/logging.conf"
    if not check_valid_path(log_config_file, "logging.conf"):
        log.error("Check logging.conf file failed.")
        return ""
    if not check_file_size(log_config_file, 1):
        log.error("The size of logging.conf is too large, exceeds 1 MB.")
        return ""
    with open(log_config_file, 'r') as file:
        content = file.readlines()

    log_dir = ""
    for line in content:
        if line.startswith("log_dir"):
            log_dir = line.strip().split('=')[1]
    if log_dir:
        if log_dir[0] != os.path.sep:
            log_dir = f"{user_home}/log/mindxsdk/{log_dir}"
    return log_dir


# 5. Program entry for one-click information collection.
def main():
    # initializethe mindx sdk log
    ret = log.init()
    if ret:
        log.error('Log init failed: Please execute \'bash sdk_info_collector.sh\' to collect sdk information.')
        return 1
    # Check sdk and output directory
    sdk_home = os.getenv('MX_SDK_HOME')
    if not check_valid_path(sdk_home, 'MX_SDK_HOME', True):
        log.error('Please execute \'bash sdk_info_collector.sh\' to collect sdk information.')
        return 1
    output_path = os.getcwd()
    if not output_path or not os.path.exists(output_path):
        log.error('Please execute \'bash sdk_info_collector.sh\' to collect sdk information.')
        return 1
    output_path += '/mindx_sdk_info'
    if not check_valid_path(output_path, 'output_folder_path'):
        log.error('Please execute \'bash sdk_info_collector.sh\' to collect sdk information.')
        return 1
    # Parsing configuration parameters
    parser = argparse.ArgumentParser(description='One-click information collection')

    parser.add_argument('-r', '--range', type=str, default='full',
                        help='log that shows the certain time range (default: every day)')
    parser.add_argument('-t', '--type', type=str, default='full',
                        help='log that shows the certain type (default: every type)')
    parser.add_argument('-x', '--device', type=str, default=G_DEVICE_ID_NONE,
                        help='log of device (default: all device)')
    parser.add_argument('-p', '--logs_path', type=str, default=None,
                        help='path to save sdk logs')

    args = parser.parse_args()
    dict_info_args = {'mx_sdk_home': sdk_home, 'output_folder_path': output_path,
                 'logs_path': args.logs_path, 'log_range': args.range, 'log_type': args.type,
                 'device_id': args.device}

    if not check_input_param(**dict_info_args):
        log.error("Check input params failed.")
        return 1
    user_home = os.getenv("HOME")
    if not check_valid_path(user_home, "HOME"):
        log.error("The path of ${HOME} is invalid.")
        return 1
    dict_info_args.update({"user_home": user_home})
    config_log_dir = get_config_log_dir(sdk_home, user_home)
    if config_log_dir.strip(os.path.sep) != dict_info_args.get("logs_path").strip(os.path.sep):
        log.error("The logs_path is different with the log path in logging.conf.")
        return 1
    log.info("Begin to collect log and config information.")
    collect_info(**dict_info_args)
    log.info("End to collect log and config information.")
    return 0


if __name__ == '__main__':
    sys.exit(main())
