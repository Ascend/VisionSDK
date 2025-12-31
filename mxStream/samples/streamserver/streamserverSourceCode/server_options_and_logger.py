#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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
Description: Instance the logger and server options.
Author: Vision SDK
Create: 2024
History: NA
"""
import configparser
import logging
import os
from typing import Optional, Union
from logging.handlers import RotatingFileHandler

from utils import file_base_check
from utils import read_json_config
from utils import get_sys_info
from utils import valid_characters


def _is_digit(value: str) -> bool:
    return value.isdigit() or (value.startswith("-") and value[1:].isdigit())


def _handle_empty_value(key: str, default_value):
    msg = "Config parameter %s is empty in config file!" % key
    if default_value:
        logging.warning(msg)
        logging.warning("The parameter %s is automatically set as default value: %s." % (key, default_value))
        return default_value
    logging.error(msg)
    raise Exception(msg)


def _apply_numeric_bounds(key: str, value: str, min_value, max_value) -> int:
    raw_value = int(value)
    result = int(value)
    if min_value:
        result = max(min_value, result)
    if max_value:
        result = min(max_value, result)
    if raw_value != result:
        logging.warning("config parameter %s is boundary value %s" % (key, result))
    return result


def _raise_must_be_digit(key: str):
    msg = "config parameter %s must be digit" % key
    logging.error(msg)
    raise Exception(msg)


class SecureConfig:
    def __init__(self):
        self.cfg = configparser.ConfigParser()

    def load_config(self, config_file_path):
        file_base_check(config_file_path)
        with open(config_file_path, 'r') as file:
            file_contents = file.read()
            self.cfg.read_string("[DEFAULT]\n" + file_contents)

    def clear_config(self):
        self.cfg.clear()

    def get_value(self, key: str, min_value=None, max_value=None, default_value=None) -> Union[int, str, None]:
        # key 存在性检查
        self._assert_key_exists(key)

        # 获取 value 并处理为空情况
        value = self.cfg.get("DEFAULT", key)
        if value == "":
            return _handle_empty_value(key, default_value)

        # 是否为整数（含负号）
        is_digit = _is_digit(value)

        # 如果设定了 min/max，就必须是数字
        if (min_value or max_value) and not is_digit:
            _raise_must_be_digit(key)

        if not is_digit:
            return value

        return _apply_numeric_bounds(key, value, min_value, max_value)

    def _assert_key_exists(self, key: str):
        if not self.has_key(key):
            msg = "Can't find config parameter %s in config file!" % key
            logging.error(msg)
            raise Exception(msg)

    def has_key(self, key: str) -> bool:
        if "DEFAULT" in self.cfg and key in self.cfg["DEFAULT"]:
            return True
        else:
            logging.warning("Can't find config parameter %s in config file!", key)
            return False


class ServerOptions:

    CONFIG_SERVER_PATH = "config/streamserver.conf"
    REQUEST_CACHE_LOWER_LIMIT = 1
    REQUEST_CACHE_UPPER_LIMIT = 1000
    REQUEST_RATE_LOWER_LIMIT = 1
    REQUEST_RATE_UPPER_LIMIT = 30
    CONTENT_LENGTH_LOWER_LIMIT = 1
    CONTENT_LENGTH_UPPER_LIMIT = 51200
    MAX_LOG_SIZE = 20
    MIN_LOG_SIZE = 1
    MAX_ROTATE_NUM = 500
    MIN_ROTATE_NUM = 1
    PORT_MIN = 1025
    PORT_MAX = 65535
    MAX_DIR_DEPTH = 10
    MAX_FUNC_TIMES = 100
    AES128_GCM = 8
    AES256_GCM = 9

    def __init__(self):
        self._server_name = ""
        self._config_repository = ""
        self._request_cache_size = ""
        self._max_request_rate = ""
        self._max_content_length = ""
        self._ip = "127.0.0.1"
        self._port = ""
        self._protocol = "HTTP"
        self._cipher_list = ()
        self._server_config = SecureConfig()
        self._config_file_path = ""
        self._server_crt_file_path = ""
        self._server_key_file_path = ""
        self._ca_crt_file_path = ""
        self._ca_crl_file_path = ""
        self._server_key_mm = ""
        self._sdp_algorithm_id = ""
        self._infer_config_json_files = []
        self._infer_configs = {}
        self._log_dir = "logs"
        self._max_log_size = 10
        self._rotate_file_number = 50

    @property
    def request_cache_size(self):
        return self._request_cache_size

    @property
    def max_request_rate(self):
        return self._max_request_rate

    @property
    def max_content_length(self):
        return self._max_content_length

    @property
    def ip_addr_port(self):
        return self._protocol + "://" + self._ip + ":" + str(self._port)

    @property
    def server_crt_file_path(self):
        return self._server_crt_file_path

    @property
    def server_key_file_path(self):
        return self._server_key_file_path

    @property
    def ca_crt_file_path(self):
        return self._ca_crt_file_path

    @property
    def ca_crl_file_path(self):
        return self._ca_crl_file_path

    @property
    def cipher_list(self):
        return self._cipher_list

    @property
    def config_file_path(self):
        return self._config_file_path

    @property
    def infer_configs(self):
        return self._infer_configs

    @property
    def server_info_json(self):
        return {"server_name": self._server_name}

    @property
    def sdp_algorithm_id(self):
        return self._sdp_algorithm_id

    @property
    def server_key_mm(self):
        return self._server_key_mm

    @property
    def port(self):
        return self._port

    @property
    def log_dir(self):
        return self._log_dir

    @property
    def max_log_size(self):
        return self._max_log_size

    @property
    def rotate_file_number(self):
        return self._rotate_file_number

    @staticmethod
    def check_file_path(config_file_path):
        file_base_check(config_file_path)
        mode = oct(os.stat(config_file_path).st_mode)[-3:]
        if mode != "400":
            logging.error("Invalid file permission")
            raise Exception("Invalid file permission")

    @staticmethod
    def validate_infer_type(json_dict):
        if "inferType" not in json_dict or json_dict["inferType"] == "":
            msg = "InferType is empty in json config file, please check!"
            logging.error(msg)
            raise Exception(msg)
        if json_dict["inferType"] != "streams" and json_dict["inferType"] != "models":
            msg = "InferType must be streams or models in json config file, please check!"
            logging.error(msg)
            raise Exception(msg)

    @staticmethod
    def validate_name(json_dict):
        if "name" not in json_dict or json_dict["name"] == "":
            msg = "Name is empty in json config file, please check!"
            logging.error(msg)
            raise Exception(msg)
        if not valid_characters("^[0-9a-zA-Z\+\-\_]+$", json_dict["name"]):
            msg = "Name must be validated char ({0-9, a-z, A-Z, +, -, _}) in json config file, please check!"
            logging.error(msg)
            raise Exception(msg)

    @staticmethod
    def validate_path(json_dict):
        if "path" not in json_dict or json_dict["path"] == "":
            msg = "Path is empty in json config file, please check!"
            logging.error(msg)
            raise Exception(msg)
        file_base_check(os.path.realpath(json_dict["path"]), 4096)

    @staticmethod
    def validate_device_id(json_dict):
        if "deviceId" not in json_dict or json_dict["deviceId"] == "":
            msg = "DeviceId is empty in json config file, please check!"
            logging.error(msg)
            raise Exception(msg)
        if json_dict["deviceId"] < 0 or json_dict["deviceId"] > 1024:
            msg = "DeviceId must be in [0, 1024] in json config file, please check!"
            logging.error(msg)
            raise Exception(msg)

    @staticmethod
    def validate_timeout(json_dict):
        if "timeoutMs" in json_dict and (json_dict["timeoutMs"] < 1 or json_dict["timeoutMs"] > 100000):
            msg = "TimeoutMs must be in [1, 100000] in json config file, please check!"
            logging.error(msg)
            raise Exception(msg)

    @staticmethod
    def validate_inputs(json_dict):
        if "inputs" not in json_dict or not json_dict["inputs"]:
            msg = "Parse inputs tensor from infer config json file failed! Inputs are empty."
            logging.error(msg)
            raise Exception(msg)

        for i in json_dict["inputs"]:
            if not i:
                msg = "Parse inputs tensor from infer config json file failed! There is empty tensor in inputs"
                logging.error(msg)
                raise Exception(msg)

    @staticmethod
    def validate_outputs(json_dict):
        if "outputs" not in json_dict or not json_dict["outputs"]:
            msg = "Parse outputs tensor from infer config json file failed! Outputs are empty."
            logging.error(msg)
            raise Exception(msg)

        for i in json_dict["outputs"]:
            if not i:
                msg = "Parse outputs tensor from infer config json file failed! here is empty tensor in outputs"
                logging.error(msg)
                raise Exception(msg)

    @staticmethod
    def check_json_config(json_dict):
        ServerOptions.validate_infer_type(json_dict)
        ServerOptions.validate_name(json_dict)
        ServerOptions.validate_path(json_dict)
        ServerOptions.validate_device_id(json_dict)
        ServerOptions.validate_timeout(json_dict)
        ServerOptions.validate_inputs(json_dict)
        ServerOptions.validate_outputs(json_dict)

    def init(self):
        # parse conf file
        env_value = os.environ.get('MX_SDK_HOME')
        if not env_value:
            msg = "Environment variable MX_SDK_HOME is not set!"
            logging.error(msg)
            raise Exception(msg)
        server_cfg_path = os.path.realpath(os.path.join(env_value.split("=")[-1].strip("\""), self.CONFIG_SERVER_PATH))
        file_base_check(server_cfg_path)
        self._server_config.load_config(server_cfg_path)
        self._config_file_path = server_cfg_path
        self._set_server_config_params()
        # parse infer file
        if not os.path.isdir(self._config_repository):
            msg = f"{self._config_repository} is not a directory"
            logging.error(msg)
            raise Exception(msg)
        self._infer_config_json_files = []
        self._get_dir_file(self._config_repository, ".json", 0, 0)
        if not self._infer_config_json_files:
            msg = "There is no infer config json file!"
            logging.error(msg)
            raise Exception(msg)
        for file in self._infer_config_json_files:
            data = read_json_config(file)
            key = list(data.keys())[0]
            official_data = data[key]
            self.check_json_config(official_data)
            official_data["id"] = official_data["inferType"] + official_data["name"]
            if official_data["id"] in self._infer_configs:
                msg = "Same inferType and name in two json config file, please check!"
                logging.error(msg)
                raise Exception(msg)
            self._infer_configs[official_data["id"]] = official_data

    def _check_server_config_params(self):
        if not self._server_config.has_key("log_dir"):
            logging.warning("The log_dir is automatically set as default relative path: logs.")
        else:
            self._log_dir = self._server_config.get_value("log_dir")

        if not self._server_config.has_key("max_log_size"):
            logging.warning("The max_log_size is automatically set as default value: 10.")
        else:
            self._max_log_size = self._server_config.get_value("max_log_size")

        if not self._server_config.has_key("rotate_file_number"):
            logging.warning("The rotate_file_number is automatically set as default value: 50.")
        else:
            self._rotate_file_number = self._server_config.get_value("rotate_file_number")


    def _set_server_config_params(self):
        self._server_name = self._server_config.get_value("server_name", default_value="StreamServer")
        self._config_repository = os.path.realpath(
            self._server_config.get_value("infer_config_repo", default_value="./inferConfigRepository"))
        self._request_cache_size = self._server_config.get_value(
            "request_cache_size", min_value=self.REQUEST_CACHE_LOWER_LIMIT,
            max_value=self.REQUEST_CACHE_UPPER_LIMIT, default_value=120)
        self._max_request_rate = self._server_config.get_value(
            "max_request_rate", min_value=self.REQUEST_RATE_LOWER_LIMIT,
            max_value=self.REQUEST_RATE_UPPER_LIMIT, default_value=20)
        self._max_content_length = self._server_config.get_value(
            "max_content_length", min_value=self.CONTENT_LENGTH_LOWER_LIMIT,
            max_value=self.CONTENT_LENGTH_UPPER_LIMIT, default_value=20480)
        self._port = self._server_config.get_value(
            "port", min_value=self.PORT_MIN, max_value=self.PORT_MAX, default_value=8080)

        if self._protocol != "http" and self._protocol != "HTTP":
            msg = "Unsupported protocol"
            logging.error(msg)
            raise Exception(msg)

        self._check_server_config_params()

        self._server_config.clear_config()

    def _get_dir_file(self, folder_path: str, exten_str: str, times: int, depth: int):
        if depth >= self.MAX_DIR_DEPTH or times >= self.MAX_FUNC_TIMES:
            return
        for file_name in os.listdir(folder_path):
            file_abs_path = os.path.join(folder_path, file_name)
            if file_name.endswith(exten_str):
                times += 1
                self._infer_config_json_files.append(file_abs_path)
            elif os.path.isdir(file_abs_path):
                self._get_dir_file(file_abs_path, exten_str, times, depth + 1)


class RotatingFileHandler(logging.handlers.RotatingFileHandler):
    def doRollover(self):
        try:
            os.chmod(self.baseFilename, mode=0o400)
            logging.handlers.RotatingFileHandler.doRollover(self)
        except PermissionError:
            os.chmod('{}.{}'.format(self.baseFilename, server_option_instance.rotate_file_number),
                     mode=0o600)
            logging.handlers.RotatingFileHandler.doRollover(self)
        finally:
            os.chmod(self.baseFilename, mode=0o600)


def get_logger(filepath, log_formatter, max_bytes, backup_count, level=logging.INFO):
    _logger = logging.getLogger("streamserver")
    console = logging.StreamHandler()
    console.setFormatter(log_formatter)
    _logger.addHandler(console)
    handler = RotatingFileHandler(filename=filepath, maxBytes=max_bytes, backupCount=backup_count, encoding='utf-8')
    handler.setFormatter(log_formatter)
    _logger.addHandler(handler)
    _logger.setLevel(level)
    return _logger


def _init():
    server_options = ServerOptions()
    try:
        server_options.init()
    except Exception as err_message:
        logging.error('init server_options failed. stream_server startup failed.')
        raise ValueError from err_message
    home_directory = os.path.expanduser('~')
    conf_log_dir = os.path.join(home_directory, 'log', 'mindxsdk', server_options.log_dir)
    if not os.path.isdir(conf_log_dir):
        os.umask(0o027)
        os.makedirs(conf_log_dir, mode=0o750)
    log_file_path = f'{conf_log_dir}/stream_server.log'

    _log_formatter = logging.Formatter('%(levelname)s %(asctime)s [%(filename)s:%(lineno)d] %(message)s',
                                       '%Y-%m-%d %H:%M:%S')

    try:
        internal_logger = get_logger(log_file_path, _log_formatter, server_options.max_log_size * 1024 * 1024,
                            server_options.rotate_file_number)
    except Exception as err_message:
        logging.error('init log failed. stream_server startup failed.')
        raise ValueError from err_message
    return internal_logger, server_options

logger, server_option_instance = _init()

