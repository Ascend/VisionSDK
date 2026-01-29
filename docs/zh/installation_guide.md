# 安装部署<a name="ZH-CN_TOPIC_0000001557429500"></a>

## 安装说明<a name="ZH-CN_TOPIC_0000002039697745"></a>

Vision SDK支持物理机部署和容器部署两种方式，本文档介绍在物理机内部署的方式，如果需要在容器中部署，请登录[Ascend Hub](https://www.hiascend.com/developer/ascendhub)获取相应基础镜像，进入基础镜像对应容器之后的安装部署与物理机部署方式一致。

基础镜像具体配置方法请参考[ascend-infer](https://www.hiascend.com/developer/ascendhub/detail/e02f286eef0847c2be426f370e0c2596)。

**注意事项<a name="section1297475493911"></a>**

如需安装Vision SDK软件包以外的第三方软件，请注意及时升级最新版本，关注并修补存在的漏洞。


## 安装依赖<a name="ZH-CN_TOPIC_0000001933475646"></a>

### Ubuntu系统<a name="ZH-CN_TOPIC_0000001631987505"></a>

Ubuntu系统环境中所需依赖名称、对应版本及获取建议请参见[表1](#table20540329125613)。

**表 1**  Ubuntu系统依赖名称对应版本
<a name="table20540329125613"></a>

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


### CentOS系统<a name="ZH-CN_TOPIC_0000001632546921"></a>

CentOS系统环境中所需依赖名称、对应版本及获取建议请参见[表1](#table20540329125613)。

**表 1** CentOS系统依赖名称对应版本
<a id="table20540329125613"></a>

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


### 安装NPU驱动固件和CANN<a name="ZH-CN_TOPIC_0000001800403528"></a>

**下载依赖软件包<a name="section119752030133014"></a>**

Vision SDK使用依赖npu-driver驱动包、npu-firmware固件包和CANN软件包，相关依赖参见[表1](#table1624445102817)。

下载本软件即表示您同意[华为企业业务最终用户许可协议（EULA）](https://e.huawei.com/cn/about/eula)的条款和条件。

**表 1**  软件包清单
<a id="table1624445102817"></a>

<table>
<tr>
<th>软件类型</th>
<th>软件包名称</th>
<th>获取方式</th>
</tr>
<tr>
<td>昇腾NPU驱动</td>
<td>Ascend-hdk-<i>{npu_type}</i>-npu-driver_<i>{version}</i>_linux-<i>{arch}</i>.run</td>
<td rowspan="4">单击<a href="https://www.hiascend.com/developer/download/commercial/result?module=cann">获取链接</a>，在左侧配套资源的“编辑资源选择”中进行配置，筛选配套的软件包，确认版本信息后获取所需软件包。</td>
</tr>
<tr>
<td>昇腾NPU固件</td>
<td>Ascend-hdk-<i>{npu_type}</i>-npu-firmware_<i>{version}</i>.run</td>
</tr>
<tr>
<td>CANN软件包</td>
<td>Ascend-cann-toolkit_<i>{version}</i>_linux-<i>{arch}</i>.run</td>
</tr>
<tr>
<td>CANN算子包</sup></td>
<td>Ascend-cann-<i>{npu_type}</i>-ops_<i>{version}</i>_linux-<i>{arch}</i>.run</td>
</tr>
</tr>
</table>

>[!NOTE] 说明 
>-   <i>{version}</i>表示软件版本号。
>-   <i>{arch}</i>表示CPU架构。
>-   <i>{npu_type}</i>表示芯片名称。


**安装NPU驱动固件和CANN<a name="section451714713564"></a>**

1.  参考《CANN 软件安装指南》中的“安装NPU驱动和固件”章节（商用版）或“安装NPU驱动和固件”章节（社区版）安装NPU驱动固件。
2.  参考《CANN 软件安装指南》的“安装CANN”章节（商用版）或《CANN 软件安装指南》的“安装CANN”章节（社区版）安装CANN。

    >[!NOTE] 说明
    >-   安装CANN和安装Vision SDK的用户需为同一用户，建议为普通用户。
    >-   安装CANN时，为确保Vision SDK正常使用，CANN的相关依赖也需要一并安装。
    >-   Vision SDK运行时依赖CANN的动态库文件，请确保CANN安装路径下的文件有效且未被非法修改。



## 获取Vision SDK软件包<a name="ZH-CN_TOPIC_0000001607908293"></a>

请参考本章获取所需软件包和对应的数字签名文件，下载本软件即表示您同意[华为企业业务最终用户许可协议（EULA）](https://e.huawei.com/cn/about/eula)的条款和条件。

|组件名称|软件包|获取链接|
|--|--|--|
|Vision SDK|Vision SDK软件包|[获取链接](https://www.hiascend.com/zh/developer/download/community/result?module=sdk+cann)|


**软件数字签名验证<a name="section10830205518487"></a>**

为了防止软件包在传递过程中或存储期间被恶意篡改，下载软件包时请下载对应的数字签名文件用于完整性验证。

在软件包下载之后，请参考下方链接查看《OpenPGP签名验证指南》，对下载的软件包进行PGP数字签名校验。如果校验失败，请勿使用该软件包并联系华为技术支持工程师解决。

使用软件包安装/升级前，也需要按照上述过程，验证软件包的数字签名，确保软件包未被篡改。

运营商客户请访问：[https://support.huawei.com/carrier/digitalSignatureAction](https://support.huawei.com/carrier/digitalSignatureAction)

企业客户请访问：[https://support.huawei.com/enterprise/zh/tool/software-digital-signature-openpgp-validation-tool-TL1000000054](https://support.huawei.com/enterprise/zh/tool/software-digital-signature-openpgp-validation-tool-TL1000000054)


## 安装Vision SDK<a name="ZH-CN_TOPIC_0000001557588776"></a>

**安装须知<a name="section689153412565"></a>**

-   安装和运行Vision SDK的用户，需要满足：
    -   安装和运行Vision SDK的用户建议为普通用户。使用root用户运行程序时，可能存在权限篡改的安全风险。
    -   安装和运行Vision SDK的用户需为同一用户。
    -   安装Vision SDK和toolkit的用户需为同一用户。

-   软件包的安装、升级、卸载及版本查询相关的日志会保存至“\~/log/mindxsdk/deployment.log”文件；完整性校验、提取文件、tar命令访问相关的日志会保存至“\~/log/makeself/makeself.log”文件。用户可查看相应文件，完成后续的日志跟踪及审计。
-   安装Vision SDK会将算子拷贝到CANN的安装路径下，因此安装Vision SDK后，如果卸载重新安装CANN，会造成找不到算子，此时需要重新安装Vision SDK。
-   如需升级或卸载Vision SDK，请参见[升级](#升级)、[卸载](#卸载)。

**安装准备<a name="section699612159153"></a>**

-   已完成[安装依赖](#安装依赖)章节的环境部署。
-   已通过[获取Vision SDK软件包](#获取vision-sdk软件包)获取配套版本的Vision SDK软件包。
-   确保安装环境中已执行CANN环境变量配置脚本，使环境变量生效。具体执行路径，请以实际安装为准。

    ```
    # 安装toolkit包
    . /usr/local/Ascend/cann/set_env.sh #此处为CANN默认安装路径，根据实际安装路径修改
    ```

**安装步骤<a name="section840011718150"></a>**

1.  登录安装环境。
2.  将Vision SDK软件包上传到安装环境的任意路径下并进入软件包所在路径。
3.  增加对软件包的可执行权限。

    ```
    chmod u+x Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run
    ```

4.  执行如下命令，校验软件包的一致性和完整性。

    ```
    ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --check
    ```

    如果系统没有shasum或者sha256sum工具则会校验失败，此时需要自行安装shasum或者sha256sum工具。

    若显示如下信息，说明软件包满足一致性和完整性。

    ```
    Verifying archive integrity...  100%   SHA256 checksums are OK. All good.    
    ```

5.  创建Vision SDK软件包的安装路径。不建议在“/tmp”路径下安装Vision SDK。
    -   若用户想指定安装路径，需要先创建安装路径。以安装路径“/home/work/Mind\_SDK”为例：

        ```
        mkdir -p /home/work/Mind_SDK
        ```

    -   若用户未指定安装路径，软件会默认安装到Vision SDK软件包所在的路径。

6.  进入软件包的上传路径，参考以下命令安装Vision SDK（安装路径的相关约束请参考[表1](#table1361972315353)中<b>--install-path</b>的相关描述）。安装Vision SDK时会弹出确认是否接受EULA协议的说明，若需要在安装时直接跳过该步骤，可在安装命令前增加**echo y |**，表示同意[华为企业业务最终用户许可协议（EULA）](https://e.huawei.com/cn/about/eula)。

    -   若用户指定了安装路径。以安装路径“/home/work/Mind\_SDK”为例：

        ```
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install --install-path=/home/work/Mind_SDK
        ```

        或者

        ```
        echo y | ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install --install-path=/home/work/Mind_SDK
        ```

    -   若用户未指定安装路径，将安装在当前路径。

        ```
        ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
        ```

        或者

        ```
        echo y | ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --install
        ```

    >[!NOTE] 说明 
    >--install安装命令同时支持输入可选参数，如[表1](#table1361972315353)所示。

7.  安装过程中提示“Do you accept the EULA to install mxVision?”时，输入Y或y，表示同意EULA协议，继续进行安装；输入其他字符时停止安装，退出程序。
8.  安装完成后，若未出现错误信息，表示软件成功安装于指定或默认路径下。

    ```
    Successfully installed mindx-xxx
    ```

    具体安装命令的接口参数介绍，请见[表1](#table1361972315353)。

9.  环境变量生效。

    进入Vision SDK的安装路径，运行以下命令，使Vision SDK的环境变量生效。

    ```
    source set_env.sh
    ```

10. Vision SDK安装完成后，可参考[快速入门](/quick_start.md)，验证Vision SDK安装结果，初步了解Vision SDK应用开发。

>[!NOTE] 说明 
>   部分接口通过AscendC算子实现，安装部署后在安装路径下会生成AscendC算子相关文件。

**相关参考<a name="section72943188425"></a>**

**表 1**  接口参数表
<a name="table1361972315353"></a>

|输入参数|含义|
|--|--|
|--help \| -h|查询帮助信息。|
|--info|查询包构建信息。|
|--list|查询文件列表。|
|--check|查询包完整性。|
|--quiet \| -q|启用静默模式，并默认接受[华为企业业务最终用户许可协议（EULA）](https://e.huawei.com/cn/about/eula)，需要和--install或--upgrade参数配合使用。|
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


>[!NOTE] 说明 
>以下参数未展示在--help参数中，用户请勿直接使用。
>-   --xwin：使用xwin模式运行。
>-   --phase2：要求执行第二步动作。



## 升级<a name="ZH-CN_TOPIC_0000001675719258"></a>

**操作步骤<a name="section977319017292"></a>**

1.  请参见[获取Vision SDK软件包](#获取vision-sdk软件包)获取并上传软件包。
2.  增加对软件包的可执行权限。

    ```
    chmod u+x Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run
    ```

3.  使用软件包升级命令升级当前Vision SDK软件包，升级命令参考如下，相关参数说明请参见[表1](#table568416506475)。升级Vision SDK时会弹出确认是否接受EULA协议的说明，若需要在升级时直接跳过该步骤，可在升级命令前增加**echo y |**，表示同意[华为企业业务最终用户许可协议（EULA）](https://e.huawei.com/cn/about/eula)。

    ```
    ./*.run --upgrade --install-path={MX_SDK_HOME}
    ```

    或者

    ```
    echo y | ./*.run --upgrade --install-path={MX_SDK_HOME}
    ```

    *.run为获取的Vision SDK软件包名，请用户自行替换。

    **表 1** Vision SDK软件包升级命令参数及说明
    <a name="table568416506475"></a>

    |参数名|参数说明|
    |--|--|
    |--upgrade|Vision SDK软件包升级操作命令，将开发套件升级到安装包所包含的版本。|
    |--install-path|（可选）自定义软件包安装根目录。如未设置，默认为当前命令执行所在目录。<br>如使用自定义目录安装，建议在升级操作时使用该参数。|
    |--quiet|启用静默模式，并默认接受[华为企业业务最终用户许可协议（EULA）](https://e.huawei.com/cn/about/eula)。需要和--install或--upgrade参数配合使用。|


4.  升级过程中提示Do you accept the EULA to install mxVision?时，输入Y或y，表示同意EULA协议，继续进行升级；输入其他字符时停止升级，退出程序。
5.  执行如下命令可查询版本升级记录。

    ```
    cd ~/log/mindxsdk/
    cat deployment.log
    ```

    如下为升级回显示例：

    ```
    MindX SDK mxVision:  5.0.RC2  ->  MindX SDK mxVision:  7.3.0
    ```


## 卸载<a name="ZH-CN_TOPIC_0000001560124198"></a>

>[!NOTE] 说明 
>-   在卸载之前会检查当前Vision SDK是否仍有服务正在运行使用。卸载过程中会保留用户的数据和配置。卸载属于高危操作，请确保没有服务正在使用SDK后，再执行卸载操作。
>-   用户在卸载时会同时删除Vision SDK相关算子文件，算子文件安装目录为“\$\{ASCEND\_OPP\_PATH\}/vendors/customize\_vision”  ，其中$\{ASCEND\_OPP\_PATH\}为[安装Vision SDK](#安装vision-sdk)时设置的CANN环境变量目录；如果环境中存在多个Vision SDK，卸载后可能会造成找不到算子，此时需要重新安装Vision SDK。

**脚本卸载操作步骤<a name="section423192952910"></a>**

1.  进入Vision SDK的安装路径，确认Vision SDK目录下“bin”目录中的“uninstall.sh”脚本是否有可执行权限。

    ```
    cd mxVision/bin
    ls -l uninstall.sh
    ```

    若脚本没有可执行权限，请执行如下命令，给予“uninstall.sh”脚本可执行权限。

    ```
    chmod u+x uninstall.sh
    ```

2.  选择执行以下任意一种命令，开始执行卸载。

    ```
    ./uninstall.sh
    ```

    >[!NOTE] 说明 
    >使用“uninstall.sh”脚本进行卸载操作仅适用于正常安装途径，如需解决安装异常等情况，请通过[软件包卸载操作步骤](#section1824842918492)进行卸载。

3.  执行卸载脚本时，卸载脚本会先行检测当前Vision SDK下的服务是否正在运行。若检测到有当前Vision SDK下的服务正在运行，则会出现错误提示，并中断卸载过程，脚本返回值为255。

    ```
    [WARN ][Uninst  ] StreamServer is still running. Uninstallation aborted.
    [INFO ][Uninst  ] Uninstall MindX SDK failed.
    ```

    此时应该使用以下命令检查当前Vision SDK下的服务的运行情况。并逐个检查进程的运行情况。若相关进程因业务原因不可停止，应考虑停止卸载过程。

    ```
    pgrep mxStreammanager | xargs ps -o cmd=-p | grep $(pwd)
    pgrep mxmfCommander | xargs ps -o cmd=-p | grep $(pwd)
    ps -ef | grep "python3 streamserverSourceCode/main.py"
    ```

    若未检测到任何当前Vision SDK下的正在运行的服务，则会出现以下提示，并开始卸载过程。

    ```
    [INFO ][Uninst  ]No service is running. Uninstallation going.
    ```

    卸载过程可能在部分文件夹中产生如下提示。

    ```
    rm: cannot remove 'config': Directory not empty
    ```

    此类提示是由于保留配置文件而产生的，可忽略相关提示。

**软件包卸载操作步骤<a name="section1824842918492"></a>**

如果用户想要对已安装的软件包进行卸载，可以执行如下步骤：

1.  以软件包的安装用户登录软件包的安装环境。
2.  进入软件包所在路径。
3.  执行以下命令卸载软件包。

    ```
    ./Ascend-mindxsdk-mxvision_{version}_linux-{arch}.run --uninstall
    ```

4.  执行卸载脚本时，卸载脚本会先行检测当前Vision SDK下的服务是否正在运行。若检测到有当前Vision SDK下的服务正在运行，则会出现错误提示，并中断卸载过程，脚本返回值为255。

    ```
    [WARN ][Uninst  ] StreamServer is still running. Uninstallation aborted.
    [INFO ][Uninst  ] Uninstall MindX SDK failed.
    ```

    此时应该使用以下命令检查当前Vision SDK下的服务的运行情况。并逐个检查进程的运行情况。若相关进程因业务原因不可停止，应考虑停止卸载过程。

    ```
    pgrep mxStreammanager | xargs ps -o cmd=-p | grep $(pwd)
    pgrep mxmfCommander | xargs ps -o cmd=-p | grep $(pwd)
    ps -ef | grep "python3 streamserverSourceCode/main.py"
    ```

    若未检测到任何当前Vision SDK下的正在运行的服务，则会出现以下提示，并开始卸载过程。

    ```
    [INFO ][Uninst  ]No service is running. Uninstallation going.
    ```

    卸载过程可能在部分文件夹中产生如下提示。

    ```
    rm: cannot remove 'config': Directory not empty
    ```

    此类提示是由于保留配置文件而产生的，可忽略相关提示。

