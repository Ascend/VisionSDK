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
opensourceName="openssl"
requiredFileList="requiredOpensslHeader.txt"
project=$1

work_space="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)"
fullFilePath="${work_space}/${requiredFileList}"
dstDir="${opensourceName}_reduced"


while read -r line; do
    srcFile="${work_space}/../output/${project}/opensource/include/${opensourceName}/${line}"
    copiedDir=$(dirname $(realpath ${srcFile}))
    fullDstBoostDir=$(echo $copiedDir | sed 's/\'${opensourceName}'/'${dstDir}'/g')
    echo "fullDst openssl dir:" ${fullDstBoostDir}
    if [ ! -d ${fullDstBoostDir} ]; then
        mkdir -p ${fullDstBoostDir}
    fi
    cp ${srcFile} ${fullDstBoostDir}

done < ${fullFilePath}