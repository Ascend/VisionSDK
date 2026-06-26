# 插件参考<a name="ZH-CN_TOPIC_0000001928269677"></a>

## 总体说明<a name="ZH-CN_TOPIC_0000001888715704"></a>

Vision SDK中实现的插件继承自[MxPluginBase](../cpp/process_orchestration.md#mxpluginbase)。插件以“mxpi\_”开头，包含“status”、“deviceId”、“dataSource”三个公共属性以及GStreamer原生属性“parent”和“name”，具体说明如[表1](#table350211521917)所示。

**表 1**  插件属性说明<a id="table350211521917"></a>

|属性|说明|
|--|--|
|status|该属性指定插件运行方式为同步执行或异步，int类型，取值范围[0, 1]，0为异步，1为同步，默认为0（异步执行），请用户根据实际情况设置。|
|deviceId|该属性指定插件运行的芯片，int类型，默认与pipeline配置文件中开头的deviceId相同，请用户根据实际情况设置。|
|dataSource|该属性指定插件处理数据的来源，string类型，默认为auto，用户可指定上游插件名称作为数据来源，请用户根据实际情况设置。|
|parent|GStreamer插件原生属性，表示元素的父对象。|
|name|GStreamer插件原生属性，表示元素的名称。|

> [!NOTE]
>
>- 插件属性可以通过工具gst-inspect-1.0进行检测，以检测结果为准。
>- 本文只列举了部分GStreamer插件原生属性，更多详细信息请查看GStreamer开源源码。

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>
