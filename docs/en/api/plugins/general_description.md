# Plugin Reference

## Overview

Plugins implemented in Vision SDK inherit from [MxPluginBase](../cpp/process_orchestration.md#mxpluginbase). Plugins start with `mxpi_` and include three public properties, `status`, `deviceId`, and `dataSource`, as well as the native GStreamer properties `parent` and `name`. The details are shown in [Table 1](#table350211521917).

**Table 1** Plugin properties<a id="table350211521917"></a>

|Property|Description|
|--|--|
|status|This property specifies whether the plugin runs synchronously or asynchronously. The type is `int`. The value range is [0, 1], where 0 indicates asynchronous mode and 1 indicates synchronous mode. The default value is 0, which means asynchronous execution. Set it based on actual conditions.|
|deviceId|This property specifies the chip on which the plugin runs. The type is `int`. The default value is the same as the `deviceId` at the beginning of the pipeline configuration file. Set it based on actual conditions.|
|dataSource|This property specifies the source of the data that the plugin processes. The type is `string`. The default value is `auto`. You can specify the name of an upstream plugin as the data source. Set it based on actual conditions.|
|parent|Native GStreamer plugin property that indicates the parent object of the element.|
|name|Native GStreamer plugin property that indicates the name of the element.|

> [!NOTE]
>
>- You can check plugin properties with `gst-inspect-1.0`. Use the inspection results as the source of truth.
>- This document lists only some native GStreamer plugin properties. For more details, see the GStreamer open-source code.

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products
