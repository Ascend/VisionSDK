#!/bin/bash

# -------------------------------------------------------------------------
#  This file is part of the Vision SDK project.
# Copyright (c) 2026 Huawei Technologies Co.,Ltd.
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
# Description: Build Python wheel package for Vision SDK.
# Author: Vision SDK
# Create: 2026
# History: NA

set -e

usage() {
    echo "Usage: $0 <arm-gcc4|x86-gcc4> [output_dir] [version] [python_executable] [distribution_name] [python|runtime]"
    echo "Example: $0 arm-gcc4 output/Software/mxVision/wheel 7.3.0 python3 visionsdk runtime"
}

frame_project="${1:-}"
output_dir="${2:-}"
version_num="${3:-}"
python_bin="${4:-python3}"
dist_name="${5:-}"
package_mode="${6:-python}"

if [ -z "${frame_project}" ]; then
    usage
    exit 1
fi

if [ "${frame_project}" != "arm-gcc4" ] && [ "${frame_project}" != "x86-gcc4" ]; then
    echo "Error: Invalid package compiler type."
    usage
    exit 1
fi

if [ "${package_mode}" != "python" ] && [ "${package_mode}" != "runtime" ]; then
    echo "Error: Invalid package mode. Use 'python' or 'runtime'."
    usage
    exit 1
fi

work_space="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)/../../"
work_space="$(readlink -f "${work_space}")"

if [ -z "${output_dir}" ]; then
    output_dir="${work_space}/output/Software/mxVision/wheel"
