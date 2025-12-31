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
# Description: Build script of mxBase.
# Author: mxBase
# Create: 2025
# History: NA
set -e
echo "==============Start packing=============="
work_space="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)/../../"
frame_project=$1
platform=$2
version_num=$3
cd ${work_space}
if [ "${frame_project}" != "arm-gcc4" ] && [ "${frame_project}" != "x86-gcc4" ]; then
    echo "Error: Invalid package compiler type."
    exit 1
fi
if [ "${platform}" != "x86_64" ] && [ "${platform}" != "aarch64" ]; then
    echo "Error: Invalid platform type."
    exit 1
fi
mkdir -p ${work_space}/mxManufacture/${frame_project}
cp ${work_space}/mxBase/output/mxBase-${frame_project}.tar.gz ${work_space}/mxManufacture/${frame_project}
cp ${work_space}/mxPlugins/output/mxPlugins-${frame_project}.tar.gz ${work_space}/mxManufacture/${frame_project}
cp ${work_space}/mxStream/output/mxStream-${frame_project}.tar.gz ${work_space}/mxManufacture/${frame_project}
cp ${work_space}/mxTools/output/mxTools-${frame_project}.tar.gz ${work_space}/mxManufacture/${frame_project}
cp ${work_space}/opensource/output/opensource-${frame_project}.tar.gz ${work_space}/mxManufacture/${frame_project}
mkdir -p ${work_space}/mxVision/${frame_project}
mkdir -p ${work_space}/mxVision/${frame_project}/opensource
tar -xvf ${work_space}/mxManufacture/${frame_project}/mxBase-${frame_project}.tar.gz -C ${work_space}/mxVision/${frame_project} --strip-components 1
tar -xvf ${work_space}/mxManufacture/${frame_project}/mxTools-${frame_project}.tar.gz -C ${work_space}/mxVision/${frame_project} --strip-components 1
tar -xvf ${work_space}/mxManufacture/${frame_project}/mxStream-${frame_project}.tar.gz -C ${work_space}/mxVision/${frame_project} --strip-components 1
tar -xvf ${work_space}/mxManufacture/${frame_project}/mxPlugins-${frame_project}.tar.gz -C ${work_space}/mxVision/${frame_project} --strip-components 1
tar -xvf ${work_space}/mxManufacture/${frame_project}/opensource-${frame_project}.tar.gz -C ${work_space}/mxVision/${frame_project}/opensource --strip-components 1
rm -rf ${work_space}/mxVision/${frame_project}/opensource/include/openssl
mv ${work_space}/mxVision/${frame_project}/opensource/include/openssl_reduced ${work_space}/mxVision/${frame_project}/opensource/include/openssl
mkdir -p ${work_space}/mxVision/mindx-${frame_project}/python_wheel
cp -rf ${work_space}/mxVision/${frame_project}/mindx ${work_space}/mxVision/mindx-${frame_project}/python_wheel
cp -rf ${work_space}/mxVision/${frame_project}/python/* ${work_space}/mxVision/mindx-${frame_project}/python_wheel/mindx/sdk/
rm -rf ${work_space}/mxVision/${frame_project}/mindx ${work_space}/mxVision/${frame_project}/opensource-*.tar.gz ${work_space}/mxVision/${frame_project}/samples/mxManufacture ${work_space}/mxVision/${frame_project}/python/*stream*.so ${work_space}/mxVision/mindx-${frame_project}/mindx/sdk/postprocess.cpython-*-linux-gnu.so
cd ${work_space}/mxVision/${frame_project}
cat ${work_space}/mxBase/build/deletefile.txt | while read line; do if [ -f $line -o -d $line ]; then rm -rf $line; else echo "[info] $line * is not exit"; fi; done
cd ${work_space}/mxVision/mindx-${frame_project}/python_wheel
python3.11 ${work_space}/mxBase/build/setup.py bdist_wheel
cp -rf ${work_space}/mxVision/mindx-${frame_project}/python_wheel/dist/* ${work_space}/mxVision/${frame_project}/python/
cd ${work_space}/mxVision/${frame_project}
cp ${work_space}mxBase/build/set_env.sh ./
cd ${work_space}/mxVision/${frame_project}
echo "MindX SDK mxVision:${version_num}" >> version.info
echo "mxVision version:${version_num}" >> version.info
echo "Plat: linux ${platform}" >> version.info
cd ${work_space}/mxVision/${frame_project}
if [ -f filelist.txt ]; then
    rm -rf filelist.txt
fi
find * | sort > filelist.txt
sed -i '/filelist.txt/d' filelist.txt
find . -type d -exec chmod 750 {} +
find . -type f -exec chmod 640 {} +
find . -type f ! -name '*.*' -exec chmod 500 {} +
find . -type f -name '*.py' -exec chmod 550 {} +
find . -type f -name '*.sh' -exec chmod 500 {} +
find . -type f -name '*.h' -exec chmod 550 {} +
rm -rf ${work_space}/mxVision/${frame_project}/build
rm -rf ${work_space}/mxVision/${frame_project}/dist
cp ${work_space}/mxBase/build/vision-install.sh ${work_space}/mxVision/${frame_project}
chmod +x ${work_space}/mxVision/${frame_project}/vision-install.sh
cd ${work_space}/mxVision/${frame_project}
PATCH="${work_space}/opensource/opensource/makeself_patch/makeself-2.5.0.patch"
if [ -f "$PATCH" ]; then
    patch -p1 -d ${work_space}/opensource/opensource/makeself/ < $PATCH
    if [ $? -ne 0 ]; then
        echo "Apply makeself patch $PATCH failed."
        exit 254
    fi
else
    echo "Cannot find makeself patch: $PATCH"
    exit 254
    fi
bash ${work_space}/opensource/opensource/makeself/makeself.sh --header ${work_space}/opensource/opensource/makeself/makeself-header.sh --help-header ${work_space}/mxBase/build/vision-help.info --threads 8 --xz --complevel 4 --tar-extra '--owner=root --group=root' --packaging-date '' --nomd5 --nocrc --sha256 --chown ${work_space}/mxVision/${frame_project} Ascend-mindxsdk-mxvision_${version_num}_linux-${platform}.run 'ASCEND MINDXSDK RUN PACKAGE' ./vision-install.sh
if [ ! -d ${work_space}/output/Software/mxVision ]; then mkdir -p ${work_space}/output/Software/mxVision; fi
mv ${work_space}/mxVision/${frame_project}/Ascend-mindxsdk-mxvision_${version_num}_linux-${platform}.run ${work_space}/output/Software/mxVision
echo "==============Packing successfully=============="
exit 0