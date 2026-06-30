# Installation Guide

## Installation Notes

Vision SDK supports deployment on a physical machine and container deployment. This document describes deployment on a physical machine. If you need to deploy it in a container, log in to [Ascend Hub](https://www.hiascend.com/developer/ascendhub) to obtain the corresponding base image. The installation and deployment steps in a container that uses the base image are the same as those for deployment on a physical machine.

For the specific base image configuration method, see [ascend-infer](https://www.hiascend.com/developer/ascendhub/detail/e02f286eef0847c2be426f370e0c2596).

**Notes**

If you need to install third-party software in addition to Vision SDK package, ensure that you update it to the latest version promptly and fix any existing vulnerabilities.

## Installing Dependencies

### Ubuntu

For the required dependency names, recommended versions, and acquisition suggestions in the Ubuntu environment, see [Table 1](#table20540329125613).

**Table 1** Ubuntu dependency names and corresponding versions
<a name="table20540329125613"></a>

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

For the required dependency names, recommended versions, and acquisition suggestions in the CentOS environment, see [Table 1](#table20540329125613).

**Table 1** CentOS dependency names and corresponding versions
<a id="table20540329125613"></a>

|Dependency|Recommended Version|Acquisition Suggestion|
|--|--|--|
|GCC|7.3.0|It is recommended that you obtain the source package and build it for installation. For installation steps, see [Install GCC 7.3.0](appendix.md#installing-gcc-730).|
|make|Not lower than 3.82|It is recommended that you install it through the package manager. See the following installation command.<br>```sudo yum install -y make```<br>If the version in the package manager does not meet the minimum version requirement, you can install it from source.|
|CMake|Not lower than 3.5.2|It is recommended that you install it through the package manager. See the following installation command.<br>```sudo yum install -y cmake```<br>If the version in the package manager does not meet the minimum version requirement, you can install it from source.|
|XZ|Not lower than 5.2.2|It is recommended that you install it through the package manager. See the following installation command.<br>```sudo yum install -y xz```<br>If the version in the package manager does not meet the minimum version requirement, you can install it from source.|
|protobuf|4.25.1|It is recommended that you install it with pip. See the following installation command.<br>```pip3 install protobuf==4.25.1```|
|Python|3.9|It is recommended that you obtain the source package and build it for installation. For installation steps, see [Installing Python Dependencies](appendix.md#installing-python-dependencies).|
|haveged (optional)|-|You need to install this dependency when you use the StreamServer feature. It is recommended that you install it through the package manager. See the following installation command.<br>```sudo yum install -y haveged```|

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

**Downloading Dependency Packages**

Vision SDK depends on the npu-driver driver package, npu-firmware firmware package, and CANN package. For related dependencies, see [Table 1](#table1624445102817).

**Table 1** Package list<a id="table1624445102817"></a>

<table>
<tr>
<th>Software Type</th>
<th>Package</th>
<th>Acquisition Method</th>
</tr>
<tr>
<td>Ascend NPU driver</td>
<td>Ascend-hdk-{npu_type}-npu-driver_{version}_linux-{arch}.run</td>
<td rowspan="4">Click the <a href="https://www.hiascend.com/developer/download/commercial/result?module=cann">download link</a>, configure the package in the "Edit resource selection" area for the supporting resources on the left, filter the matching software packages, and obtain the required packages after you confirm the version information.</td>
</tr>
<tr>
<td>Ascend NPU firmware</td>
<td>Ascend-hdk-{npu_type}-npu-firmware_{version}.run</td>
</tr>
<tr>
<td>CANN package</td>
<td>Ascend-cann-toolkit_{version}_linux-{arch}.run</td>
</tr>
<tr>
<td>CANN operator package</td>
<td>Ascend-cann-{npu_type}-ops_{version}_linux-{arch}.run</td>
</tr>
</table>

>[!NOTE]
>
>- `{version}` indicates the software version number.
>- `{arch}` indicates the CPU architecture.
>- `{npu_type}` indicates the chip name.

**Installing the NPU Driver Firmware and CANN**

1. Refer to the "Installing NPU Driver and Firmware" section in the *CANN Software Installation Guide* for the commercial edition or the community edition to install the NPU driver firmware.
2. Refer to the "Installing CANN" section in the *CANN Software Installation Guide* for the commercial edition or the community edition to install CANN.

    >[!NOTE]
    >- The user who installs CANN and the user who installs Vision SDK must be the same user. A regular user is recommended.
    >- When you install CANN, install the related dependencies as well to ensure that Vision SDK works properly.
    >- Vision SDK depends on the CANN dynamic library files at runtime. Ensure that the files in the CANN installation path are valid and have not been illegally modified.

## Obtaining Vision SDK Package

Obtain the required package and its digital signature file according to this chapter.

|Component|Package|Link|
|--|--|--|
|Vision SDK|Vision SDK package|[Download link](https://www.hiascend.com/zh/developer/download/community/result?module=sdk+cann)|

**Verifying the Software Digital Signature**

To prevent the package from being tampered with during transfer or storage, download the corresponding digital signature file when you download the package for integrity verification.

After you download the package, refer to the following link to view the OpenPGP signature verification guide and verify the PGP digital signature of the downloaded package. If the verification fails, do not use the package and contact a Huawei technical support engineer.

Before you install or upgrade with a package, also verify the digital signature of the package according to the preceding process to ensure that the package has not been tampered with.

Carrier customers, visit: [https://support.huawei.com/carrier/digitalSignatureAction](https://support.huawei.com/carrier/digitalSignatureAction)

Enterprise customers, visit: [https://support.huawei.com/enterprise/zh/tool/software-digital-signature-openpgp-validation-tool-TL1000000054](https://support.huawei.com/enterprise/zh/tool/software-digital-signature-openpgp-validation-tool-TL1000000054)

## Installing Vision SDK

**Installation Instructions**

- The user who installs and runs Vision SDK:
    - Must be the same user for installation and runtime.
    - Is advised to be a regular user. Running the program as the `root` user may pose a security risk because permissions can be tampered with.
    - Must also install the toolkit.

- Logs related to package installation, upgrade, uninstallation, and version queries are saved to the `~/log/mindxsdk/deployment.log` file. Logs related to integrity verification, file extraction, and tar command access are saved to the `~/log/makeself/makeself.log` file. You can review the corresponding files for later log tracing and auditing.
- Installing Vision SDK copies the operators to the CANN installation path. Therefore, if you uninstall and reinstall CANN after you install Vision SDK, the operators will not be found. In that case, reinstall Vision SDK.
- If you need to upgrade or uninstall Vision SDK, see [Upgrade](#upgrade) and [Uninstallation](#uninstallation).

**Installation Preparation**

- The environment deployment in the [Installing Dependencies](#installing-dependencies) section is complete.
- You have obtained Vision SDK package of the matching version from [Obtaining Vision SDK Package](#obtaining-vision-sdk-package).
- Ensure that you have run the CANN environment variable configuration script in the installation environment so that the environment variables take effect. Use the actual installation path when you run the script.

    ```bash
    # Install the toolkit package
    . /usr/local/Ascend/cann/set_env.sh # This is the default CANN installation path. Modify it according to the actual installation path
    ```

**Installation Steps**

1. Log in to the installation environment.
2. Upload Vision SDK package to any path in the installation environment and go to the package path.
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

5. Create the installation path for Vision SDK package. Do not install Vision SDK in `/tmp`.
    - If you want to specify an installation path, create the installation path first. For example, if the installation path is `/home/work/Mind_SDK`:

        ```bash
        mkdir -p /home/work/Mind_SDK
        ```

    - If you do not specify an installation path, the software is installed in the directory that contains Vision SDK package by default.

6. Go to the package upload path and install Vision SDK by using the following command. For constraints related to the installation path, see the description of `--install-path` in [Table 1](#table1361972315353). When you install Vision SDK, a prompt asks whether you accept the download license agreement. If you need to skip this step during installation, add `echo y |` before the installation command to indicate that you agree to the [Huawei Software Download License](../../mxBase/build/agreement.conf).

    - If you specify an installation path. For example, if the installation path is `/home/work/Mind_SDK`:

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install --install-path=/home/work/Mind_SDK
        ```

        or

        ```bash
        echo y | ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install --install-path=/home/work/Mind_SDK
        ```

    - If you do not specify an installation path, the software is installed in the current directory.

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
        ```

        or

        ```bash
        echo y | ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
        ```

    >[!NOTE]
    >The `--install` command also supports optional parameters, as shown in [Table 1](#table1361972315353).

7. When the prompt `Do you accept the LICENSE to install VisionSDK?[Y/N]` appears during installation, enter `Y` or `y` to accept the download agreement and continue. If you enter any other character, installation stops and the program exits.

8. After the installation is complete, if no error message appears, the software is successfully installed to the specified or default path.

    ```text
    Successfully installed mindx-xxx
    ```

    For detailed descriptions of the installation command parameters, see [Table 1](#table1361972315353).

9. Activate the environment variables.

    Go to Vision SDK installation path and run the following command to activate Vision SDK environment variables.

    ```bash
    source set_env.sh
    ```

10. After Vision SDK is installed, you can refer to [Quick Start](/quick_start.md) to verify the installation result of Vision SDK and gain an initial understanding of Vision SDK application development.

>[!NOTE]
> Some interfaces are implemented by AscendC operators. After installation and deployment, AscendC operator-related files are generated in the installation path.

**Reference**

**Table 1** Interface parameter table
<a id="table1361972315353"></a>

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
|--install-path=*\<path>*|(Optional) Customize the root directory for package installation. If not set, the current command execution directory is used by default.<li>It is recommended that you install the development kit by using an absolute path. Avoid using a relative path when you specify the installation path.</li><li>This conflicts with the `--version` input parameter. Do not install Vision SDK in `/tmp`.</li><li>Use it together with `--install` or `--upgrade`.</li><li>The path value cannot contain invalid characters. Only uppercase and lowercase letters, digits, and the special characters `-`, `_`, `.`, and `/` are supported.</li>|
|--uninstall|Uninstall. This is valid only for the installation package in the same directory as the `.run` package. For details, see [Uninstallation](#uninstallation). The current path cannot contain invalid characters. Only uppercase and lowercase letters, digits, and the special characters `-`, `_`, `.`, and `/` are supported.|
|--cann-path|The custom installation path of CANN. If CANN is installed to a user-defined path, import it through this parameter. For example, `/home/xxx/Ascend`.|
|--upgrade|Upgrade Vision SDK. For details, see [Upgrade](#upgrade).|
|--version|Query Vision SDK version.|
|--choose-gcc=\<0,1>|Choose the corresponding `.run` package for installation based on the GCC version. Use it together with `--install` or `--upgrade`.<li>`0`: Install the GCC 7 compiler version. This is the default value.</li><li>`1`: Install the GCC 4.8.5 compiler version.</li>|
|--nox11|Deprecated interface. It has no practical effect. If it has already been used, use it together with `--install` or `--upgrade`.|

>[!NOTE]
>The following parameters are not shown in the `--help` parameter. Do not use them directly.
>
>- `--xwin`: Run in xwin mode.
>- `--phase2`: Require the second action to be executed.

## Upgrade

**Procedure**

1. Obtain and upload the package by referring to [Obtaining Vision SDK Package](#obtaining-vision-sdk-package).
2. Grant execute permission on the package.

    ```bash
    chmod u+x Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run
    ```

3. Use the package upgrade command to upgrade the current Vision SDK package. See the following example for the upgrade command. For parameter details, see [Table 1](#table568416506475). When you upgrade Vision SDK, a prompt asks whether you accept the download license agreement. If you need to skip this step during upgrade, add `echo y |` before the upgrade command to indicate that you agree to the [Huawei Software Download License](../../mxBase/build/agreement.conf).

    ```bash
    ./*.run --upgrade --install-path={MX_SDK_HOME}
    ```

    or

    ```bash
    echo y | ./*.run --upgrade --install-path={MX_SDK_HOME}
    ```

    `*.run` is the name of Vision SDK package that you obtain. Replace it with the actual name.

    **Table 1** Parameters and descriptions for Vision SDK package upgrade command
    <a name="table568416506475"></a>

    |Parameter Name|Parameter Description|
    |--|--|
    |--upgrade|Vision SDK package upgrade command. It upgrades the development kit to the version included in the installation package.|
    |--install-path|Optional. Customize the root directory for package installation. If not set, the current command execution directory is used by default.<br>If you use a custom directory for installation, it is recommended that you use this parameter during the upgrade.|
    |--quiet|Enable silent mode and accept the [Huawei Software Download License](../../mxBase/build/agreement.conf) by default. Use it together with `--install` or `--upgrade`.|

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

>[!NOTE]
>
>- Before uninstallation, the system checks whether any Vision SDK services are still running. During uninstallation, user data and configuration are retained. Uninstallation is a high-risk operation. Ensure that no services are using the SDK before you run the uninstallation.
>- During uninstallation, Vision SDK-related operator files are deleted as well. The operator file installation directory is `${ASCEND_OPP_PATH}/vendors/customize_vision`, where `${ASCEND_OPP_PATH}` is the CANN environment variable directory set during [Installing Vision SDK](#installing-vision-sdk). If multiple Vision SDK instances exist in the environment, uninstalling may cause the operators to be unavailable. In that case, reinstall Vision SDK.

**Uninstallation by Script**

1. Go to Vision SDK installation path and check whether the `uninstall.sh` script in the `bin` directory of Vision SDK directory has execute permission.

    ```bash
    cd mxVision/bin
    ls -l uninstall.sh
    ```

    If the script does not have execute permission, run the following command to grant execute permission to the `uninstall.sh` script.

    ```bash
    chmod u+x uninstall.sh
    ```

2. Choose either of the following commands to start the uninstallation.

    ```bash
    ./uninstall.sh
    ```

    >[!NOTE]
    >Uninstallation with the `uninstall.sh` script applies only to normal installation paths. If you need to resolve installation exceptions or similar issues, uninstall by following [Uninstall by package](#section1824842918492).

3. When you run the uninstall script, it first checks whether services under the current Vision SDK are running. If it detects that services under the current Vision SDK are running, it displays an error message, interrupts the uninstallation process, and returns 255.

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

**Uninstallation by Package<a name="section1824842918492"></a>**

If you want to uninstall an installed package, perform the following steps:

1. Log in to the installation environment as the package installation user.
2. Go to the package path.
3. Run the following command to uninstall the package.

    ```bash
    ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --uninstall
    ```

4. When you run the uninstall script, it first checks whether services under the current Vision SDK are running. If it detects that services under the current Vision SDK are running, it displays an error message, interrupts the uninstallation process, and returns 255.

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