elif [[ "${output_dir}" != /* ]]; then
    output_dir="${work_space}/${output_dir}"
fi

if ! command -v "${python_bin}" > /dev/null 2>&1; then
    if command -v python3 > /dev/null 2>&1; then
        python_bin="python3"
    else
        echo "Error: Cannot find python executable."
        exit 1
    fi
fi

"${python_bin}" -c "import wheel" > /dev/null 2>&1 || {
    echo "Error: Python package 'wheel' is required. Please run: ${python_bin} -m pip install wheel"
    exit 1
}

assembled_dir="${work_space}/mxVision/${frame_project}"
mxbase_output_dir="${work_space}/mxBase/output/${frame_project}/mxBase"
mxstream_python_dir="${work_space}/mxStream/output/${frame_project}/mxStream/python"
staging_dir="${work_space}/mxVision/mindx-${frame_project}/python_wheel"

if [ -d "${assembled_dir}/mindx" ]; then
    mindx_src="${assembled_dir}/mindx"
elif [ -d "${mxbase_output_dir}/mindx" ]; then
    mindx_src="${mxbase_output_dir}/mindx"
elif [ -d "${work_space}/mxBase/mindx" ]; then
    mindx_src="${work_space}/mxBase/mindx"
else
    echo "Error: Cannot find mindx Python package. Please build mxBase first."
    exit 1
fi

if [ "${package_mode}" = "runtime" ] && [ -d "${mxstream_python_dir}" ]; then
    python_src="${mxstream_python_dir}"
elif [ -d "${assembled_dir}/python" ]; then
    python_src="${assembled_dir}/python"
elif [ -d "${mxstream_python_dir}" ]; then
    python_src="${mxstream_python_dir}"
else
    echo "Error: Cannot find mxStream Python output. Please build mxStream first."
    exit 1
fi

mkdir -p "${output_dir}"
rm -f "${output_dir}"/*.whl

rm -rf "${staging_dir}"
mkdir -p "${staging_dir}/mindx/sdk"
cp -rf "${mindx_src}" "${staging_dir}/"
cp -rf "${python_src}"/* "${staging_dir}/mindx/sdk/"
find "${staging_dir}/mindx/sdk" -maxdepth 1 -type f -name '*.whl' -delete

# Keep the existing run-package behavior: do not include this duplicate
# postprocess extension in the wheel when it is present in python output.
find "${staging_dir}/mindx/sdk" -maxdepth 1 -type f -name 'postprocess.cpython-*-linux-gnu.so' -delete

install_runtime_ascendc_ops() {
    local runtime_dir="$1"
    local op_pkg_dir="${runtime_dir}/operators/operatorascendc"
    local op_install_path="${runtime_dir}/operators/ascendc"

    if [ ! -d "${op_pkg_dir}" ]; then
        return
    fi

    local op_packages=("${op_pkg_dir}"/custom_opp_*.run)
    if [ ! -f "${op_packages[0]}" ]; then
        return
    fi

    mkdir -p "${op_install_path}"
    echo "Installing AscendC custom operators into wheel runtime."
    unset DISPLAY
    export LD_LIBRARY_PATH=/usr/lib64:${LD_LIBRARY_PATH:-}
    if ! bash "${op_packages[0]}" --install-path="${op_install_path}" --nox11 > /dev/null 2>&1; then
        echo "Error: Install AscendC custom operator package failed."
        exit 1
    fi

    if [ ! -d "${op_install_path}/vendors/customize" ]; then
        echo "Error: Deploy AscendC custom operators failed."
        exit 1
    fi

    rm -rf "${op_pkg_dir}"
    rm -rf "${runtime_dir}/operators/packages"
    rm -rf "${op_install_path}/vendors/customize/op_api/include"
    rm -rf "${op_install_path}/vendors/customize/op_impl/ai_core/tbe/customize_impl"
    rm -rf "${op_install_path}/vendors/customize/op_proto/inc"
    rm -rf "${op_install_path}/vendors/customize/bin"
    rm -f "${runtime_dir}/operators/help.info"
    rm -f "${runtime_dir}/operators/install.sh"
    rm -f "${runtime_dir}/operators/upgrade.sh"

    find "${op_install_path}" -type d -exec chmod 750 {} +
    find "${op_install_path}" -type f -name '*.py' -exec chmod 500 {} +
    find "${op_install_path}" -type f -name '*.json' -exec chmod 640 {} +
    find "${op_install_path}" -type f -name '*.so' -exec chmod 440 {} +
}

if [ "${package_mode}" = "runtime" ]; then
    if [ ! -d "${assembled_dir}/lib" ] || [ ! -d "${assembled_dir}/opensource" ] || [ ! -d "${assembled_dir}/operators" ]; then
        echo "Error: Full runtime wheel requires assembled mxVision runtime at ${assembled_dir}."
        echo "Please build it through mxBase/build/package.sh or build_all.sh first."
        exit 1
    fi

    mkdir -p "${staging_dir}/visionsdk"
    cat > "${staging_dir}/visionsdk/__init__.py" <<'PY'
from ._bootstrap import activate, get_preload_errors, get_runtime_home

activate()

__all__ = ["activate", "get_preload_errors", "get_runtime_home"]
PY

    cat > "${staging_dir}/visionsdk/_bootstrap.py" <<'PY'
import ctypes
import os
import sys
from pathlib import Path

_ACTIVATED = False
_PRELOAD_ERRORS = []


def get_runtime_home():
    return Path(__file__).resolve().parent / "runtime"


def _prepend_env(name, values):
    current = os.environ.get(name, "")
    parts = [str(value) for value in values if value and Path(value).exists()]
    if current:
        parts.extend([part for part in current.split(":") if part])

    seen = set()
    unique_parts = []
    for part in parts:
        if part not in seen:
            seen.add(part)
            unique_parts.append(part)
    os.environ[name] = ":".join(unique_parts)


def _fix_runtime_permissions(runtime_home):
    permission_modes = {
        ".py": 0o550,
        ".sh": 0o500,
        ".h": 0o550,
    }
    try:
        runtime_home.chmod(0o750)
    except OSError:
        pass

    for item in runtime_home.rglob("*"):
        if item.is_symlink():
            continue
        try:
            if item.is_dir():
                item.chmod(0o750)
            elif item.is_file():
                if "." not in item.name:
                    item.chmod(0o500)
                else:
                    item.chmod(permission_modes.get(item.suffix, 0o640))
        except OSError:
            pass


def _preload_libraries(runtime_home):
    global _PRELOAD_ERRORS
    if not hasattr(ctypes, "RTLD_GLOBAL"):
        return

    preload_files = [
        runtime_home / "operators" / "ascendc" / "vendors" / "customize" / "op_api" / "lib" / "libcust_opapi.so",
        runtime_home / "lib" / "libmxbase.so",
        runtime_home / "lib" / "libmxpidatatype.so",
        runtime_home / "lib" / "libplugintoolkit.so",
        runtime_home / "lib" / "libstreammanager.so",
    ]

    for so_file in preload_files:
        if not so_file.exists():
            continue
        try:
            ctypes.CDLL(str(so_file), mode=ctypes.RTLD_GLOBAL)
        except OSError as error:
            _PRELOAD_ERRORS.append(f"{so_file}: {error}")


def get_preload_errors():
    return list(_PRELOAD_ERRORS)


def activate(preload=True):
    global _ACTIVATED
    runtime_home = get_runtime_home()
    if not runtime_home.exists():
        return str(runtime_home)

    if os.environ.get("VISIONSDK_SKIP_RUNTIME_CHMOD", "0") != "1":
        _fix_runtime_permissions(runtime_home)

    os.environ["MX_SDK_HOME"] = str(runtime_home)
    os.environ["GST_PLUGIN_SCANNER"] = str(runtime_home / "opensource" / "libexec" / "gstreamer-1.0" / "gst-plugin-scanner")
    _prepend_env("GST_PLUGIN_PATH", [
        runtime_home / "opensource" / "lib" / "gstreamer-1.0",
        runtime_home / "lib" / "plugins",
    ])
    _prepend_env("LD_LIBRARY_PATH", [
        runtime_home / "operators" / "ascendc" / "vendors" / "customize" / "op_api" / "lib",
        runtime_home / "lib" / "modelpostprocessors",
        runtime_home / "lib",
        runtime_home / "opensource" / "lib",
        runtime_home / "opensource" / "lib64",
    ])
    _prepend_env("ASCEND_CUSTOM_OPP_PATH", [
        runtime_home / "operators" / "ascendc" / "vendors" / "customize",
    ])

    python_dir = runtime_home / "python"
    if python_dir.exists() and str(python_dir) not in sys.path:
        sys.path.insert(0, str(python_dir))

    package_dir = Path(__file__).resolve().parent.parent
    if str(package_dir) in sys.path:
        sys.path.remove(str(package_dir))
    sys.path.insert(0, str(package_dir))

    if preload and not _ACTIVATED:
        _preload_libraries(runtime_home)
    _ACTIVATED = True
    return str(runtime_home)
PY

    cp -rf "${assembled_dir}" "${staging_dir}/visionsdk/runtime"
    install_runtime_ascendc_ops "${staging_dir}/visionsdk/runtime"
    find "${staging_dir}/visionsdk/runtime/python" -maxdepth 1 -type f -name '*.whl' -delete
    cat >> "${staging_dir}/visionsdk/runtime/set_env.sh" <<'SH'

    if [ "${VISIONSDK_SKIP_RUNTIME_CHMOD:-0}" != "1" ] && [ -n "${MX_SDK_HOME:-}" ] && [ "${MX_SDK_HOME}" = "${sdk_path}" ]; then
      find "${MX_SDK_HOME}" -type d -exec chmod 750 {} + 2>/dev/null || true
      find "${MX_SDK_HOME}" -type f -exec chmod 640 {} + 2>/dev/null || true
      find "${MX_SDK_HOME}" -type f ! -name '*.*' -exec chmod 500 {} + 2>/dev/null || true
      find "${MX_SDK_HOME}" -type f -name '*.py' -exec chmod 550 {} + 2>/dev/null || true
      find "${MX_SDK_HOME}" -type f -name '*.sh' -exec chmod 500 {} + 2>/dev/null || true
      find "${MX_SDK_HOME}" -type f -name '*.h' -exec chmod 550 {} + 2>/dev/null || true
    fi
SH

    (
        cd "${staging_dir}/visionsdk/runtime"
        find * | sort > filelist.txt
        sed -i '/filelist.txt/d' filelist.txt
    )

    tmp_init="${staging_dir}/mindx/__init__.py.tmp"
    {
        echo "try:"
        echo "    from visionsdk import activate as _visionsdk_activate"
        echo "    _visionsdk_activate()"
        echo "except Exception:"
        echo "    pass"
        cat "${staging_dir}/mindx/__init__.py"
    } > "${tmp_init}"
    mv "${tmp_init}" "${staging_dir}/mindx/__init__.py"
fi

cat > "${staging_dir}/StreamManagerApi.py" <<'PY'
try:
    from visionsdk import activate as _visionsdk_activate
    _visionsdk_activate()
except Exception:
    pass
from mindx.sdk.StreamManagerApi import *  # noqa: F401,F403
PY

cat > "${staging_dir}/stream.py" <<'PY'
try:
    from visionsdk import activate as _visionsdk_activate
    _visionsdk_activate()
except Exception:
    pass
from mindx.sdk.stream import *  # noqa: F401,F403
PY

"${python_bin}" - "${staging_dir}" <<'PY'
import re
import sys
from pathlib import Path

staging_dir = Path(sys.argv[1])
module_names = ("_base", "_dvpp", "_log", "_post", "_stream", "_StreamManagerApi")

for py_file in (staging_dir / "mindx").rglob("*.py"):
    text = py_file.read_text(encoding="utf-8")
    for module_name in module_names:
        pattern = re.compile(rf"^import {re.escape(module_name)}\s*$", re.MULTILINE)
        replacement = (
            "try:\n"
            f"    from . import {module_name}\n"
            "except ImportError:\n"
            f"    import {module_name}\n"
        )
        text = pattern.sub(replacement, text)
    py_file.write_text(text, encoding="utf-8")
PY

rm -rf "${staging_dir}/build" "${staging_dir}/dist" "${staging_dir}"/*.egg-info

cd "${staging_dir}"
if [ -n "${version_num}" ]; then
    export VISIONSDK_WHEEL_VERSION="${version_num}"
fi
if [ -n "${dist_name}" ]; then
    export VISIONSDK_WHEEL_NAME="${dist_name}"
fi

bdist_args=(bdist_wheel)
if [ -n "${WHEEL_PLAT_NAME:-}" ]; then
    bdist_args+=(--plat-name "${WHEEL_PLAT_NAME}")
fi

"${python_bin}" "${work_space}/mxBase/build/setup.py" "${bdist_args[@]}"
cp -f "${staging_dir}"/dist/*.whl "${output_dir}/"

echo "Wheel package output directory: ${output_dir}"
