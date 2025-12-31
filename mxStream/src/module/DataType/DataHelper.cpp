/*
* -------------------------------------------------------------------------
*  This file is part of the Vision SDK project.
* Copyright (c) 2025 Huawei Technologies Co.,Ltd.
*
* Vision SDK is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*
*           http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
* -------------------------------------------------------------------------
 * Description: data structure helper.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxStream/DataType/DataHelper.h"

using namespace MxBase;

namespace MxStream {
MxstBufferInput DataHelper::ReadImage(const std::string& imagePath)
{
    MxstBufferInput bufferInput;
    std::string fileContent = MxBase::FileUtils::ReadFileContent(imagePath);
    if (fileContent.empty()) {
        return bufferInput;
    }
    MemoryData memoryData(fileContent.size(), MemoryData::MEMORY_HOST_MALLOC);
    APP_ERROR ret = MemoryHelper::MxbsMalloc(memoryData);
    if (ret != APP_ERR_OK) {
        LogError << "Allocate memory failed. dataSize: " << fileContent.size() << "."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return bufferInput;
    }
    bufferInput.dataSize = static_cast<int>(memoryData.size);
    bufferInput.dataPtr = static_cast<uint32_t *>(memoryData.ptrData);
    std::copy(fileContent.begin(), fileContent.end(), reinterpret_cast<char *>(bufferInput.dataPtr));
    return bufferInput;
}

std::string DataHelper::ReadFile(const std::string& filePath)
{
    return FileUtils::ReadFileContent(filePath);
}
}