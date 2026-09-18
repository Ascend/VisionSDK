# Initialization and Deinitialization<a name="ZH-CN_TOPIC_0000001813200880"></a>

## Overview<a name="ZH-CN_TOPIC_0000001883563682"></a>

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

<term>Atlas 800I A2 inference products</term>

## `MxInit`<a name="ZH-CN_TOPIC_0000001813360396"></a>

**Function<a name="section169698281559"></a>**

Performs global initialization and allocates device resources and log resources. After all Vision SDK APIs finish running, you must call the `MxDeInit()` API to deinitialize the initialized global resources. For details about the usage process, see [Initialization and Deinitialization](../../user_guide.md#initialization-and-deinitialization).

- For <term>Atlas 200I/500 A2 inference products</term>: use `MxInit()`.
- For <term>Atlas inference products</term>: you can use all function prototypes of `MxInit`. After calling `MxInit()`, the first calls to the image processing interfaces of the ImageProcessor class and some interfaces of TensorOperations initialize the DVPP resource pool. Subsequent calls do not initialize the pool again but directly use the DVPP channel resource pool. You can set the DVPP resource pool size through `globalCfg` or `globalCfgExtra`. The first calls to some ImageProcessor and TensorOperations interfaces include the DVPP resource pool initialization time, which increases the overall interface call time.
- For Atlas 800I A2 inference servers: you can use all function prototypes of `MxInit`. After calling `MxInit()`, you can directly use the DVPP channel resource pool. You can set the DVPP channel resource pool size through `globalCfg` or `globalCfgExtra`.

> [!NOTE]
>
>- You can call the `MxInit` interface only once in a process.
>- The underlying channel resources applied for by the DVPP resource pool are shared among processes. In multi-process scenarios, resources may be exhausted. Use this feature with caution.
>- The DVPP resource pool shares underlying channel resources with VideoEncoder and VideoDecoder. In extreme scenarios, resources may be exhausted. You are advised to initialize the resource pool size based on the actual scenario requirements.
>- This interface registers threads that monitor the number of logs and the log time. The thread names are `log_filenum` and `log_time`.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR MxInit();
```

```cpp
APP_ERROR MxInit(const AppGlobalCfg &globalCfg);
```

```cpp
APP_ERROR MxInit(const AppGlobalCfgExtra &globalCfgExtra);
```

**Parameters<a name="section2853321918"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|globalCfg|Input|[AppGlobalCfg](./data_structures_and_enumeration_types.md#appglobalcfg) type, global application configuration|
|globalCfgExtra|Input|[AppGlobalCfgExtra](./data_structures_and_enumeration_types.md#appglobalcfgextra) type, global application configuration|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

## `MxInitFromConfig`<a name="ZH-CN_TOPIC_0000001860121225"></a>

**Function<a name="section169698281559"></a>**

Performs global initialization, allocates device resources and log resources, and preloads operators through the input preload configuration JSON file. The configuration file writing rules are as follows. For a complete configuration file example, see [Example of Initializing the Operator Preload File](../../appendix.md#example-of-an-initialization-operator-preload-file). After all Vision SDK APIs finish running, you must call the `MxDeInit()` API to deinitialize the initialized global resources. For details about the usage process, see [Initialization and Deinitialization](../../user_guide.md#initialization-and-deinitialization).

This interface does not support <term>Atlas 800I A2 inference products</term>.

- The preloaded operator configuration must include the following fields.
    - Operator name (`name`)
    - Operator preload list (`preload_list`)
    - Operator type (`type`), a reserved field. You are advised not to use it for now.

- The preload list must include the following fields.
    - Operator input shape (`input_shape`)
    - Input type (`input_type`)
    - Output shape (`output_shape`)
    - Output type (`output_type`)

        If attributes need to be set during operator preloading, the following fields must also be included.

        - Attribute name (`attr_name`)
        - Attribute data type (`attr_type`)
        - Attribute value (`attr_val`)

- For the restrictions on the input and output shapes and data types of each operator, see the corresponding operator restrictions in [TensorOperations](./media_data_processing.md#tensoroperations). The number of shapes of each tensor must be the same as the number of corresponding types, and they are separated by semicolons. The dimensions in each tensor are separated by commas.
- After calling `MxInitFromConfig`, the first calls to the image processing interfaces of the ImageProcessor class and some interfaces of TensorOperations initialize the DVPP resource pool. Subsequent calls do not initialize the pool again but directly use the DVPP channel resource pool. You can set the DVPP resource pool size through `globalCfg` or `globalCfgExtra`. The first calls to some ImageProcessor and TensorOperations interfaces include the DVPP resource pool initialization time, which increases the overall interface call time.

> [!NOTE]
>
>- You can call the `MxInitFromConfig` interface only once in a process.
>- The underlying channel resources applied for by the DVPP resource pool are shared among processes. In multi-process scenarios, resources may be exhausted. Use this feature with caution.
>- The DVPP resource pool shares underlying channel resources with VideoEncoder and VideoDecoder. In extreme scenarios, resources may be exhausted. You are advised to initialize the resource pool size based on the actual scenario requirements.
>- This interface registers threads that monitor the number of logs and the log time. The thread names are `log_filenum` and `log_time`.

**Function Prototype<a name="section1235164015518"></a>**

```cpp
APP_ERROR MxInitFromConfig(const std::string &configFile);
```

```cpp
APP_ERROR MxInitFromConfig(const std::string &configFile, const AppGlobalCfg &globalCfg);
```

```cpp
APP_ERROR MxInitFromConfig(const std::string &configFile, const AppGlobalCfgExtra &globalCfgExtra);
```

**Parameters<a name="section2853321918"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|configFile|Input|`std::string` type, the path of the preload configuration JSON file. The JSON file must be owned by the current user, and its permissions must not exceed 640.|
|globalCfg|Input|[AppGlobalCfg](./data_structures_and_enumeration_types.md#appglobalcfg) type, global application configuration.|
|globalCfgExtra|Input|[AppGlobalCfgExtra](./data_structures_and_enumeration_types.md#appglobalcfgextra) type, global application configuration.|

**Returns<a name="section819710191484"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

## `MxDeInit`<a name="ZH-CN_TOPIC_0000001813361028"></a>

**Function<a name="section1432565411361"></a>**

Performs global deinitialization and releases resources related to logs and devices. You must use this interface together with the global initialization interface ([MxInit()](#mxinit) or [MxInitFromConfig()](#mxinitfromconfig)). For details about the usage process, see [Initialization and Deinitialization](../../user_guide.md#initialization-and-deinitialization).

> [!NOTE]
>
>- The classes that involve applying for device-side resources, such as [Tensor](./media_data_processing.md#ZH-CN_TOPIC_0000001860000645), [Model](./model_inference.md#ZH-CN_TOPIC_0000001860000893), [Image](./media_data_processing.md#ZH-CN_TOPIC_0000001860001341), [ImageProcessor](./media_data_processing.md#ZH-CN_TOPIC_0000001813201028), [VideoEncoder](./media_data_processing.md#ZH-CN_TOPIC_0000001860001181), and [VideoDecoder](./media_data_processing.md#ZH-CN_TOPIC_0000001860000857), conflict with the scope of `MxDeInit`. Therefore, the scope of these classes cannot be greater than or equal to the scope of `MxDeInit`.
>- After `MxDeInit()` is executed, the log level becomes invalid and logs can no longer be written to the drive.

**Function Prototype<a name="section15011354173612"></a>**

```cpp
APP_ERROR MxDeInit();
```

**Returns<a name="section2693254123618"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|
