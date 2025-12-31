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
 * Description: The DumpDataHelper For Test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXPLUGINS_DUMPDATAHELPER_H
#define MXPLUGINS_DUMPDATAHELPER_H

#include <gtest/gtest.h>
#include "google/protobuf/util/json_util.h"
#include "MxBase/Log/Log.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"

namespace MxPlugins {
class DumpDataHelper {
public:
    static bool CompareDumpData(const std::string& result, const std::string& expect)
    {
        bool ret = false;
        MxTools::MxpiDumpData resultDumpData;
        auto status = google::protobuf::util::JsonStringToMessage(result, &resultDumpData);
        EXPECT_TRUE(status.ok());
        MxTools::MxpiDumpData expectDumpData;
        status = google::protobuf::util::JsonStringToMessage(expect, &expectDumpData);
        EXPECT_TRUE(status.ok());

        for (auto& metaData : expectDumpData.metadata()) {
            MxTools::MetaData expectMetaData;
            MxTools::MetaData resultMetaData;
            auto key = metaData.key();
            if (FindMetaDataFromDumpData(expectDumpData, key, expectMetaData)) {
                EXPECT_TRUE(FindMetaDataFromDumpData(resultDumpData, key, resultMetaData));
                LogInfo << "start to compare key:" << key;
                ret = CompareMetaDataByKey(resultMetaData, expectMetaData, key);
                EXPECT_EQ(ret, true);
            }
        }
        return ret;
    }

    static bool CompareDumpData(const std::string& result, const std::string& expect,
                                const std::vector<std::string> keys)
    {
        bool ret = false;
        MxTools::MxpiDumpData resultDumpData;
        auto status = google::protobuf::util::JsonStringToMessage(result, &resultDumpData);
        EXPECT_TRUE(status.ok());
        MxTools::MxpiDumpData expectDumpData;
        status = google::protobuf::util::JsonStringToMessage(expect, &expectDumpData);
        EXPECT_TRUE(status.ok());

        for (std::string key : keys) {
            MxTools::MetaData expectMetaData;
            MxTools::MetaData resultMetaData;
            if (FindMetaDataFromDumpData(expectDumpData, key, expectMetaData)) {
                EXPECT_TRUE(FindMetaDataFromDumpData(resultDumpData, key, resultMetaData));
                LogInfo << "start to compare key:" << key;
                ret = CompareMetaDataByKey(resultMetaData, expectMetaData, key);
                EXPECT_EQ(ret, true);
            }
        }

        return ret;
    }
private:
    static bool FindMetaDataFromDumpData(const MxTools::MxpiDumpData& mxpiDumpData, const std::string& key,
        MxTools::MetaData& metaData)
    {
        for (auto& value : mxpiDumpData.metadata()) {
            if (value.key() == key) {
                metaData = value;
                return true;
            }
        }
        return false;
    }

    static void CompareMxpiTensorPackageList(const MxTools::MetaData &expectMetaData,
        const MxTools::MetaData &resultMetaData)
    {
        MxTools::MxpiTensorPackageList resultTensorPackageList;
        google::protobuf::util::JsonStringToMessage(resultMetaData.content(), &resultTensorPackageList);
        MxTools::MxpiTensorPackageList expectTensorPackageList;
        google::protobuf::util::JsonStringToMessage(expectMetaData.content(), &expectTensorPackageList);

        for (int i = 0; i < expectTensorPackageList.tensorpackagevec_size(); i++) {
            auto tensorPackageExpect = expectTensorPackageList.tensorpackagevec(i);
            LogInfo << "tensorPackageExpect has member index(" << i << ").";
            auto tensorPackageResult = resultTensorPackageList.tensorpackagevec(i);
            LogInfo << "tensorPackageResult has member index(" << i << ").";
            for (int j = 0; j < tensorPackageExpect.headervec_size(); j++) {
                EXPECT_EQ(tensorPackageExpect.headervec(j).SerializeAsString(),
                          tensorPackageResult.headervec(j).SerializeAsString());
            }
            for (int j = 0; j < tensorPackageExpect.tensorvec_size(); j++) {
                auto tensorExpect = tensorPackageExpect.tensorvec(j);
                tensorExpect.set_tensordataptr(0);
                tensorExpect.set_deviceid(0);
                tensorExpect.set_freefunc(0);
                auto tensorResult = tensorPackageResult.tensorvec(j);
                tensorResult.set_tensordataptr(0);
                tensorResult.set_deviceid(0);
                tensorResult.set_freefunc(0);
                EXPECT_EQ(tensorExpect.SerializeAsString(), tensorResult.SerializeAsString());
            }
        }
    }

    static void CompareMxpiVisionList(const MxTools::MetaData &expectMetaData, const MxTools::MetaData &resultMetaData)
    {
        MxTools::MxpiVisionList resultVisionList;
        google::protobuf::util::JsonStringToMessage(resultMetaData.content(), &resultVisionList);
        MxTools::MxpiVisionList expectVisionList;
        google::protobuf::util::JsonStringToMessage(expectMetaData.content(), &expectVisionList);

        for (int i = 0; i < expectVisionList.visionvec_size(); i++) {
            auto visionExpect = expectVisionList.visionvec(i);
            visionExpect.mutable_visiondata()->set_dataptr(0);
            visionExpect.mutable_visiondata()->set_deviceid(0);
            visionExpect.mutable_visiondata()->set_freefunc(0);
            visionExpect.mutable_visioninfo()->mutable_preprocessinfo()->Clear();
            LogInfo << "expectVisionList has member index(" << i << ").";
            auto visionResult = resultVisionList.visionvec(i);
            visionResult.mutable_visiondata()->set_dataptr(0);
            visionResult.mutable_visiondata()->set_deviceid(0);
            visionResult.mutable_visiondata()->set_freefunc(0);
            visionResult.mutable_visioninfo()->mutable_preprocessinfo()->Clear();
            LogInfo << "resultVisionList has member index(" << i << ").";
            for (int j = 0; j < visionExpect.headervec_size(); j++) {
                EXPECT_EQ(visionExpect.headervec(j).SerializeAsString(),
                          visionResult.headervec(j).SerializeAsString());
            }
            EXPECT_EQ(visionExpect.visioninfo().SerializeAsString(),
                      visionResult.visioninfo().SerializeAsString());
            EXPECT_EQ(visionExpect.visiondata().SerializeAsString(),
                      visionResult.visiondata().SerializeAsString());
        }
    }

    static bool CompareMetaDataByKey(const MxTools::MetaData& expectMetaData, const MxTools::MetaData& resultMetaData,
        const std::string& key)
    {
        std::string result;
        std::string expect;

        if (expectMetaData.protodatatype() == "MxpiVisionList") {
            CompareMxpiVisionList(expectMetaData, resultMetaData);
        } else if (expectMetaData.protodatatype() == "MxpiTensorPackageList") {
            CompareMxpiTensorPackageList(expectMetaData, resultMetaData);
        } else {
            result = resultMetaData.SerializeAsString();
            expect = expectMetaData.SerializeAsString();
        }
        return (result == expect);
    }
};
}
#endif // MXPLUGINS_DUMPDATAHELPER_H