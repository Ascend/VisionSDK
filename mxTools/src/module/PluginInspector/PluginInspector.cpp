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
 * Description: Collect information about plug-ins available in the environment.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginInspector/PluginInspector.h"

#include <sys/stat.h>
#include <gst/gst.h>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <cstdarg>
#include <vector>
#include <iomanip>
#include <utility>
#include <dvpp/securec.h>
#include "nlohmann/json.hpp"

// "g_param_spec_types[18]" represents array type, wich holds an array of values
#define G_PARAM_SPEC_ARRAY(pspec) (G_TYPE_CHECK_INSTANCE_CAST ((pspec), g_param_spec_types[18], GParamSpecValueArray))

namespace {
using namespace MxBase;
using ustring = std::string;
const ustring EL_NAME = "name";
const ustring EL_FACTORY = "Factory Details";
const ustring EL_FACTORY_RANK = "Rank";
const ustring EL_FACTORY_GRAPH_AGENT = "GraphAgentElement";
const ustring EL_FACTORY_CLASSIFICATION = "Classification";
const ustring EL_FACTORY_KLASS = "Klass";
const ustring EL_PLUGIN = "Plugin Details";
const ustring EL_PLUGIN_NAME = "Name";
const ustring EL_PLUGIN_DESCRIPTION = "Description";
const ustring EL_PLUGIN_FILENAME = "Filename";
const ustring EL_PLUGIN_VERSION = "Version";
const ustring EL_PLUGIN_LICSENCE = "License";
const ustring EL_PLUGIN_SRCMODULE = "Source module";
const ustring EL_PLUGIN_SRCRELEASEDATA = "Source release date";
const ustring EL_PLUGIN_BINPACKAGE = "Binary package";
const ustring EL_PLUGIN_OURL = "Origin URL";
const ustring EL_PADTEMPLATES = "Pad Templates";
const ustring EL_PADTEMPLATES_SINK = "SINK template";
const ustring EL_PADTEMPLATES_SRC = "SRC template";
const ustring EL_PADTEMPLATES_NAME = "padname";
const ustring EL_PADTEMPLATES_AVAIL = "Availability";
const ustring EL_PADTEMPLATES_CAP = "Capabilities";
const ustring EL_PAD = "Pads";
const ustring EL_PAD_NAME = "name";
const ustring EL_PAD_SRC = "SRC";
const ustring EL_PAD_SINK = "SINK";
const ustring EL_PAD_TEMPLATE = "Pad Template";
const ustring EL_PROPS = "Element Properties";
const ustring EL_PROPS_BLURB = "blurb";
const ustring EL_PROPS_FLAGS = "flags";
const ustring EL_PROPS_VALUE_TYPE = "type";
const ustring EL_PROPS_VALUE_DEFAULT = "default";
const ustring EL_PROPS_VALUE_MAX = "range_max";
const ustring EL_PROPS_VALUE_MIN = "range_min";
const ustring EL_PROPS_VALUE_LIST = "choice";
std::map<ustring, int> supportedPlugins;
const int MAXIMUM_NUMBER_PLUGINS = 500;
const int NUM_RANK_LEVELS = 4;
const int LEN_RANK_NAME = 50;
const int MAX_FILE_LINES = 100000;
std::map<ustring, ustring> pluginKlassMap;

const std::vector<std::string> NOT_SHOW_PLUGINS = {
    "rtpbin", "rtpjitterbuffer", "rtpptdemux", "rtpsession", "rtprtxqueue", "rtprtxreceive", "rtprtxsend",
    "rtpssrcdemux", "rtpmux", "rtpdtmfmux", "rtpfunnel", "rtpac3depay", "rtpac3pay", "rtpbvdepay", "rtpbvpay",
    "rtpceltdepay", "rtpceltpay", "rtpdvdepay", "rtpdvpay", "rtpgstdepay", "rtpgstpay", "rtpilbcpay", "rtpilbcdepay",
    "rtpg722depay", "rtpg722pay", "rtpg723depay", "rtpg723pay", "rtpg726depay", "rtpg726pay", "rtpg729depay",
    "rtpg729pay", "rtpgsmdepay", "rtpgsmpay", "rtpamrdepay", "rtpamrpay", "rtppcmadepay", "rtppcmudepay",
    "rtppcmupay", "rtppcmapay", "rtpmpadepay", "rtpmpapay", "rtpmparobustdepay", "rtpmpvdepay", "rtpmpvpay",
    "rtpopusdepay", "rtpopuspay", "rtph261pay", "rtph261depay", "rtph263ppay", "rtph263pdepay", "rtph263depay",
    "rtph263pay", "rtph264depay", "rtph264pay", "rtph265depay", "rtph265pay", "rtpj2kdepay", "rtpj2kpay",
    "rtpjpegdepay", "rtpjpegpay", "rtpklvdepay", "rtpklvpay", "rtpL8pay", "rtpL8depay", "rtpL16pay", "rtpL16depay",
    "rtpL24pay", "rtpL24depay", "asteriskh263", "rtpmp1sdepay", "rtpmp2tdepay", "rtpmp2tpay", "rtpmp4vpay",
    "rtpmp4vdepay", "rtpmp4apay", "rtpmp4adepay", "rtpmp4gdepay", "rtpmp4gpay", "rtpqcelpdepay", "rtpqdm2depay",
    "rtpsbcdepay", "rtpsbcpay", "rtpsirenpay", "rtpsirendepay", "rtpspeexpay", "rtpspeexdepay", "rtpsv3vdepay",
    "rtptheoradepay", "rtptheorapay", "rtpvorbisdepay", "rtpvorbispay", "rtpvp8depay", "rtpvp8pay", "rtpvp9depay",
    "rtpvp9pay", "rtpvrawdepay", "rtpvrawpay", "rtpstreampay", "rtpstreamdepay", "rtpredenc", "rtpreddec",
    "rtpulpfecdec", "rtpulpfecenc", "rtpstorage", "rtspsrc", "rtpsrc", "rtpsink", "rtpdec", "h263parse", "h264parse",
    "diracparse", "mpegvideoparse", "mpeg4videoparse", "pngparse", "jpeg2000parse", "h265parse", "vc1parse", "shmsrc",
    "shmsink", "capsfilter", "concat", "dataurisrc", "downloadbuffer", "fakesrc",
    "fdsrc", "fdsink", "filesrc", "funnel", "identity", "input-selector", "output-selector",
    "queue2", "typefind", "multiqueue", "valve", "streamiddemux", "v4l2src", "v4l2sink",
    "srtpenc", "srtpdec"
};

const std::map<std::string, std::vector<std::string>> GSTREAMER_SHOW_PROPS = {
    {"test_appsrc", {"name", "blocksize"}},
    {"test_appsink", {"name", "blocksize"}},
    {"test_queue", {"name", "max-size-buffers"}},
    {"test_fakesink", {"name"}},
    {"test_tee", {"name"}},
    {"test_filesink", {"name", "append", "buffer-mode", "buffer-size", "location", "max-transient-error-timeout",
                       "o-sync"}},
};

std::vector<std::pair<ustring, ustring>> klassNamePairs {
    std::pair<ustring, ustring>("infer", "Inference"),
    std::pair<ustring, ustring>("Infer", "Inference"),
    std::pair<ustring, ustring>("Codec/Encoder", "Encoder"),
    std::pair<ustring, ustring>("Codec/Decoder", "Decoder"),
    std::pair<ustring, ustring>("parse", "Parser"),
    std::pair<ustring, ustring>("Parse", "Parser"),
    std::pair<ustring, ustring>("demux", "Demux"),
    std::pair<ustring, ustring>("Demux", "Demux"),
    std::pair<ustring, ustring>("mux", "Mux"),
    std::pair<ustring, ustring>("Mux", "Mux")
};

const std::vector<std::string> SCREEN_DISPLAY_PLUGINS = {
    "mxpi_opencvosd", "mxpi_object2osdinstances", "mxpi_class2osdinstances", "mxpi_osdinstancemerger",
    "mxpi_channelselector", "mxpi_channelimagesstitcher", "mxpi_channelosdcoordsconverter", "mxpi_bufferstablizer",
};

const std::vector<std::string> DEBUG_PLUGINS = {
    "mxpi_dumpdata", "mxpi_loaddata",
};

const std::vector<std::string> INTELLIGENT_VIDEO_PLUGINS = {
    "mxpi_motsimplesort", "mxpi_motsimplesortV2", "mxpi_facealignment", "mxpi_qualitydetection",
};

const std::vector<std::string> MODEL_PROCESS_PLUGINS = {
    "mxpi_objectpostprocessor", "mxpi_classpostprocessor", "mxpi_semanticsegpostprocessor",
    "mxpi_textgenerationpostprocessor", "mxpi_keypointpostprocessor",
};

const std::vector<std::string> MODEL_PLUGINS = {
    "mxpi_modelinfer", "mxpi_tensorinfer",
};

const std::vector<std::string> MEDIA_DATA_HANDLE_PLUGINS = {
    "mxpi_imagedecoder", "mxpi_imageresize", "mxpi_imagecrop", "mxpi_videodecoder", "mxpi_videoencoder",
    "mxpi_imagedecoder", "mxpi_imageencoder", "mxpi_imagenormalize", "mxpi_opencvcentercrop", "mxpi_warpperspective",
    "mxpi_rotation",
};

const std::vector<std::string> STREAMING_PLUGINS = {
    "mxpi_parallel2serial", "mxpi_distributor", "mxpi_synchronize", "queue", "tee", "mxpi_datatransfer",
    "mxpi_nmsoverlapedroi", "mxpi_nmsoverlapedroiv2", "mxpi_roigenerator", "mxpi_semanticsegstitcher",
    "mxpi_objectselector", "mxpi_skipframe",
};

const std::vector<std::string> OUTPUT_PLUGINS = {
    "mxpi_dataserialize", "appsink", "fakesink", "filesink",
};

const std::vector<std::string> INPUT_PLUGINS = {
    "appsrc", "mxpi_rtspsrc",
};

const std::map<std::string, std::vector<std::string>> PLUGINS_CLASSIFICATION = {
    {"Input", INPUT_PLUGINS},
    {"Output", OUTPUT_PLUGINS},
    {"Streaming", STREAMING_PLUGINS},
    {"Media Data Process", MEDIA_DATA_HANDLE_PLUGINS},
    {"Model", MODEL_PLUGINS},
    {"Model Postprocess", MODEL_PROCESS_PLUGINS},
    {"Intelligent Video", INTELLIGENT_VIDEO_PLUGINS},
    {"Debug", DEBUG_PLUGINS},
    {"Screen Display", SCREEN_DISPLAY_PLUGINS},
};

ustring GetFactoryClassification(GstElementFactory *factory, GstElement *element, ustring& klass)
{
    ustring factoryName(GST_OBJECT_NAME(factory));

    for (auto iter = PLUGINS_CLASSIFICATION.begin(); iter != PLUGINS_CLASSIFICATION.end(); ++iter) {
        if (std::find(iter->second.begin(), iter->second.end(), factoryName) != iter->second.end()) {
            return iter->first;
        }
    }

    if (pluginKlassMap.find(factoryName) != pluginKlassMap.end()) {
        return pluginKlassMap[factoryName];
    }
    if (GST_OBJECT_FLAG_IS_SET(element, GST_ELEMENT_FLAG_SOURCE)) {
        return "Input";
    }
    if (GST_OBJECT_FLAG_IS_SET(element, GST_ELEMENT_FLAG_SINK)) {
        return "Output";
    }

    for (auto pair : klassNamePairs) {
        if (klass.find(pair.first) != ustring::npos) {
            return pair.second;
        }
    }
    return "Other";
}

inline int UpdateBestRank(int rank, int currentRank, int bestRank, int idx, int bestIdx)
{
    bestIdx = (abs(rank - currentRank) < abs(rank - bestRank)) ? idx : bestIdx;
    return bestIdx;
}

void GetRankName(char s[], const int size, gint rank)
{
    static const int ranks[NUM_RANK_LEVELS] = {
        GST_RANK_NONE, GST_RANK_MARGINAL, GST_RANK_SECONDARY, GST_RANK_PRIMARY
    };
    static const char *rankNames[NUM_RANK_LEVELS] = {"none", "marginal", "secondary", "primary"};

    int len = 0;
    int bestIdx = 0;
    for (int i = 0; i < NUM_RANK_LEVELS; i++) {
        if (rank == ranks[i]) {
            len = sprintf_s(s, size, "%s (%d)", rankNames[i], rank);
            if (rank != len) {
                std::cout << "[WARNING] " << "PluginInspector.cpp" << ":" << __LINE__
                          << "use sprintf_s secure function write " << len
                          << " characters, but expect write " << rank << " characters. " << std::endl;
            }
            return;
        }
        bestIdx = UpdateBestRank(rank, ranks[i], ranks[bestIdx], i, bestIdx);
    }

    len = sprintf_s(s, size, "%s %c %d (%d)", rankNames[bestIdx],
                    (rank - ranks[bestIdx] > 0) ? '+' : '-', abs(ranks[bestIdx] - rank), rank);
    if (rank != len) {
        std::cout << "[WARNING] " << "PluginInspector.cpp" << ":" << __LINE__
                  << " use sprintf_s secure function write " << len
                  << " characters, but expect write " << rank << " characters. " << std::endl;
    }
    return;
}

nlohmann::json GetFactoryInfoJson(GstElementFactory *factory, GstElement *element)
{
    nlohmann::json jsonFactory = nlohmann::json::object();
    gchar **keys, **k;
    guint rank;
    char rankName[LEN_RANK_NAME];

    if (memset_s(rankName, sizeof(rankName), 0, sizeof(rankName)) != EOK) {
        LogError << "Call memset_s failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return {};
    }
    rank = gst_plugin_feature_get_rank(GST_PLUGIN_FEATURE(factory));
    GetRankName(rankName, LEN_RANK_NAME, rank);
    jsonFactory[EL_FACTORY_RANK] = rankName;
    jsonFactory[EL_FACTORY_GRAPH_AGENT] = false;

    keys = gst_element_factory_get_metadata_keys(factory);
    if (keys != nullptr) {
        for (k = keys; *k != nullptr; ++k) {
            const gchar *val;
            gchar *key = *k;
            val = gst_element_factory_get_metadata(factory, key);
            key[0] = g_ascii_toupper(key[0]);
            jsonFactory[key] = val;
        }
        g_strfreev(keys);
    }

    ustring klass;
    if (jsonFactory.find(EL_FACTORY_KLASS) != jsonFactory.end()) {
        klass = jsonFactory[EL_FACTORY_KLASS];
    }

    ustring classfication = GetFactoryClassification(factory, element, klass);
    jsonFactory[EL_FACTORY_CLASSIFICATION] = classfication;

    return jsonFactory;
}

void GetFormattedDate(const gchar *releaseDate, nlohmann::json& jsonPlugin)
{
    if (releaseDate != nullptr) {
        const gchar *tz = "(UTC)";
        gchar *str = nullptr;
        gchar *sep = nullptr;

        /* may be: YYYY-MM-DD or YYYY-MM-DDTHH:MMZ */
        /* YYYY-MM-DDTHH:MMZ => YYYY-MM-DD HH:MM (UTC) */
        str = g_strdup(releaseDate);
        if (str != nullptr) {
            sep = strstr(str, "T");
            if (sep == nullptr) {
                tz = "";
            }
            ustring data = str;
            data.append(tz);
            jsonPlugin[EL_PLUGIN_SRCRELEASEDATA] = data;

            g_free(str);
        }
    }
}

nlohmann::json GetPluginInfoJson(GstElementFactory *factory)
{
    nlohmann::json jsonPlugin = nlohmann::json::object();
    GstPlugin *plugin = nullptr;

    plugin = gst_plugin_feature_get_plugin(GST_PLUGIN_FEATURE(factory));
    if (!plugin) {
        return {};
    }

    const gchar *releaseDate = gst_plugin_get_release_date_string(plugin);
    const gchar *filename = gst_plugin_get_filename(plugin);

    jsonPlugin[EL_PLUGIN_NAME] = gst_plugin_get_name(plugin);
    jsonPlugin[EL_PLUGIN_DESCRIPTION] = gst_plugin_get_description(plugin);
    jsonPlugin[EL_PLUGIN_FILENAME] = (filename != nullptr) ? filename : "(null)";
    jsonPlugin[EL_PLUGIN_VERSION] = gst_plugin_get_version(plugin);
    jsonPlugin[EL_PLUGIN_LICSENCE] = gst_plugin_get_license(plugin);
    jsonPlugin[EL_PLUGIN_SRCMODULE] = gst_plugin_get_source(plugin);

    GetFormattedDate(releaseDate, jsonPlugin);

    jsonPlugin[EL_PLUGIN_BINPACKAGE] = gst_plugin_get_package(plugin);
    jsonPlugin[EL_PLUGIN_OURL] = gst_plugin_get_origin(plugin);

    gst_object_unref(plugin);

    return jsonPlugin;
}

void SetCapsInfoJson(nlohmann::json& result, const ustring& field, const nlohmann::json& structJson)
{
    if (result.find(field) != result.end()) {
        if (!result[field].is_array()) {
            nlohmann::json jsontmp = result[field];
            result[field] = nlohmann::json::array();
            result[field][result[field].size()] = jsontmp;
        }
        result[field][result[field].size()] = structJson;
    } else {
        result[field] = structJson;
    }
}

ustring GetStructureStr(const GstStructure* structure, const GstCapsFeatures* features)
{
    ustring str = gst_structure_get_name(structure);

    if (features && (gst_caps_features_is_any(features) ||
        !gst_caps_features_is_equal(features, GST_CAPS_FEATURES_MEMORY_SYSTEM_MEMORY))) {
        gchar *featureString = gst_caps_features_to_string(features);
        str.append("(");
        str.append(featureString);
        str.append(")");
        g_free(featureString);
    }
    return str;
}

inline bool IsCapsAnyOrEmpty(const GstCaps *caps)
{
    return gst_caps_is_any(caps) || gst_caps_is_empty(caps);
}

inline nlohmann::json CapsInfoAnyOrEmpty(const GstCaps *caps)
{
    if (gst_caps_is_any(caps)) {
        return "ANY";
    }
    return "EMPTY";
}

nlohmann::json GetCapsInfoJson(GstCaps *caps)
{
    nlohmann::json result = nlohmann::json::object();
    guint i;

    if (IsCapsAnyOrEmpty(caps)) {
        return CapsInfoAnyOrEmpty(caps);
    }

    for (i = 0; i < gst_caps_get_size(caps); i++) {
        nlohmann::json structJson = nlohmann::json::object();
        GstStructure *structure = gst_caps_get_structure(caps, i);
        GstCapsFeatures *features = gst_caps_get_features(caps, i);
        ustring str = GetStructureStr(structure, features);
        guint len, j;
        len = (guint)gst_structure_n_fields(structure);
        for (j = 0; j < len; j++) {
            const gchar *filed = gst_structure_nth_field_name(structure, j);
            const GValue *value = gst_structure_get_value(structure, filed);
            gchar *value_str = gst_value_serialize(value);
            if (value_str != nullptr) {
                structJson[filed] = value_str;
                g_free(value_str);
            }
        }
        SetCapsInfoJson(result, str, structJson);
    }

    return result;
}

inline void GatherPadTemplates(nlohmann::json& allPadTemplates,
    const nlohmann::json& srcPadTemplate, const nlohmann::json& sinkPadTemplate)
{
    if (srcPadTemplate.size()) {
        allPadTemplates[EL_PADTEMPLATES_SRC] = srcPadTemplate;
    }

    if (sinkPadTemplate.size()) {
        allPadTemplates[EL_PADTEMPLATES_SINK] = sinkPadTemplate;
    }
}

struct Index {
    size_t srcIdx;
    size_t sinkIdx;
};

inline void AddSubPadTemplate(nlohmann::json& srcPadTemplate,
    nlohmann::json& sinkPadTemplate, const GstStaticPadTemplate* padTemplate,
    struct Index& index, const nlohmann::json& jsonPad)
{
    if (padTemplate->direction == GST_PAD_SRC) {
        srcPadTemplate[index.srcIdx++] = jsonPad;
    } else {
        sinkPadTemplate[index.sinkIdx++] = jsonPad;
    }
}

nlohmann::json GetPadsTemplatesInfoJson(GstElementFactory* factory)
{
    nlohmann::json allPadTemplates = nlohmann::json::object();
    nlohmann::json srcPadTemplate = nlohmann::json::array();
    nlohmann::json sinkPadTemplate = nlohmann::json::array();
    Index index = {0, 0};
    GstStaticPadTemplate* padTemplate = nullptr;

    const GList *pads = gst_element_factory_get_static_pad_templates(factory);
    while (pads) {
        nlohmann::json jsonPad = nlohmann::json::object();
        padTemplate = (GstStaticPadTemplate*)(pads->data);
        pads = g_list_next(pads);

        jsonPad[EL_PADTEMPLATES_NAME] = padTemplate->name_template;

        if (padTemplate->presence == GST_PAD_ALWAYS) {
            jsonPad[EL_PADTEMPLATES_AVAIL] = "Always";
        } else if (padTemplate->presence == GST_PAD_SOMETIMES) {
            jsonPad[EL_PADTEMPLATES_AVAIL] = "Sometimes";
        } else if (padTemplate->presence == GST_PAD_REQUEST) {
            jsonPad[EL_PADTEMPLATES_AVAIL] = "On request";
        } else {
            jsonPad[EL_PADTEMPLATES_AVAIL] = "UNKNOWN";
        }

        if (padTemplate->static_caps.string) {
            GstCaps *caps = gst_static_caps_get(&padTemplate->static_caps);
            jsonPad[EL_PADTEMPLATES_CAP] = GetCapsInfoJson(caps);
            gst_caps_unref(caps);
        }
        AddSubPadTemplate(srcPadTemplate, sinkPadTemplate, padTemplate, index, jsonPad);
    }
    GatherPadTemplates(allPadTemplates, srcPadTemplate, sinkPadTemplate);

    return allPadTemplates;
}

void SetPadTemplates(const GstPad *pad, nlohmann::json& jsonPads, const gchar *name)
{
    if (pad->padtemplate) {
        jsonPads[EL_PAD_TEMPLATE] = pad->padtemplate->name_template;
    } else {
        /* 这里获取到的pad都是always类型的，always类型的pad名称都与template名称一致 */
        jsonPads[EL_PAD_TEMPLATE] = name;
    }
}

void FillPadJsonByType(nlohmann::json& jsonPads, const nlohmann::json& jsonEachPad,
    const ustring& sinkSrcPadName)
{
    if (jsonPads.find(sinkSrcPadName) != jsonPads.end()) {
        if (!jsonPads[sinkSrcPadName].is_array()) {
            nlohmann::json jsontmp = jsonPads[sinkSrcPadName];
            jsonPads[sinkSrcPadName] = nlohmann::json::array();
            jsonPads[sinkSrcPadName][jsonPads[sinkSrcPadName].size()] = jsontmp;
        }
        jsonPads[sinkSrcPadName][jsonPads[sinkSrcPadName].size()] = jsonEachPad;
    } else {
        jsonPads[sinkSrcPadName] = jsonEachPad;
    }
}

void FillPadsJson(GstPad* pad, nlohmann::json& jsonPads, const nlohmann::json& jsonEachPad)
{
    if (gst_pad_get_direction(pad) == GST_PAD_SRC) {
        FillPadJsonByType(jsonPads, jsonEachPad, EL_PAD_SRC);
    } else {
        FillPadJsonByType(jsonPads, jsonEachPad, EL_PAD_SINK);
    }
}

nlohmann::json GetPadsInfoJson(GstElement *element)
{
    nlohmann::json jsonPads = nlohmann::json::object();
    GList *pads = nullptr;
    GstPad *pad = nullptr;

    if (!element->numpads) {
        return {};
    }

    pads = element->pads;
    while (pads) {
        nlohmann::json jsonEachPad = nlohmann::json::object();
        gchar *name = nullptr;

        pad = GST_PAD(pads->data);
        pads = g_list_next(pads);

        name = gst_pad_get_name(pad);
        jsonEachPad[EL_PAD_NAME] = name;
        SetPadTemplates(pad, jsonEachPad, name);
        g_free(name);
        FillPadsJson(pad, jsonPads, jsonEachPad);
    }
    return jsonPads;
}

ustring UtilityStringFormat(const char *fmt, ...)
{
    va_list ap;
    int count = 0;
    int limit = 10;
    unsigned int len1 = 0;
    unsigned int bufSize = 1024;
    while ((++count) <= limit) {
        bufSize = bufSize << 1;
        ustring tmpBuf(bufSize, '\0');
        va_start(ap, fmt);
        len1 = (unsigned int)vsnprintf_s(&tmpBuf[0], bufSize, bufSize - 1, fmt, ap);
        va_end(ap);
        if (len1 < bufSize) {
            break;
        }
    }
    if (count > limit) {
        std::cout << "[ERROR] " << "PluginInspector.cpp" << ":" << __LINE__
                  << " use vsnprintf_s secure function write characters failed. return value: " << len1 << std::endl;
        return ustring();
    }

    ustring buf(len1 + static_cast<unsigned int>(1), '\0');
    va_start(ap, fmt);
    unsigned int len2 = (unsigned int)vsnprintf_s(&buf[0], buf.size(), buf.size() - 1, fmt, ap);
    va_end(ap);
    if (len1 != len2) {
        std::cout << "[WARNING] " << "PluginInspector.cpp" << ":" << __LINE__
                  << " use vsnprintf_s secure function write " << len2
                  << " characters, but expect write " << len1 << " characters. " << std::endl;
    }
    buf.pop_back();
    return buf;
}

void GetJsonFromParamPointer(const GParamSpec *param, nlohmann::json &jsonProp)
{
    if (param->value_type != G_TYPE_POINTER) {
        ustring type = UtilityStringFormat("Pointer of type %s",
                                           g_type_name(param->value_type));
        jsonProp[EL_PROPS_VALUE_TYPE] = type;
    } else {
        jsonProp[EL_PROPS_VALUE_TYPE] = "Pointer";
    }
}

void GetJsonFromValueArray(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    GParamSpecValueArray *pvarray = G_PARAM_SPEC_ARRAY(param);

    if (pvarray->element_spec) {
        ustring type = UtilityStringFormat("Array of GValues of type %s",
                                           g_type_name(pvarray->element_spec->value_type));
        gchar *str = gst_value_serialize(&value);
        jsonProp[EL_PROPS_VALUE_TYPE] = type;
        jsonProp[EL_PROPS_VALUE_DEFAULT] = str;
        g_free(str);
    } else {
        gchar *str = gst_value_serialize(&value);
        jsonProp[EL_PROPS_VALUE_TYPE] = "Array of GValues";
        jsonProp[EL_PROPS_VALUE_DEFAULT] = str;
        g_free(str);
    }
}

void GetJsonFromGstTypeArray(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    GstParamSpecArray *parray = GST_PARAM_SPEC_ARRAY_LIST(param);

    if (parray->element_spec) {
        ustring type = UtilityStringFormat(
            "GstValueArray of GValues of type %s", g_type_name(parray->element_spec->value_type));
        gchar *str = gst_value_serialize(&value);
        jsonProp[EL_PROPS_VALUE_TYPE] = type;
        jsonProp[EL_PROPS_VALUE_DEFAULT] = str;
        g_free(str);
    } else {
        gchar *str = gst_value_serialize(&value);
        jsonProp[EL_PROPS_VALUE_TYPE] = "GstValueArray of GValues";
        jsonProp[EL_PROPS_VALUE_DEFAULT] = str;
        g_free(str);
    }
}

void GetJsonFromParamEnum(const GParamSpec* param, nlohmann::json& jsonProp, const GValue& value)
{
    nlohmann::json jsonVal = nlohmann::json::object();
    GEnumValue* values;
    guint j = 0;
    ustring enumValue;
    GTypeClass* typeClass = (GTypeClass*)g_type_class_ref(param->value_type);
    values = G_ENUM_CLASS(typeClass)->values;
    enumValue = UtilityStringFormat("%d", g_value_get_enum(&value));
    jsonProp[EL_PROPS_VALUE_TYPE] = "Enum";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = enumValue;

    while (values[j].value_name) {
        ustring key = UtilityStringFormat("%d", values[j].value);
        ustring value = UtilityStringFormat("%-16s - %s", values[j].value_nick, values[j].value_name);
        jsonVal[key] = value;
        j++;
    }
    jsonProp[EL_PROPS_VALUE_LIST] = jsonVal;
    g_type_class_unref(typeClass);
}

void GetJsonFromParamFlags(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    nlohmann::json jsonVal = nlohmann::json::object();
    GParamSpecFlags *pflags = G_PARAM_SPEC_FLAGS(param);
    GFlagsValue *vals;

    ustring defaultVal = UtilityStringFormat("0x%08x", g_value_get_flags(&value));
    vals = pflags->flags_class->values;
    jsonProp[EL_PROPS_VALUE_TYPE] = "Flags";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = defaultVal;

    while (vals[0].value_name) {
        ustring key = UtilityStringFormat("0x%08x", vals[0].value);
        ustring value = UtilityStringFormat("%-16s - %s", vals[0].value_nick,
                                            vals[0].value_name);
        jsonVal[key] = value;
        ++vals;
    }
    jsonProp[EL_PROPS_VALUE_LIST] = jsonVal;
}

void PropsValueTypeDefault(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    if (G_IS_PARAM_SPEC_ENUM(param)) {
        GetJsonFromParamEnum(param, jsonProp, value);
    } else if (G_IS_PARAM_SPEC_FLAGS(param)) {
        GetJsonFromParamFlags(param, jsonProp, value);
    } else if (G_IS_PARAM_SPEC_OBJECT(param)) {
        ustring type = UtilityStringFormat("Object of type %s",
                                           g_type_name(param->value_type));
        jsonProp[EL_PROPS_VALUE_TYPE] = type;
    } else if (G_IS_PARAM_SPEC_BOXED(param)) {
        ustring type = UtilityStringFormat("Boxed pointer of type %s",
                                           g_type_name(param->value_type));
        jsonProp[EL_PROPS_VALUE_TYPE] = type;
    } else if (G_IS_PARAM_SPEC_POINTER(param)) {
        GetJsonFromParamPointer(param, jsonProp);
    } else if (param->value_type == g_value_array_get_type()) {
        GetJsonFromValueArray(param, jsonProp, value);
    } else if (GST_IS_PARAM_SPEC_FRACTION(param)) {
        GstParamSpecFraction *pfraction = GST_PARAM_SPEC_FRACTION(param);
        ustring defaultString = UtilityStringFormat(
            "%d/%d", gst_value_get_fraction_numerator(&value), gst_value_get_fraction_denominator(&value)
            );
        ustring minString = UtilityStringFormat("%d/%d", pfraction->min_num,
                                                pfraction->min_den);
        ustring maxString = UtilityStringFormat("%d/%d", pfraction->max_num,
                                                pfraction->max_den);
        jsonProp[EL_PROPS_VALUE_TYPE] = "Fraction";
        jsonProp[EL_PROPS_VALUE_DEFAULT] = defaultString;
        jsonProp[EL_PROPS_VALUE_MAX] = maxString;
        jsonProp[EL_PROPS_VALUE_MIN] = minString;
    } else if (param->value_type == GST_TYPE_ARRAY) {
        GetJsonFromGstTypeArray(param, jsonProp, value);
    } else {
        jsonProp[EL_PROPS_VALUE_TYPE] = "Unknown";
    }
}

void GetJsonFromValueTypeString(nlohmann::json &jsonProp, const GValue &value)
{
    const char *stringVal = g_value_get_string(&value);
    jsonProp[EL_PROPS_VALUE_TYPE] = "String";
    if (stringVal == nullptr) {
        jsonProp[EL_PROPS_VALUE_DEFAULT] = "null";
    } else {
        jsonProp[EL_PROPS_VALUE_DEFAULT] = stringVal;
    }
}

void GetJsonFromValueTypeUlong(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    GParamSpecULong *pulong = G_PARAM_SPEC_ULONG(param);
    jsonProp[EL_PROPS_VALUE_TYPE] = "Unsigned Long";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = UtilityStringFormat("%lu", g_value_get_ulong(&value));
    jsonProp[EL_PROPS_VALUE_MAX] = UtilityStringFormat("%lu", pulong->maximum);
    jsonProp[EL_PROPS_VALUE_MIN] = UtilityStringFormat("%lu", pulong->minimum);
}

void GetJsonFromValueTypeLong(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    GParamSpecLong *plong = G_PARAM_SPEC_LONG(param);
    jsonProp[EL_PROPS_VALUE_TYPE] = "Long";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = UtilityStringFormat("%ld", g_value_get_long(&value));
    jsonProp[EL_PROPS_VALUE_MAX] = UtilityStringFormat("%ld", plong->maximum);
    jsonProp[EL_PROPS_VALUE_MIN] = UtilityStringFormat("%ld", plong->minimum);
}

void GetJsonFromValueTypeUint(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    GParamSpecUInt *puint = G_PARAM_SPEC_UINT(param);
    jsonProp[EL_PROPS_VALUE_TYPE] = "Unsigned Integer";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = UtilityStringFormat("%u", g_value_get_uint(&value));
    jsonProp[EL_PROPS_VALUE_MAX] = UtilityStringFormat("%u", puint->maximum);
    jsonProp[EL_PROPS_VALUE_MIN] = UtilityStringFormat("%u", puint->minimum);
}

void GetJsonFromValueTypeInt(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    GParamSpecInt *pint = G_PARAM_SPEC_INT(param);
    jsonProp[EL_PROPS_VALUE_TYPE] = "Integer";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = UtilityStringFormat("%d", g_value_get_int(&value));
    jsonProp[EL_PROPS_VALUE_MAX] = UtilityStringFormat("%d", pint->maximum);
    jsonProp[EL_PROPS_VALUE_MIN] = UtilityStringFormat("%d", pint->minimum);
}

void GetJsonFromValueTypeUint64(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    GParamSpecUInt64 *puint64 = G_PARAM_SPEC_UINT64(param);
    jsonProp[EL_PROPS_VALUE_TYPE] = "Unsigned Integer64";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = UtilityStringFormat("%llu", g_value_get_uint64(&value));
    jsonProp[EL_PROPS_VALUE_MAX] = UtilityStringFormat("%llu", puint64->maximum);
    jsonProp[EL_PROPS_VALUE_MIN] = UtilityStringFormat("%llu", puint64->minimum);
}

void GetJsonFromValueTyeInt64(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    GParamSpecInt64 *pint64 = G_PARAM_SPEC_INT64(param);
    jsonProp[EL_PROPS_VALUE_TYPE] = "Integer64";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = UtilityStringFormat("%lld", g_value_get_int64(&value));
    jsonProp[EL_PROPS_VALUE_MAX] = UtilityStringFormat("%lld", pint64->maximum);
    jsonProp[EL_PROPS_VALUE_MIN] = UtilityStringFormat("%lld", pint64->minimum);
}

void GetJsonFromValueTypeFloat(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    GParamSpecFloat *pfloat = G_PARAM_SPEC_FLOAT(param);
    jsonProp[EL_PROPS_VALUE_TYPE] = "Float";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = UtilityStringFormat("%g", g_value_get_float(&value));
    jsonProp[EL_PROPS_VALUE_MAX] = UtilityStringFormat("%g", pfloat->maximum);
    jsonProp[EL_PROPS_VALUE_MIN] = UtilityStringFormat("%g", pfloat->minimum);
}

void GetJsonFromValueTypeDouble(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    GParamSpecDouble *pdouble = G_PARAM_SPEC_DOUBLE(param);
    jsonProp[EL_PROPS_VALUE_TYPE] = "Double";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = UtilityStringFormat("%g", g_value_get_double(&value));
    jsonProp[EL_PROPS_VALUE_MAX] = UtilityStringFormat("%g", pdouble->maximum);
    jsonProp[EL_PROPS_VALUE_MIN] = UtilityStringFormat("%g", pdouble->minimum);
}

void GetJsonFromValueTypeBoolean(nlohmann::json &jsonProp, const GValue &value)
{
    gboolean boolVal = g_value_get_boolean(&value);
    jsonProp[EL_PROPS_VALUE_TYPE] = "Boolean";
    jsonProp[EL_PROPS_VALUE_DEFAULT] = boolVal ? "true" : "false";
}

void GetPropJsonFromTypes(const GParamSpec *param, nlohmann::json &jsonProp, const GValue &value)
{
    switch (G_VALUE_TYPE(&value)) {
        case G_TYPE_STRING: {
            GetJsonFromValueTypeString(jsonProp, value);
            break;
        }
        case G_TYPE_BOOLEAN: {
            GetJsonFromValueTypeBoolean(jsonProp, value);
            break;
        }
        case G_TYPE_ULONG: {
            GetJsonFromValueTypeUlong(param, jsonProp, value);
            break;
        }
        case G_TYPE_LONG: {
            GetJsonFromValueTypeLong(param, jsonProp, value);
            break;
        }
        case G_TYPE_UINT: {
            GetJsonFromValueTypeUint(param, jsonProp, value);
            break;
        }
        case G_TYPE_INT: {
            GetJsonFromValueTypeInt(param, jsonProp, value);
            break;
        }
        case G_TYPE_UINT64: {
            GetJsonFromValueTypeUint64(param, jsonProp, value);
            break;
        }
        case G_TYPE_INT64: {
            GetJsonFromValueTyeInt64(param, jsonProp, value);
            break;
        }
        case G_TYPE_FLOAT: {
            GetJsonFromValueTypeFloat(param, jsonProp, value);
            break;
        }
        case G_TYPE_DOUBLE: {
            GetJsonFromValueTypeDouble(param, jsonProp, value);
            break;
        }
        case G_TYPE_CHAR:
        case G_TYPE_UCHAR:
        default:
            PropsValueTypeDefault(param, jsonProp, value);
            break;
    }
}

void SetElementProps(GObject *obj, nlohmann::json &jsonPropsOut, GParamSpec *param, nlohmann::json& jsonProp)
{
    if (obj == nullptr) {
        LogError << "Failed to set element props, object ptr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return;
    }
    std::string elementName = GST_OBJECT_NAME(obj);
    auto iter = GSTREAMER_SHOW_PROPS.find(elementName);
    if (iter != GSTREAMER_SHOW_PROPS.end()) {
        if (std::find(iter->second.begin(), iter->second.end(), g_param_spec_get_name(param)) != iter->second.end()) {
            jsonPropsOut[g_param_spec_get_name(param)] = jsonProp;
        }
        return;
    }
    jsonPropsOut[g_param_spec_get_name(param)] = jsonProp;
}

void GetPropsInfoJsonJsonIteration(GObject *obj, nlohmann::json &jsonPropsOut, gboolean readable,
    gboolean firstFlag, GParamSpec *param)
{
    nlohmann::json jsonProp = nlohmann::json::object();
    ustring flags = "";
    GValue value = {};
    jsonProp[EL_PROPS_BLURB] = g_param_spec_get_blurb(param);
    g_value_init(&value, param->value_type);
    firstFlag = TRUE;
    readable = FALSE;
    readable = !!(param->flags & G_PARAM_READABLE);
    if (readable && obj != nullptr) {
        g_object_get_property(obj, param->name, &value);
    } else {
        g_param_value_set_default(param, &value);
    }

    if (readable) {
        flags.append((firstFlag) ? "readable" : ", readable");
        firstFlag = FALSE;
    }
    if (param->flags & G_PARAM_WRITABLE) {
        flags.append((firstFlag) ? "writable" : ", writable");
        firstFlag = FALSE;
    }
    if (param->flags & G_PARAM_DEPRECATED) {
        flags.append((firstFlag) ? "deprecated" : ", deprecated");
        firstFlag = FALSE;
    }
    if (param->flags & GST_PARAM_CONTROLLABLE) {
        flags.append((firstFlag) ? "controllable" : ", controllable");
        firstFlag = FALSE;
    }
    if (param->flags & GST_PARAM_MUTABLE_PLAYING) {
        flags.append(", changeable in NULL, READY, PAUSED or PLAYING state");
    } else if (param->flags & GST_PARAM_MUTABLE_PAUSED) {
        flags.append(", changeable only in NULL, READY or PAUSED state");
    } else if (param->flags & GST_PARAM_MUTABLE_READY) {
        flags.append(", changeable only in NULL or READY state");
    }
    jsonProp[EL_PROPS_FLAGS] = flags;

    GetPropJsonFromTypes(param, jsonProp, value);

    g_value_reset(&value);
    SetElementProps(obj, jsonPropsOut, param, jsonProp);
}

nlohmann::json GetPropsInfoJson(GObjectClass *obj_class, GObject *obj = nullptr)
{
    nlohmann::json jsonPropsOut = nlohmann::json::object();
    GParamSpec **propertySpecs;
    guint numProperties, i;
    gboolean readable = false;
    gboolean firstFlag = false;

    propertySpecs = g_object_class_list_properties(obj_class, &numProperties);
    for (i = 0; i < numProperties; i++) {
        GParamSpec *param = propertySpecs[i];
        GType owner_type = param->owner_type;

        /* We're printing pad properties */
        if (obj == nullptr && (owner_type == G_TYPE_OBJECT
            || owner_type == GST_TYPE_OBJECT || owner_type == GST_TYPE_PAD)) {
            continue;
        }
        GetPropsInfoJsonJsonIteration(obj, jsonPropsOut, readable, firstFlag, param);
    }

    g_free(propertySpecs);

    return jsonPropsOut;
}

nlohmann::json GetElementAllInfo(GstPluginFeature *feature)
{
    nlohmann::json jsonResult = nlohmann::json::object();
    ustring elementName = "test_";
    nlohmann::json jsonProps = nlohmann::json::array();
    GstElementFactory *factory = nullptr;
    GstElement *element = nullptr;

    factory = GST_ELEMENT_FACTORY(gst_plugin_feature_load(feature));
    if (!factory) {
        LogError << "[GetElementAllInfo] element plugin couldn't be loaded." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return {};
    }

    elementName.append(GST_OBJECT_NAME(feature));
    element = gst_element_factory_create(factory, elementName.c_str());
    if (!element) {
        gst_object_unref(factory);
        LogError << "[GetElementAllInfo] couldn't construct element for some reason."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return {};
    }
    jsonResult[EL_NAME] = GST_OBJECT_NAME(feature);

    /* fatory info */
    jsonResult[EL_FACTORY] = GetFactoryInfoJson(factory, element);
    /* plugin info */
    jsonResult[EL_PLUGIN] = GetPluginInfoJson(factory);

    /* pad template info */
    jsonResult[EL_PADTEMPLATES] = GetPadsTemplatesInfoJson(factory);

    /* pad info */
    jsonResult[EL_PAD] = GetPadsInfoJson(element);

    /* props info */
    jsonResult[EL_PROPS] = GetPropsInfoJson(G_OBJECT_GET_CLASS(G_OBJECT(element)), G_OBJECT(element));
    gst_object_unref(element);
    gst_object_unref(factory);

    return jsonResult;
}

void GetAllElementsInfoJson(GstPluginFeature *feature, nlohmann::json &result, size_t &idx)
{
    std::string factoryName = GST_OBJECT_NAME(feature);
    if (std::find(NOT_SHOW_PLUGINS.begin(), NOT_SHOW_PLUGINS.end(), factoryName) != NOT_SHOW_PLUGINS.end()) {
        return;
    }
    nlohmann::json tmp = GetElementAllInfo(feature);
    if (tmp.empty()) {
        return;
    }
    result[idx++] = tmp;
}

void GetElementListProcess(nlohmann::json &result, size_t &idx)
{
    GList* plugins = nullptr;
    GList* origPlugins = nullptr;

    origPlugins = plugins = gst_registry_get_plugin_list(gst_registry_get());
    while (plugins) {
        LogDebug << "Plugin index: " << idx << " .";
        GList* features = nullptr;
        GList* orig_features = nullptr;
        GstPlugin* plugin = nullptr;

        plugin = (GstPlugin*)(plugins->data);
        plugins = g_list_next(plugins);

        if (GST_OBJECT_FLAG_IS_SET(plugin, GST_PLUGIN_FLAG_BLACKLISTED)) {
            continue;
        }

        orig_features = features = gst_registry_get_feature_list_by_plugin(gst_registry_get(),
                                                                           gst_plugin_get_name(plugin));
        while (features) {
            GstPluginFeature* feature = nullptr;
            GstElementFactory* factory = nullptr;

            if (G_UNLIKELY(features->data == nullptr))
                goto next;
            feature = GST_PLUGIN_FEATURE(features->data);
            if (!GST_IS_ELEMENT_FACTORY(feature)) {
                goto next;
            }
            factory = GST_ELEMENT_FACTORY(feature);
            if (gst_element_factory_get_num_pad_templates(factory) == 0) {
                goto next;
            }
            GetAllElementsInfoJson(feature, result, idx);
            next:
            features = g_list_next(features);
        }

        gst_plugin_feature_list_free(orig_features);
    }

    gst_plugin_list_free(origPlugins);
}

nlohmann::json GetAllElementsInfo(void)
{
    nlohmann::json result = nlohmann::json::array();
    size_t idx = 0;

    GetElementListProcess(result, idx);
    LogDebug << "Total result index:" << idx;
    return result;
}

void PluginInfoMapClear()
{
    pluginKlassMap.clear();
    supportedPlugins.clear();
}
}

namespace MxTools {
PluginInspector::PluginInspector()
{
}

void PluginInspector::ExportAllPluginInfos(std::string outputJsonFile)
{
    LogInfo << "Exporting all plugins...";
    PluginInfoMapClear();

    nlohmann::json result = GetAllElementsInfo();

    // output plugin info to json file.
    std::ofstream outStream(outputJsonFile);
    chmod(outputJsonFile.c_str(), S_IRUSR | S_IWUSR | S_IRGRP);
    outStream << result << std::endl;
    outStream.close();
    PluginInfoMapClear();
    LogInfo << "Finish plugins inspection. Json file was output";
}

PluginInspector::~PluginInspector()
{
}
}