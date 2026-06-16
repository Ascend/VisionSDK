# 安装部署

## 安装说明

Vision SDK支持物理机部署和容器部署两种方式，本文档介绍在物理机内部署的方式。

如果需要在容器中进行部署，只需要完成系统依赖的安装，然后参考[VisionSDK镜像](https://www.hiascend.com/developer/ascendhub/detail/9e0edaf9488b447b951072c5c61ce8f1)启动。

**注意事项**

如需安装Vision SDK软件包以外的第三方软件，请注意及时升级最新版本，关注并修补存在的漏洞。

## 安装依赖

### Ubuntu系统

Ubuntu系统环境中所需依赖名称、对应版本及获取建议请参见[表1](#table-ubuntu-system)。

**表 1**  Ubuntu系统依赖名称对应版本
<a id="table-ubuntu-system"></a>

|依赖名称|版本建议|获取建议|
|--|--|--|
|gcc|7.3.0|建议通过获取源码包编译安装，安装步骤可参考[安装7.3.0版本gcc](appendix.md#安装730版本gcc)。|
|make|不低于3.82|建议通过包管理安装，安装命令参考如下。<br>``` sudo apt-get install -y make ```<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。|
|cmake|不低于3.5.2|建议通过包管理安装，安装命令参考如下。<br>```sudo apt-get install -y cmake```<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。|
|xz|不低于5.2.2|建议通过包管理安装，安装命令参考如下。<br>```sudo apt-get install -y xz-utils```<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。|
|protobuf|4.25.1|建议通过pip安装，安装命令参考如下。<br>```pip3 install protobuf==4.25.1```|
|Python|3.9/3.10/3.11/3.12|建议通过获取源码包编译安装，安装步骤可参考[安装Python依赖](appendix.md#安装python依赖)。|
|haveged（可选）|-|使用StreamServer功能时需要安装该依赖，建议通过包管理安装，安装命令参考如下。<br>```sudo apt-get install -y haveged```|

参考如下命令，检查是否已安装GCC、Make、CMake等依赖软件。

```bash
gcc --version
make --version
cmake --version
xz --version
python3 --version
pip3 list|grep protobuf
```

若分别返回如下信息，说明相应软件已安装（以下回显仅为示例，请以实际情况为准）。

```bash
gcc (Ubuntu 7.3.0-3ubuntu1~18.04) 7.3.0
GNU Make 3.82
cmake version 3.5.2
xz (XZ Utils) 5.2.2
Python 3.9.2
protobuf           4.25.1
```

### CentOS系统

CentOS系统环境中所需依赖名称、对应版本及获取建议请参见[表1](#table-centos-system)。

**表 1** CentOS系统依赖名称对应版本
<a id="table-centos-system"></a>

|依赖名称|版本建议|获取建议|
|--|--|--|
|gcc|7.3.0|建议通过获取源码包编译安装，安装步骤可参考[安装7.3.0版本gcc](appendix.md#安装730版本gcc)。|
|make|不低于3.82|建议通过包管理安装，安装命令参考如下。<br>```sudo yum install -y make```<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。|
|cmake|不低于3.5.2|建议通过包管理安装，安装命令参考如下。<br>```sudo yum install -y cmake```<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。|
|xz|不低于5.2.2|建议通过包管理安装，安装命令参考如下。<br>```sudo yum install -y xz```<br>若包管理中的版本不符合最低版本要求，可自行通过源码方式安装。|
|protobuf|4.25.1|建议通过pip安装，安装命令参考如下。<br>```pip3 install protobuf==4.25.1```|
|Python|3.9|建议通过获取源码包编译安装，安装步骤可参考[安装Python依赖](appendix.md#安装python依赖)。|
|haveged（可选）|-|使用StreamServer功能时需要安装该依赖，建议通过包管理安装，安装命令参考如下。<br>```sudo yum install -y haveged```|

参考如下命令，检查是否已安装GCC、Make、CMake等依赖软件。

```bash
gcc --version
make --version
cmake --version
xz --version
python3 --version
pip3 list|grep protobuf
```

若分别返回如下信息，说明相应软件已安装（以下回显仅为示例，请以实际情况为准）。

```bash
gcc 7.3.0
GNU Make 3.82
cmake version 3.5.2
xz (XZ Utils) 5.2.2
Python 3.9.2
protobuf           4.25.1
```

### 安装NPU驱动固件和CANN

1. 参考[CANN安装指南](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/900/softwareinst/instg/instg_0000.html)，使用CANN 9.0.0及对应驱动版本完成NPU驱动固件和CANN的安装，CANN需要包含toolkit和ops。

## 安装方式

### 在线安装

**安装步骤**

1. 从pip镜像源安装指定版本。

    ```bash
    python3 -m pip install --index-url <pip_index_url> visionsdk=={version}
    ```

2. 安装完成后，执行如下命令检查pip包信息。

    ```bash
    python3 -m pip show visionsdk
    ```

### 离线安装

#### pip安装

Vision SDK支持通过pip安装`visionsdk` wheel包。

**安装须知**

- pip安装方式要求安装环境拥有Python3
- 安装环境需要已完成NPU驱动、固件和CANN安装，并确保CANN运行库可被系统找到。若CANN环境变量未生效，请先执行CANN环境变量配置脚本，具体路径以实际安装路径为准。

    ```bash
    source /usr/local/Ascend/ascend-toolkit/set_env.sh
    ```

- 若环境中缺少驱动侧动态库路径，请根据实际安装路径补充`LD_LIBRARY_PATH`。例如：

    ```bash
    export LD_LIBRARY_PATH=/usr/local/Ascend/driver/lib64:/usr/local/Ascend/driver/lib64/common:/usr/local/Ascend/driver/lib64/driver:$LD_LIBRARY_PATH
    ```

- pip安装方式会将Vision SDK运行时安装到当前Python环境的`site-packages/visionsdk/runtime`目录。导入`visionsdk`后，会在当前Python进程内自动设置`MX_SDK_HOME`、`GST_PLUGIN_PATH`、`GST_PLUGIN_SCANNER`、`LD_LIBRARY_PATH`、`ASCEND_CUSTOM_OPP_PATH`等环境变量。
- pip安装方式的wheel包内置Vision SDK运行时依赖，并在构建wheel时将AscendC自定义算子预部署到`site-packages/visionsdk/runtime/operators/ascendc`目录。安装后`ASCEND_CUSTOM_OPP_PATH`会指向wheel运行时中的算子目录，不需要向CANN安装目录写入这部分AscendC算子。
- 若运行C++样例、shell脚本或其他非Python进程，需要先使wheel运行时环境变量生效。可先通过`python3 -m pip show visionsdk`确认安装路径，再执行`source <site-packages>/visionsdk/runtime/set_env.sh`。
- pip安装方式不会执行run包安装脚本中的系统级安装、部署日志记录、CANN目录写入和卸载脚本逻辑。如需完整系统级部署，或依赖必须安装到CANN目录下的算子部署流程，请使用run包安装方式。

**安装步骤**

1. 使用pip安装。

    使用本地wheel文件安装：

    ```bash
    python3 -m pip install ./visionsdk-{version}-py3-none-linux_{arch}.whl
    ```

2. 若环境中已安装旧版本，建议使用如下命令重新安装。

    ```bash
    python3 -m pip install --force-reinstall visionsdk=={version}
    ```

3. 安装完成后，执行如下命令检查pip包信息。

    ```bash
    python3 -m pip show visionsdk
    ```

**安装验证**

安装完成后，可执行如下命令验证Python接口是否可正常导入。

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

若输出`import ok`，且`preload errors`为空，表示pip安装方式的Python运行环境验证通过。此时`MX_SDK_HOME`应指向当前Python环境下的`site-packages/visionsdk/runtime`目录。

若需要验证C++样例或shell脚本，可先执行如下命令使wheel运行时环境变量生效：

```bash
SDK_RUNTIME=$(python3 - <<'PY'
import visionsdk
print(visionsdk.get_runtime_home())
PY
)
source "${SDK_RUNTIME}/set_env.sh"
```

执行后可检查AscendC自定义算子动态库是否存在：

```bash
ls -l "${MX_SDK_HOME}/operators/ascendc/vendors/customize/op_api/lib/libcust_opapi.so"
```

**常见问题**

- 若导入时报`libascendcl.so`、`libascend_hal.so`等动态库找不到，请检查NPU驱动、固件、CANN是否安装完成，并确认CANN及驱动动态库路径已加入`LD_LIBRARY_PATH`。
- 若导入时报`libmxbase.so`、`libstreammanager.so`等动态库找不到，请确认安装的是匹配当前CPU架构的`visionsdk` wheel包，并重新执行安装验证命令。
- 若运行涉及`Abs`、`Absolute`等AscendC自定义算子的用例时报`aclnnAbsCustomGetWorkspaceSize does not support`或提示`libcust_opapi.so`异常，请确认已安装包含AscendC算子预部署修正的wheel包，并确认`${MX_SDK_HOME}/operators/ascendc/vendors/customize/op_api/lib/libcust_opapi.so`存在。C++样例运行前需先执行`source <site-packages>/visionsdk/runtime/set_env.sh`。
- 若执行`pip install visionsdk=={version}`提示找不到版本，请确认对应版本wheel包已上传到pip镜像源，且版本号与wheel包元数据一致。

#### run包安装

**获取所需软件包**

|组件名称|软件包|获取链接|
|--|--|--|
|Vision SDK|Vision SDK软件包|[获取链接](https://www.hiascend.com/zh/developer/download/community/result?module=sdk+cann)|


**安装须知**

- 安装和运行Vision SDK的用户，需要满足：
    - 安装和运行Vision SDK的用户建议为普通用户。使用root用户运行程序时，可能存在权限篡改的安全风险。
    - 安装和运行Vision SDK的用户需为同一用户。
    - 安装Vision SDK和toolkit的用户需为同一用户。

- 软件包的安装、升级、卸载及版本查询相关的日志会保存至“\~/log/mindxsdk/deployment.log”文件；完整性校验、提取文件、tar命令访问相关的日志会保存至“\~/log/makeself/makeself.log”文件。用户可查看相应文件，完成后续的日志跟踪及审计。
- 安装Vision SDK会将算子拷贝到CANN的安装路径下，因此安装Vision SDK后，如果卸载重新安装CANN，会造成找不到算子，此时需要重新安装Vision SDK。

**安装步骤**

1. 登录安装环境。
2. 将Vision SDK软件包上传到安装环境的任意路径下并进入软件包所在路径。
3. 增加对软件包的可执行权限。

    ```bash
    chmod u+x Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run
    ```

4. 执行如下命令，校验软件包的一致性和完整性。

    ```bash
    ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --check
    ```

    如果系统没有shasum或者sha256sum工具则会校验失败，此时需要自行安装shasum或者sha256sum工具。

    若显示如下信息，说明软件包满足一致性和完整性。

    ```text
    Verifying archive integrity...  100%   SHA256 checksums are OK. All good.
    ```

5. 创建Vision SDK软件包的安装路径。不建议在“/tmp”路径下安装Vision SDK。
    - 若用户想指定安装路径，需要先创建安装路径。以安装路径“/home/work/Mind\_SDK”为例：

        ```bash
        mkdir -p /home/work/Mind_SDK
        ```

    - 若用户未指定安装路径，软件会默认安装到Vision SDK软件包所在的路径。

6. 进入软件包的上传路径，参考以下命令安装Vision SDK（安装路径的相关约束请参考[表1](#table-run-install-args)中<b>--install-path</b>的相关描述）。安装Vision SDK时会弹出确认是否接受下载许可协议的说明，若需要在安装时直接跳过该步骤，可在安装命令前增加`echo y |`，表示同意[华为软件下载许可](../../mxBase/build/agreement.conf)。

    - 若用户指定了安装路径。以安装路径“/home/work/Mind\_SDK”为例：

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install --install-path=/home/work/Mind_SDK
        ```

        或者

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install --install-path=/home/work/Mind_SDK
        ```

    - 若用户未指定安装路径，将安装在当前路径。

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
        ```

        或者

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
        ```

    >[!NOTE]
    >--install安装命令同时支持输入可选参数，如[表1](#table-run-install-args)所示。

7. 安装过程中提示 "Do you accept the LICENSE to install VisionSDK?[Y/N]" 时，输入Y或y，表示同意下载协议，继续进行安装；输入其他字符时停止安装，退出程序。

8. 安装完成后，若未出现错误信息，表示软件成功安装于指定或默认路径下。

    ```text
    Successfully installed mindx-xxx
    ```

9. 环境变量生效。

    进入Vision SDK的安装路径，运行以下命令，使Vision SDK的环境变量生效。

    ```bash
    source set_env.sh
    ```

>[!NOTE]
> 部分接口通过AscendC算子实现，安装部署后在安装路径下会生成AscendC算子相关文件。

#### 源码安装

本节介绍如何通过源码编译生成 Vision SDK的run包和whl包，执行下述操作前请确保拉取Vision SDK代码仓并进入工程目录。

1. 编译依赖下载

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

2. 执行编译

    ```bash
    cd VisionSDK
    mkdir -p ../ci/config && echo "version: 1.0.0" > ../ci/config/config.ini
    # arm架构执行
    bash build_all.sh arm-gcc4 aarch64 notest
    # x86架构执行
    bash build_all.sh x86-gcc4 x86_64 notest
    ```

3. 验证产品构建包

    执行`build_all.sh`后，会同时在如下目录生成用于[pip安装](#pip安装)的whl包和[run包安装](#run包安装)和run包：
    ```text
    output/
        `-- Software
            `-- mxVision
                |-- Ascend-mindxsdk-mxvision_1.0.0_linux-aarch64.run
                `-- wheel
                    `-- visionsdk-1.0.0-py3-none-linux_aarch64.whl
    ```

4. 测试构建

    ```bash
    # 安装lcov2.0用于统计测试覆盖率和生成可视化报告
    apt update
    apt install -y libcapture-tiny-perl libdatetime-perl libtimedate-perl
    wget https://github.com/linux-test-project/lcov/releases/download/v2.0/lcov-2.0.tar.gz
    tar -xzf lcov-2.0.tar.gz && cd lcov-2.0
    make install
    # 下载测试依赖
    cd VisionSDK/opensource/opensource
    git clone -b v2.7.x-h3 https://gitcode.com/cann-src-third-party/mockcpp.git mockcpp_patch
    git clone -b mindsdk https://gitcode.com/Ascend/mockcpp.git mockcpp
    git clone -b release-1.11.0 https://gitcode.com/GitHub_Trending/go/googletest.git googletest
    cd VisionSDK
    # arm架构执行
    bash build_all.sh arm-gcc4 aarch64 test
    # x86架构执行
    bash build_all.sh x86-gcc4 x86_64 test
    ```

## 升级

Vision SDK支持pip安装和run包安装，两种安装方式的升级命令不同，请根据实际安装方式选择。

**pip安装方式升级**

1. 通过pip安装的Vision SDK可使用如下命令升级到指定版本：

    ```bash
    python3 -m pip install --upgrade visionsdk=={version}
    ```

2. 若需要从本地wheel包升级，可执行：

    ```bash
    python3 -m pip install --upgrade ./visionsdk-{version}-py3-none-linux_{arch}.whl
    ```

3. 若环境中已安装旧版本且需要强制重新安装，可执行：

    ```bash
    python3 -m pip install --force-reinstall visionsdk=={version}
    ```

**run包安装方式升级**

1. 请参见[run包安装](#run包安装)获取并上传软件包。
2. 增加对软件包的可执行权限。

    ```bash
    chmod u+x Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run
    ```

3. 使用软件包升级命令升级当前Vision SDK软件包，升级命令参考如下，相关参数说明请参见[表1](#table-run-update-args)。升级Vision SDK时会弹出确认是否接受下载许可协议的说明，若需要在升级时直接跳过该步骤，可在升级命令前增加`echo y |`，表示同意[华为软件下载许可](../../mxBase/build/agreement.conf)。

    ```bash
    ./*.run --upgrade --install-path={MX_SDK_HOME}
    ```

    或者

    ```bash
    ./*.run --upgrade --install-path={MX_SDK_HOME}
    ```

    *.run为获取的Vision SDK软件包名，请用户自行替换。


4. 升级过程中提示 "Do you accept the LICENSE to install VisionSDK?[Y/N]" 时，输入Y或y，表示同意下载协议，继续进行升级；输入其他字符时停止升级，退出程序。

5. 执行如下命令可查询版本升级记录。

    ```bash
    cd ~/log/mindxsdk/
    cat deployment.log
    ```

    如下为升级回显示例：

    ```text
    MindX SDK mxVision:  5.0.RC2  ->  MindX SDK mxVision:  7.3.0
    ```

## 卸载

Vision SDK支持pip安装方式和run包安装方式，两种安装方式的卸载操作不同，请根据实际安装方式选择。

**pip安装方式卸载**

1. 通过pip安装的Vision SDK可使用如下命令卸载：

    ```bash
    python3 -m pip uninstall visionsdk
    ```

**run包安装方式卸载**

>[!NOTE]
>
>- 以下说明中的脚本卸载和软件包卸载仅适用于run包安装方式。
>- run包安装方式在卸载之前会检查当前Vision SDK是否仍有服务正在运行使用。卸载过程中会保留用户的数据和配置。卸载属于高危操作，请确保没有服务正在使用SDK后，再执行卸载操作。
>- run包安装方式在卸载时会同时删除Vision SDK相关算子文件，算子文件安装目录为“\$\{ASCEND\_OPP\_PATH\}/vendors/customize\_vision”  ，其中$\{ASCEND\_OPP\_PATH\}为[安装Vision SDK](#安装vision-sdk)时设置的CANN环境变量目录；如果环境中存在多个Vision SDK，卸载后可能会造成找不到算子，此时需要重新安装Vision SDK。

1. 任意选择卸载方式
    1. 进入Vision SDK的安装路径，确认Vision SDK目录下“bin”目录中的“uninstall.sh”脚本是否有可执行权限。

        ```bash
        cd mxVision/bin
        ls -l uninstall.sh
        ```

        若脚本没有可执行权限，请执行如下命令，给予“uninstall.sh”脚本可执行权限。

        ```bash
        chmod u+x uninstall.sh

        ./uninstall.sh
        ```

    2. 进入Vision SDK软件包所在路径

        ```bash
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --uninstall
        ```


2. 执行卸载脚本时，卸载脚本会先行检测当前Vision SDK下的服务是否正在运行。若检测到有当前Vision SDK下的服务正在运行，则会出现错误提示，并中断卸载过程，脚本返回值为255。

    ```text
    [WARN ][Uninst  ] StreamServer is still running. Uninstallation aborted.
    [INFO ][Uninst  ] Uninstall MindX SDK failed.
    ```

    此时应该使用以下命令检查当前Vision SDK下的服务的运行情况。并逐个检查进程的运行情况。若相关进程因业务原因不可停止，应考虑停止卸载过程。

    ```bash
    pgrep mxStreammanager | xargs ps -o cmd=-p | grep $(pwd)
    pgrep mxmfCommander | xargs ps -o cmd=-p | grep $(pwd)
    ps -ef | grep "python3 streamserverSourceCode/main.py"
    ```

    若未检测到任何当前Vision SDK下的正在运行的服务，则会出现以下提示，并开始卸载过程。

    ```text
    [INFO ][Uninst  ]No service is running. Uninstallation going.
    ```

    卸载过程可能在部分文件夹中产生如下提示。

    ```text
    rm: cannot remove 'config': Directory not empty
    ```

    此类提示是由于保留配置文件而产生的，可忽略相关提示。

### 附录

**表 1**  Vision SDK软件包安装命令参数及说明
<a id="table-run-install-args"></a>

|输入参数|含义|
|--|--|
|--help \| -h|查询帮助信息。|
|--info|查询包构建信息。|
|--list|查询文件列表。|
|--check|查询包完整性。|
|--quiet \| -q|启用静默模式，并默认接受[华为软件下载许可](../../mxBase/build/agreement.conf)，需要和--install或--upgrade参数配合使用。|
|--noexec|不执行内部脚本。|
|--extract=\<path>|直接提取到目标目录（绝对路径或相对路径）。<br>通常与--noexec选项一起使用，仅用于提取文件而不运行它们。|
|--tar arg1 [arg2 ...]|通过tar命令访问归档文件的内容。|
|--install|执行安装。当前路径不能存在非法字符，仅支持大小写字母、数字、-_./特殊字符。|
|--install-path=*\<path>*|（可选）自定义软件包安装根目录。如未设置，默认为当前命令执行所在目录。<li>建议用户使用绝对路径安装开发套件，在指定安装路径时请避免使用相对路径。</li><li>与“--version”输入参数有冲突，不建议在“/tmp”路径下安装Vision SDK。</li><li>需要和--install或--upgrade参数配合使用。</li><li>传入参数路径不能存在非法字符，仅支持大小写字母、数字、-_./特殊字符。</li>|
|--uninstall|卸载，仅对run包同目录下的安装包有效。具体使用操作请参见[卸载](#卸载)。当前路径不能存在非法字符，仅支持大小写字母、数字、-_./特殊字符。|
|--cann-path|CANN自定义安装的路径，如CANN安装路径为用户自定义，请通过该参数导入。比如“/home/xxx/Ascend”。|
|--upgrade|升级Vision SDK。具体使用操作请参见[升级](#升级)。|
|--version|查询Vision SDK的版本。|
|--choose-gcc=\<0,1>|根据gcc的版本选择对应的run包进行安装。需要和--install或--upgrade参数配合使用。<li>0：安装gcc 7的编译器版本，默认值。</li><li>1：安装gcc 4.8.5的编译器版本。</li>|
|--nox11|废弃接口，无实际作用。若已使用，需要和--install或--upgrade参数配合使用。|

>[!NOTE]
>以下参数未展示在--help参数中，用户请勿直接使用。
>
>- --xwin：使用xwin模式运行。
>- --phase2：要求执行第二步动作。


**表 1** Vision SDK软件包升级命令参数及说明
<a id="table-run-update-args"></a>

|参数名|参数说明|
|--|--|
|--upgrade|Vision SDK软件包升级操作命令，将开发套件升级到安装包所包含的版本。|
|--install-path|（可选）自定义软件包安装根目录。如未设置，默认为当前命令执行所在目录。<br>如使用自定义目录安装，建议在升级操作时使用该参数。|
|--quiet|启用静默模式，并默认接受[华为软件下载许可](../../mxBase/build/agreement.conf)，需要和--install或--upgrade参数配合使用。|
