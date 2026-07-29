# VisionSDK Dev Container Dockerfile
# 基于官方 Dockerfile.910b.ubuntu 适配，移除 .run 包安装，提供源码开发所需的 opensource 构建环境
ARG CANN_VERSION=9.0.0

FROM swr.cn-south-1.myhuaweicloud.com/ascendhub/cann:${CANN_VERSION}-910b-ubuntu22.04-py3.11

ENV DEBIAN_FRONTEND="noninteractive"
ENV TZ="Asia/Shanghai"

ARG ASCEND_BASE=/usr/local/Ascend
ARG TOOLKIT_PATH=${ASCEND_BASE}/ascend-toolkit/latest

# ========== 1. 更换 apt 源为华为云镜像 ==========
RUN cp -a /etc/apt/sources.list /etc/apt/sources.list.bak && \
    sed -i "s@http://.*archive.ubuntu.com@https://mirrors.huaweicloud.com@g" /etc/apt/sources.list && \
    sed -i "s@http://.*security.ubuntu.com@https://mirrors.huaweicloud.com@g" /etc/apt/sources.list

# ========== 2. 安装系统依赖（含 opensource 构建所需） ==========
RUN apt update && \
    apt install --no-install-recommends -y \
        ca-certificates wget vim dos2unix net-tools ssh lsof sshpass \
        curl gcc cmake g++ make pkg-config unzip autoconf automake libtool \
        git patch zlib1g-dev xz-utils haveged \
        python3-pip python3-dev ninja-build bison flex perl gettext \
        libglib2.0-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
        libsrtp2-dev libcapture-tiny-perl libdatetime-perl libtimedate-perl \
        gdb valgrind \
        && apt clean && rm -rf /var/lib/apt/lists/*

# ========== 3. 配置 pip 为阿里云镜像 ==========
RUN mkdir -p ~/.pip && \
    echo "[global]" > ~/.pip/pip.conf && \
    echo "trusted-host=mirrors.aliyun.com" >> ~/.pip/pip.conf && \
    echo "index-url=https://mirrors.aliyun.com/pypi/simple" >> ~/.pip/pip.conf && \
    echo "timeout=200" >> ~/.pip/pip.conf

# ========== 4. 安装 Python 依赖 ==========
RUN pip3 install -U pip && \
    pip3 install decorator sympy==1.4 cffi==1.15.1 pyyaml pathlib2 protobuf==4.25.1 \
                 scipy requests attrs psutil numpy==1.26.4 && \
    rm -rf /root/.cache/pip

# ========== 5. 安装 lcov 和 googletest ==========
RUN wget https://github.com/linux-test-project/lcov/releases/download/v2.0/lcov-2.0.tar.gz && \
    tar -xzf lcov-2.0.tar.gz && cd lcov-2.0 && make install && \
    cd .. && rm -rf lcov-2.0.tar.gz lcov-2.0

RUN wget https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz && \
    cp release-1.11.0.tar.gz /tmp/googletest-release-1.11.0.tar.gz && \
    tar xf release-1.11.0.tar.gz && cd googletest-release-1.11.0 && \
    cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/usr/local/gtest . && \
    make -j12 && make install && \
    cd .. && rm -rf release-1.11.0.tar.gz googletest-release-1.11.0

# ========== 6. 设置 Ascend 环境变量 ==========
ENV LD_LIBRARY_PATH=${TOOLKIT_PATH}/runtime/lib64:${LD_LIBRARY_PATH} \
    TBE_IMPL_PATH=${TOOLKIT_PATH}/opp/op_impl/built-in/ai_core/tbe \
    PATH=${TOOLKIT_PATH}/atc/ccec_compiler/bin:${TOOLKIT_PATH}/atc/bin:${PATH} \
    ASCEND_OPP_PATH=${TOOLKIT_PATH}/opp \
    ASCEND_AICPU_PATH=${TOOLKIT_PATH} \
    PYTHONPATH=${TBE_IMPL_PATH}:${PYTHONPATH} \
    ASCEND_HOME=${ASCEND_BASE}

# ========== 7. 配置 bashrc ==========
RUN sed -i '$a\source /usr/local/Ascend/ascend-toolkit/set_env.sh' ~/.bashrc && \
    sed -i '$a\export LD_LIBRARY_PATH=/usr/local/gtest/lib:$LD_LIBRARY_PATH' ~/.bashrc && \
    sed -i '$a\export LD_LIBRARY_PATH=/usr/local/Ascend/driver/lib64/driver:$LD_LIBRARY_PATH' ~/.bashrc

# ========== 8. 设置工作目录 ==========
WORKDIR /workspace
