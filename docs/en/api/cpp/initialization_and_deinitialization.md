# Initialization and Deinitialization

## Overview

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

Atlas 800I A2 inference products

## MxInit

**Function Description**

Global initialization allocates Device and log resources. After you finish using all Vision SDK APIs, you must call the `MxDeInit()` API to deinitialize the initialized global resources. For the usage flow, see [Initialization and Deinitialization](../../user_guide.md#initialization-and-deinitialization).

- For Atlas 200I/500 A2 inference products, use `MxInit()`.
- For Atlas inference products, you can use all `MxInit` function prototypes. After you call `MxInit()`, the first call to the image-processing APIs in the `ImageProcessor` class and some `TensorOperations` APIs initializes the DVPP resource pool. Subsequent calls do not initialize it and use the DVPP channel resource pool directly. The DVPP resource pool size can be set through `globalCfg` or `globalCfgExtra`. The first call to `ImageProcessor` and some `TensorOperations` APIs includes the time required to initialize the DVPP resource pool. Therefore, the overall API call time increases.
- For the Atlas 800I A2 inference server, you can use all `MxInit` function prototypes. After you call `MxInit()`, you can use the DVPP channel resource pool directly. The DVPP channel resource pool size can be set through `globalCfg` or `globalCfgExtra`.

> [!NOTE]
>
>- You can call the `MxInit` API only once in a process.
>- The underlying channel resources allocated by the DVPP resource pool are shared across processes. In a multi-process scenario, resources may be exhausted. Therefore, use it with caution.
>- The DVPP resource pool shares underlying channel resources with `VideoEncoder` and `VideoDecoder`. In extreme scenarios, resources may be exhausted. You are advised to initialize the resource pool size according to actual requirements.
>- This API registers threads that monitor the log count and log time, and the threads are named `log_filenum` and `log_time`.

**Function Prototype**

```cpp
APP_ERROR MxInit();
```

```cpp
APP_ERROR MxInit(const AppGlobalCfg &globalCfg);
```

```cpp
APP_ERROR MxInit(const AppGlobalCfgExtra &globalCfgExtra);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|globalCfg|Input|The [AppGlobalCfg](./data_structures_and_enumeration_types.md#appglobalcfg) type. Global application configuration.|
|globalCfgExtra|Input|The [AppGlobalCfgExtra](./data_structures_and_enumeration_types.md#appglobalcfgextra) type. Global application configuration.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

## MxInitFromConfig

**Function Description**

Global initialization allocates Device and log resources, and it preloads operators from the input preload configuration JSON file. The configuration file format is as follows, and a complete example is available in [Example of the Initialization Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file). After you finish using all Vision SDK APIs, you must call the `MxDeInit()` API to deinitialize the initialized global resources. For the usage flow, see [Initialization and Deinitialization](../../user_guide.md#initialization-and-deinitialization).

This API does not support Atlas 800I A2 inference products.

- The preloaded operator configuration must contain the following fields.
    - Operator name (`name`).
    - Operator preload list (`preload_list`).
    - Operator type (`type`), which is a reserved field and is not recommended for now.

- The preload list must contain the following fields.
    - Operator input shape (`input_shape`).
    - Input type (`input_type`).
    - Output shape (`output_shape`).
    - Output type (`output_type`).

        If you need to set attributes during operator preloading, it must also contain the following fields.

        - Attribute name (`attr_name`).
        - Attribute data type (`attr_type`).
        - Attribute value (`attr_val`).

- For the input and output shape and data type restrictions of each operator, see the corresponding restrictions in [TensorOperations](./media_data_processing.md#tensoroperations). The shapes of each Tensor and the corresponding number of types must match, and separate them with semicolons. Use commas to separate the dimensions in each Tensor.
- After you call `MxInitFromConfig`, the first call to the image-processing APIs in the `ImageProcessor` class and some `TensorOperations` APIs initializes the DVPP resource pool. Subsequent calls do not initialize it and use the DVPP channel resource pool directly. The DVPP resource pool size can be set through `globalCfg` or `globalCfgExtra`. The first call to `ImageProcessor` and some `TensorOperations` APIs includes the time required to initialize the DVPP resource pool. Therefore, the overall API call time increases.

> [!NOTE]
>
>- You can call the `MxInitFromConfig` API only once in a process.
>- The underlying channel resources allocated by the DVPP resource pool are shared across processes. In a multi-process scenario, resources may be exhausted. Therefore, use it with caution.
>- The DVPP resource pool shares underlying channel resources with `VideoEncoder` and `VideoDecoder`. In extreme scenarios, resources may be exhausted. You are advised to initialize the resource pool size according to actual requirements.
>- This API registers threads that monitor the log count and log time, and the threads are named `log_filenum` and `log_time`.

**Function Prototype**

```cpp
APP_ERROR MxInitFromConfig(const std::string &configFile);
```

```cpp
APP_ERROR MxInitFromConfig(const std::string &configFile, const AppGlobalCfg &globalCfg);
```

```cpp
APP_ERROR MxInitFromConfig(const std::string &configFile, const AppGlobalCfgExtra &globalCfgExtra);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|configFile|Input|The path of the preload configuration JSON file, which must be owned by the current user and have permissions no greater than 640.|
|globalCfg|Input|The [AppGlobalCfg](./data_structures_and_enumeration_types.md#appglobalcfg) type. Global application configuration.|
|globalCfgExtra|Input|The [AppGlobalCfgExtra](./data_structures_and_enumeration_types.md#appglobalcfgextra) type. Global application configuration.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

## MxDeInit

**Function Description**

Global deinitialization releases Log, Device, and other related resources. Use it together with the global initialization APIs ([MxInit](#mxinit) or [MxInitFromConfig](#mxinitfromconfig)). For the usage flow, see [Initialization and Deinitialization](../../user_guide.md#initialization-and-deinitialization).

> [!NOTE]
>
>- Classes that allocate Device-side resources ([Tensor](./media_data_processing.md#ZH-CN_TOPIC_0000001860000645), [Model](./model_inference.md#ZH-CN_TOPIC_0000001860000893), [Image](./media_data_processing.md#ZH-CN_TOPIC_0000001860001341), [ImageProcessor](./media_data_processing.md#ZH-CN_TOPIC_0000001813201028), [VideoEncoder](./media_data_processing.md#ZH-CN_TOPIC_0000001860001181), and [VideoDecoder](./media_data_processing.md#ZH-CN_TOPIC_0000001860000857)) conflict with the scope of `MxDeInit()`. Therefore, the scope of these classes must be smaller than the scope of `MxDeInit()`.
>- After `MxDeInit()`, the log level no longer takes effect, and logs cannot be written to disk.

**Function Prototype**

```cpp
APP_ERROR MxDeInit();
```

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by the program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|
