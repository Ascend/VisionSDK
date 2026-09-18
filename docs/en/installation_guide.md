# Installation Guide

## Installation Notes

Vision SDK supports deployment on a physical machine and container deployment. This document describes deployment on a physical machine.

If you need to deploy it in a container, you only need to complete the installation of system dependencies and then start the container by referring to the [Vision SDK image](https://www.hiascend.com/developer/ascendhub/detail/9e0edaf9488b447b951072c5c61ce8f1).

**Notes**

If you need to install third-party software in addition to the Vision SDK package, ensure that you update it to the latest version promptly and fix any existing vulnerabilities.

## Installing Dependencies

### Ubuntu

For the required dependency names, recommended versions, and acquisition suggestions in the Ubuntu environment, see [Table 1](#table-ubuntu-system).

**Table 1** Ubuntu dependency names and corresponding versions
<a id="table-ubuntu-system"></a>

|Dependency|Recommended Version|Acquisition Suggestion|
|--|--|--|
|GCC|7.3.0|It is recommended that you obtain the source package and build it for installation. For installation steps, see [Install GCC 7.3.0](appendix.md#installing-gcc-730).|
|make|Not lower than 3.82|It is recommended that you install it through the package manager. See the following installation command.<br>`sudo apt-get install -y make`<br>If the version in the package manager does not meet the minimum version requirement, you can install it from source.|
|CMake|Not lower than 3.5.2|It is recommended that you install it through the package manager. See the following installation command.<br>`sudo apt-get install -y cmake`<br>If the version in the package manager does not meet the minimum version requirement, you can install it from source.|
|XZ|Not lower than 5.2.2|It is recommended that you install it through the package manager. See the following installation command.<br>`sudo apt-get install -y xz-utils`<br>If the version in the package manager does not meet the minimum version requirement, you can install it from source.|
|protobuf|4.25.1|It is recommended that you install it with pip. See the following installation command.<br>`pip3 install protobuf==4.25.1`|
|Python|3.9/3.10/3.11/3.12|It is recommended that you obtain the source package and build it for installation. For installation steps, see [Installing Python Dependencies](appendix.md#installing-python-dependencies).|
|haveged (optional)|-|You need to install this dependency when you use the StreamServer feature. It is recommended that you install it through the package manager. See the following installation command.<br>`sudo apt-get install -y haveged`|

Use the following command to check whether GCC, Make, CMake, and other dependency software are installed.

```bash
gcc --version
make --version
cmake --version
xz --version
python3 --version
pip3 list|grep protobuf
```

If the following information is returned for each command, the corresponding software is installed. The following output is only an example. Use the actual output as the reference.

```bash
gcc (Ubuntu 7.3.0-3ubuntu1~18.04) 7.3.0
GNU Make 3.82
cmake version 3.5.2
xz (XZ Utils) 5.2.2
Python 3.9.2
protobuf           4.25.1
```

### CentOS

For the required dependency names, recommended versions, and acquisition suggestions in the CentOS environment, see [Table 1](#table-centos-system).

**Table 1** CentOS dependency names and corresponding versions
<a id="table-centos-system"></a>

|Dependency|Recommended Version|Acquisition Suggestion|
|--|--|--|
|GCC|7.3.0|It is recommended that you obtain the source package and build it for installation. For installation steps, see [Install GCC 7.3.0](appendix.md#installing-gcc-730).|
|make|Not lower than 3.82|It is recommended that you install it through the package manager. See the following installation command.<br>`sudo yum install -y make`<br>If the version in the package manager does not meet the minimum version requirement, you can install it from source.|
|CMake|Not lower than 3.5.2|It is recommended that you install it through the package manager. See the following installation command.<br>`sudo yum install -y cmake`<br>If the version in the package manager does not meet the minimum version requirement, you can install it from source.|
|XZ|Not lower than 5.2.2|It is recommended that you install it through the package manager. See the following installation command.<br>`sudo yum install -y xz`<br>If the version in the package manager does not meet the minimum version requirement, you can install it from source.|
|protobuf|4.25.1|It is recommended that you install it with pip. See the following installation command.<br>`pip3 install protobuf==4.25.1`|
|Python|3.9|It is recommended that you obtain the source package and build it for installation. For installation steps, see [Installing Python Dependencies](appendix.md#installing-python-dependencies).|
|haveged (optional)|-|You need to install this dependency when you use the StreamServer feature. It is recommended that you install it through the package manager. See the following installation command.<br>`sudo yum install -y haveged`|

Use the following command to check whether GCC, Make, CMake, and other dependency software are installed.

```bash
gcc --version
make --version
cmake --version
xz --version
python3 --version
pip3 list|grep protobuf
```

If the following information is returned for each command, the corresponding software is installed. The following output is only an example. Use the actual output as the reference.

```bash
gcc 7.3.0
GNU Make 3.82
cmake version 3.5.2
xz (XZ Utils) 5.2.2
Python 3.9.2
protobuf           4.25.1
```

### Installing the NPU Driver Firmware and CANN

Refer to [CANN Quick Installation](https://www.hiascend.com/cann/download) to install the Ascend NPU driver firmware and CANN software (including the Toolkit and ops packages), and configure the environment variables.

## Installation Methods

### Online Installation

**Installation Steps**

1. Install the specified version from the pip mirror source.

    ```bash
    python3 -m pip install --index-url <pip_index_url> visionsdk=={version}
    ```

2. After the installation is complete, run the following command to check the pip package information.

    ```bash
    python3 -m pip show visionsdk
    ```

### Offline Installation

#### pip Installation

Vision SDK supports installing the `visionsdk` wheel package through pip.

**Installation Instructions**

- The pip installation method requires Python 3 in the installation environment.
- The installation environment must have the NPU driver, firmware, and CANN installed, and the CANN runtime libraries must be findable by the system. If the CANN environment variables are not in effect, run the CANN environment variable configuration script first. The specific path depends on the actual installation path.

    ```bash
    source /usr/local/Ascend/ascend-toolkit/set_env.sh
    ```

- If the driver-side dynamic library path is missing in the environment, add the path to `LD_LIBRARY_PATH` based on the actual installation path. For example:

    ```bash
    export LD_LIBRARY_PATH=/usr/local/Ascend/driver/lib64:/usr/local/Ascend/driver/lib64/common:/usr/local/Ascend/driver/lib64/driver:$LD_LIBRARY_PATH
    ```

- The pip installation method installs the Vision SDK runtime to the `site-packages/visionsdk/runtime` directory of the current Python environment. After you import `visionsdk`, environment variables such as `MX_SDK_HOME`, `GST_PLUGIN_PATH`, `GST_PLUGIN_SCANNER`, `LD_LIBRARY_PATH`, and `ASCEND_CUSTOM_OPP_PATH` are automatically set in the current Python process.
- The wheel package used by the pip installation method contains the Vision SDK runtime dependencies and pre-deploys the AscendC custom operators to the `site-packages/visionsdk/runtime/operators/ascendc` directory when the wheel is built. After installation, `ASCEND_CUSTOM_OPP_PATH` points to the operator directory in the wheel runtime. Therefore, you do not need to write these AscendC operators to the CANN installation directory.
- If you run C++ samples, shell scripts, or other non-Python processes, activate the wheel runtime environment variables first. You can run `python3 -m pip show visionsdk` to confirm the installation path, and then run `source <site-packages>/visionsdk/runtime/set_env.sh`.
- The pip installation method does not execute the system-level installation, deployment log recording, CANN directory writing, or uninstallation script logic in the run package installation script. If you need a complete system-level deployment, or rely on the operator deployment process in which operators must be installed in the CANN directory, use the run package installation method.

**Installation Steps**

1. Install with pip.

    To install with a local wheel file:

    ```bash
    python3 -m pip install ./visionsdk-{version}-py3-none-linux_{arch}.whl
    ```

2. If an old version is installed in the environment, you are advised to reinstall it by using the following command.

    ```bash
    python3 -m pip install --force-reinstall visionsdk=={version}
    ```

3. After the installation is complete, run the following command to check the pip package information.

    ```bash
    python3 -m pip show visionsdk
    ```

**Installation Verification**

After the installation is complete, you can run the following command to verify that the Python interfaces can be imported properly.

```bash
python3 - <<'PY'
import os
import visionsdk

print("visionsdk =", visionsdk.__file__)
print("MX_SDK_HOME =", os.environ.get("MX_SDK_HOME"))
print("runtime =", visionsdk.get_runtime_home())
print("preload errors =", visionsdk.get_preload_errors()[:10])

from mindx.sdk import base
from StreamManagerApi import StreamManagerApi
print("import ok")
PY
```

If `import ok` is displayed and `preload errors` is empty, the Python runtime environment of the pip installation method passes the verification. In this case, `MX_SDK_HOME` should point to the `site-packages/visionsdk/runtime` directory of the current Python environment.

If you need to verify C++ samples or shell scripts, run the following command to activate the wheel runtime environment variables first:

```bash
SDK_RUNTIME=$(python3 - <<'PY'
import visionsdk
print(visionsdk.get_runtime_home())
PY
)
source "${SDK_RUNTIME}/set_env.sh"
```

After the execution, you can check whether the AscendC custom operator dynamic library exists:

```bash
ls -l "${MX_SDK_HOME}/operators/ascendc/vendors/customize/op_api/lib/libcust_opapi.so"
```

**FAQ**

- If the import fails because dynamic libraries such as `libascendcl.so` and `libascend_hal.so` cannot be found, check whether the NPU driver, firmware, and CANN are installed, and confirm that the CANN and driver dynamic library paths have been added to `LD_LIBRARY_PATH`.
- If the import fails because dynamic libraries such as `libmxbase.so` and `libstreammanager.so` cannot be found, confirm that the installed `visionsdk` wheel package matches the current CPU architecture, and run the installation verification command again.
- If a test case that involves AscendC custom operators such as `Abs` and `Absolute` reports `aclnnAbsCustomGetWorkspaceSize does not support` or prompts that `libcust_opapi.so` is abnormal, confirm that the wheel package containing the AscendC operator pre-deployment fix is installed, and confirm that `${MX_SDK_HOME}/operators/ascendc/vendors/customize/op_api/lib/libcust_opapi.so` exists. Before running C++ samples, run `source <site-packages>/visionsdk/runtime/set_env.sh` first.
- If `pip install visionsdk=={version}` reports that the version cannot be found, confirm that the wheel package of the corresponding version has been uploaded to the pip mirror source, and that the version number is consistent with the wheel package metadata.

#### run Package Installation

**Obtaining the Required Package**

|Component|Package|Link|
|--|--|--|
|Vision SDK|Vision SDK package|[Download link](https://www.hiascend.com/developer/download/community/result?module=sdk+cann)|

**Installation Instructions**

- The user who installs and runs Vision SDK:
    - Must be the same user for installation and runtime.
    - Is advised to be a regular user. Running the program as the `root` user may pose a security risk because permissions can be tampered with.
    - Must also install the toolkit.

- Logs related to package installation, upgrade, uninstallation, and version queries are saved to the `~/log/mindxsdk/deployment.log` file. Logs related to integrity verification, file extraction, and tar command access are saved to the `~/log/makeself/makeself.log` file. You can review the corresponding files for later log tracing and auditing.
- Installing Vision SDK copies the operators to the CANN installation path. Therefore, if you uninstall and reinstall CANN after you install Vision SDK, the operators will not be found. In that case, reinstall Vision SDK.

**Installation Steps**

1. Log in to the installation environment.
2. Upload the Vision SDK package to any path in the installation environment and go to the package path.
3. Grant execute permission on the package.

    ```bash
    chmod u+x Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run
    ```

4. Run the following command to verify the consistency and integrity of the package.

    ```bash
    ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --check
    ```

    If the system does not have the `shasum` or `sha256sum` tool, verification fails. In that case, install `shasum` or `sha256sum` yourself.

    If the following information is displayed, the package meets the consistency and integrity requirements.

    ```text
    Verifying archive integrity...  100%   SHA256 checksums are OK. All good.
    ```

5. Create the installation path for the Vision SDK package. Do not install Vision SDK in `/tmp`.
    - If you want to specify an installation path, create the installation path first. For example, if the installation path is `/home/work/Mind_SDK`:

        ```bash
        mkdir -p /home/work/Mind_SDK
        ```

    - If you do not specify an installation path, the software is installed in the directory that contains the Vision SDK package by default.

6. Go to the package upload path and install Vision SDK by using the following command. For constraints related to the installation path, see the description of `--install-path` in [Table 1](#table-run-install-args). When you install Vision SDK, a prompt asks whether you accept the download license agreement. If you need to skip this step during installation, add `echo y |` before the installation command to indicate that you agree to the [Huawei Software Download License](../../mxBase/build/agreement.conf).

    - If you specify an installation path. For example, if the installation path is `/home/work/Mind_SDK`:

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install --install-path=/home/work/Mind_SDK
        ```

        or

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install --install-path=/home/work/Mind_SDK
        ```

    - If you do not specify an installation path, the software is installed in the current directory.

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
        ```

        or

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
        ```

    >[!NOTE]
    >The `--install` command also supports optional parameters, as shown in [Table 1](#table-run-install-args).

7. When the prompt `Do you accept the LICENSE to install VisionSDK?[Y/N]` appears during installation, enter `Y` or `y` to accept the download agreement and continue. If you enter any other character, installation stops and the program exits.

8. After the installation is complete, if no error message appears, the software is successfully installed to the specified or default path.

    ```text
    Successfully installed mindx-xxx
    ```

9. Activate the environment variables.

    Go to Vision SDK installation path and run the following command to activate Vision SDK environment variables.

    ```bash
    source set_env.sh
    ```

>[!NOTE]
> Some interfaces are implemented by AscendC operators. After installation and deployment, AscendC operator-related files are generated in the installation path.

#### Source Installation

This section describes how to build and generate the Vision SDK run package and wheel package from source. Before performing the following operations, ensure that you have pulled the Vision SDK code repository and entered the project directory.

1. Downloading build dependencies

    ```bash
    cd VisionSDK
    wget https://mindcluster.obs.cn-north-4.myhuaweicloud.com/opensource-arm-gcc4.tar.gz
    wget https://mindcluster.obs.cn-north-4.myhuaweicloud.com/opensource-device-arm-gcc4.tar.gz
    wget https://mindcluster.obs.cn-north-4.myhuaweicloud.com/opensource-x86-gcc4.tar.gz
    wget https://mindcluster.obs.cn-north-4.myhuaweicloud.com/opensource-device-x86-gcc4.tar.gz
    cd opensource/opensource
    git clone -b release-2.5.0 https://gitcode.com/gh_mirrors/ma/makeself.git
    git clone -b v2.5.0.x https://gitcode.com/cann-src-third-party/makeself.git makeself_patch
    cd ../../
    ```

2. Running the build

    ```bash
    cd VisionSDK
    mkdir -p ../ci/config && echo "version: 1.0.0" > ../ci/config/config.ini
    # For the arm architecture
    bash build_all.sh arm-gcc4 aarch64 notest
    # For the x86 architecture
    bash build_all.sh x86-gcc4 x86_64 notest
    ```

3. Verifying the product build package

    After you run `build_all.sh`, the wheel package for [pip installation](#pip-installation) and the run package for [run package installation](#run-package-installation) are generated in the following directories:

    ```text
    output/
        `-- Software
            `-- mxVision
                |-- Ascend-mindxsdk-mxvision_1.0.0_linux-aarch64.run
                `-- wheel
                    `-- visionsdk-1.0.0-py3-none-linux_aarch64.whl
    ```

4. Testing the build

    ```bash
    # Install lcov 2.0 to calculate the test coverage and generate visual reports
    apt update
    apt install -y libcapture-tiny-perl libdatetime-perl libtimedate-perl
    wget https://github.com/linux-test-project/lcov/releases/download/v2.0/lcov-2.0.tar.gz
    tar -xzf lcov-2.0.tar.gz && cd lcov-2.0
    make install
    # Download the test dependencies
    cd VisionSDK/opensource/opensource
    git clone -b v2.7.x-h3 https://gitcode.com/cann-src-third-party/mockcpp.git mockcpp_patch
    git clone -b mindsdk https://gitcode.com/Ascend/mockcpp.git mockcpp
    git clone -b release-1.11.0 https://gitcode.com/GitHub_Trending/go/googletest.git googletest
    cd VisionSDK
    # For the arm architecture
    bash build_all.sh arm-gcc4 aarch64 test
    # For the x86 architecture
    bash build_all.sh x86-gcc4 x86_64 test
    ```

## Upgrade

Vision SDK supports pip installation and run package installation. The upgrade commands of the two installation methods are different. Choose the method based on the actual installation method.

**Upgrade by pip Installation**

1. Vision SDK installed through pip can be upgraded to the specified version by using the following command:

    ```bash
    python3 -m pip install --upgrade visionsdk=={version}
    ```

2. If you need to upgrade from a local wheel package, run:

    ```bash
    python3 -m pip install --upgrade ./visionsdk-{version}-py3-none-linux_{arch}.whl
    ```

3. If an old version is installed in the environment and you need to force a reinstall, run:

    ```bash
    python3 -m pip install --force-reinstall visionsdk=={version}
    ```

**Upgrade by run Package Installation**

1. Obtain and upload the package by referring to [run package installation](#run-package-installation).
2. Grant execute permission on the package.

    ```bash
    chmod u+x Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run
    ```

3. Use the package upgrade command to upgrade the current Vision SDK package. See the following example for the upgrade command. For parameter details, see [Table 1](#table-run-update-args). When you upgrade Vision SDK, a prompt asks whether you accept the download license agreement. If you need to skip this step during upgrade, add `echo y |` before the upgrade command to indicate that you agree to the [Huawei Software Download License](../../mxBase/build/agreement.conf).

    ```bash
    ./*.run --upgrade --install-path={MX_SDK_HOME}
    ```

    or

    ```bash
    ./*.run --upgrade --install-path={MX_SDK_HOME}
    ```

    `*.run` is the name of the Vision SDK package that you obtain. Replace it with the actual name.

4. When the prompt `Do you accept the LICENSE to install VisionSDK?[Y/N]` appears during upgrade, enter `Y` or `y` to accept the download agreement and continue. If you enter any other character, the upgrade stops and the program exits.

5. Run the following command to query the version upgrade record.

    ```bash
    cd ~/log/mindxsdk/
    cat deployment.log
    ```

    The following is an example of the upgrade output:

    ```text
    MindX SDK mxVision:  5.0.RC2  ->  MindX SDK mxVision:  7.3.0
    ```

## Uninstallation

Vision SDK supports the pip installation method and the run package installation method. The uninstallation operations of the two installation methods are different. Choose the method based on the actual installation method.

**Uninstallation by pip Installation**

1. Vision SDK installed through pip can be uninstalled by using the following command:

    ```bash
    python3 -m pip uninstall visionsdk
    ```

**Uninstallation by run Package Installation**

>[!NOTE]
>
>- The script uninstallation and package uninstallation described in the following content apply only to the run package installation method.
>- Before uninstallation, the run package installation method checks whether any Vision SDK services are still running. During uninstallation, user data and configuration are retained. Uninstallation is a high-risk operation. Ensure that no services are using the SDK before you run the uninstallation.
>- During uninstallation, the run package installation method also deletes Vision SDK-related operator files. The operator file installation directory is `${ASCEND_OPP_PATH}/vendors/customize_vision`, where `${ASCEND_OPP_PATH}` is the CANN environment variable directory set during [Installing the NPU Driver Firmware and CANN](#installing-the-npu-driver-firmware-and-cann). If multiple Vision SDK instances exist in the environment, uninstalling may cause the operators to be unavailable. In that case, reinstall Vision SDK.

1. Choose either uninstallation method:
    1. Go to the Vision SDK installation path and check whether the `uninstall.sh` script in the `bin` directory of the Vision SDK directory has execute permission.

        ```bash
        cd mxVision/bin
        ls -l uninstall.sh
        ```

        If the script does not have execute permission, run the following command to grant execute permission to the `uninstall.sh` script.

        ```bash
        chmod u+x uninstall.sh

        ./uninstall.sh
        ```

    2. Go to the path of the Vision SDK package.

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --uninstall
        ```

2. When you run the uninstall script, it first checks whether services under the current Vision SDK are running. If it detects that services under the current Vision SDK are running, it displays an error message, interrupts the uninstallation process, and returns 255.

    ```text
    [WARN ][Uninst  ] StreamServer is still running. Uninstallation aborted.
    [INFO ][Uninst  ] Uninstall MindX SDK failed.
    ```

    In this case, use the following command to check the running status of the services under the current Vision SDK. Check each process one by one. If a related process cannot be stopped for business reasons, consider stopping the uninstallation process.

    ```bash
    pgrep mxStreammanager | xargs ps -o cmd=-p | grep $(pwd)
    pgrep mxmfCommander | xargs ps -o cmd=-p | grep $(pwd)
    ps -ef | grep "python3 streamserverSourceCode/main.py"
    ```

    If no running services are detected under the current Vision SDK, the following prompt appears and the uninstallation process starts.

    ```text
    [INFO ][Uninst  ]No service is running. Uninstallation going.
    ```

    During uninstallation, the following prompt may appear in some folders.

    ```text
    rm: cannot remove 'config': Directory not empty
    ```

    This prompt appears because configuration files are retained. You can ignore it.

### Appendix

**Table 1** Parameters and descriptions for Vision SDK package installation command
<a id="table-run-install-args"></a>

|Input Parameter|Description|
|--|--|
|--help \| -h|Query help information.|
|--info|Query package build information.|
|--list|Query the file list.|
|--check|Query package integrity.|
|--quiet \| -q|Enable silent mode and accept the [Huawei Software Download License](../../mxBase/build/agreement.conf) by default. Use it together with `--install` or `--upgrade`.|
|--noexec|Do not run internal scripts.|
|--extract=\<path>|Extract directly to the target directory. The path can be absolute or relative.<br>This is usually used together with `--noexec` to extract files without running them.|
|--tar arg1 [arg2 ...]|Access the contents of the archive through the tar command.|
|--install|Perform installation. The current path cannot contain invalid characters. Only uppercase and lowercase letters, digits, and the special characters `-`, `_`, `.`, and `/` are supported.|
|--install-path=\<path>|(Optional) Customize the root directory for package installation. If not set, the current command execution directory is used by default.<li>It is recommended that you install the development kit by using an absolute path. Avoid using a relative path when you specify the installation path.</li><li>This conflicts with the `--version` input parameter. Do not install Vision SDK in `/tmp`.</li><li>Use it together with `--install` or `--upgrade`.</li><li>The path value cannot contain invalid characters. Only uppercase and lowercase letters, digits, and the special characters `-`, `_`, `.`, and `/` are supported.</li>|
|--uninstall|Uninstall. This is valid only for the installation package in the same directory as the `.run` package. For details, see [Uninstallation](#uninstallation). The current path cannot contain invalid characters. Only uppercase and lowercase letters, digits, and the special characters `-`, `_`, `.`, and `/` are supported.|
|--cann-path|Custom installation path of CANN. If CANN is installed to a user-defined path, import it through this parameter. For example, `/home/xxx/Ascend`.|
|--upgrade|Upgrade Vision SDK. For details, see [Upgrade](#upgrade).|
|--version|Query Vision SDK version.|
|--choose-gcc=\<0,1>|Choose the corresponding `.run` package for installation based on the GCC version. Use it together with `--install` or `--upgrade`.<li>`0`: Install the GCC 7 compiler version. This is the default value.</li><li>`1`: Install the GCC 4.8.5 compiler version.</li>|
|--nox11|Deprecated interface. It has no practical effect. If it has already been used, use it together with `--install` or `--upgrade`.|

>[!NOTE]
>The following parameters are not shown in the `--help` parameter. Do not use them directly.
>
>- `--xwin`: Run in xwin mode.
>- `--phase2`: Require the second action to be executed.

**Table 1** Parameters and descriptions for Vision SDK package upgrade command
<a id="table-run-update-args"></a>

|Parameter Name|Parameter Description|
|--|--|
|--upgrade|Vision SDK package upgrade command. It upgrades the development kit to the version included in the installation package.|
|--install-path|Optional. Customize the root directory for package installation. If not set, the current command execution directory is used by default.<br>If you use a custom directory for installation, it is recommended that you use this parameter during the upgrade.|
|--quiet|Enable silent mode and accept the [Huawei Software Download License](../../mxBase/build/agreement.conf) by default. Use it together with `--install` or `--upgrade`.|
