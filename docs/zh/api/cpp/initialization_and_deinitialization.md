# 初始化和去初始化<a name="ZH-CN_TOPIC_0000001813200880"></a>

## 总体说明<a name="ZH-CN_TOPIC_0000001883563682"></a>

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

<term>Atlas 800I A2推理产品</term>

## MxInit<a name="ZH-CN_TOPIC_0000001813360396"></a>

**函数功能<a name="section169698281559"></a>**

全局初始化，申请Device资源与日志资源，Vision SDK接口全部执行完毕后，用户需调用MxDeInit\(\)接口对初始化的全局资源进行去初始化操作。相关使用流程请参考[初始化与去初始化](../../user_guide.md#初始化与去初始化)。

- 对于<term>Atlas 200I/500 A2 推理产品</term>：使用MxInit\(\)。
- 对于<term>Atlas 推理系列产品</term>：可以使用MxInit的全部函数原型。调用MxInit\(\)后，在初次调用ImageProcessor类图像处理接口和TensorOperations的部分接口会初始化DVPP资源池，再次调用接口时不会进行初始化，会直接使用DVPP通道资源池，其中DVPP资源池大小可通过“globalCfg”或“globalCfgExtra”进行设置。第一次调用ImageProcessor和TensorOperations的部分接口时间会包含DVPP资源池初始化的时间，导致接口调用的总体时间增长。
- 对于Atlas 800I A2 推理服务器：可以使用MxInit的全部函数原型。调用MxInit\(\)后，可直接使用DVPP通道资源池，其中DVPP通道资源池大小可通过“globalCfg”或“globalCfgExtra”进行设置。

> [!NOTE]
>
>- 一个进程内只能调用一次MxInit接口。
>- DVPP资源池所申请的底层通道资源为进程共享，多进程场景可能导致资源耗尽，请谨慎使用。
>- DVPP资源池与VideoEncoder及VideoDecoder共享底层通道资源，极端场景可能导致资源耗尽，建议根据实际场景的需要进行资源池大小的初始化。
>- 该接口会注册监测日志数量和监测日志时间的线程，线程命名分别为“log\_filenum”和“log\_time”。

**函数原型<a name="section1235164015518"></a>**

```cpp
APP_ERROR MxInit();
```

```cpp
APP_ERROR MxInit(const AppGlobalCfg &globalCfg);
```

```cpp
APP_ERROR MxInit(const AppGlobalCfgExtra &globalCfgExtra);
```

**参数说明<a name="section2853321918"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|globalCfg|输入|[AppGlobalCfg](./data_structures_and_enumeration_types.md#appglobalcfg)类型，应用全局配置。|
|globalCfgExtra|输入|[AppGlobalCfgExtra](./data_structures_and_enumeration_types.md#appglobalcfgextra)类型，应用全局配置。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

## MxInitFromConfig<a name="ZH-CN_TOPIC_0000001860121225"></a>

**函数功能<a name="section169698281559"></a>**

全局初始化，申请设备资源与日志资源并通过输入的预加载配置JSON文件进行算子预加载操作，配置文件编写规则参考如下，完整配置文件示例请参见[初始化算子预加载文件示例](../../appendix.md#初始化算子预加载文件示例)。Vision SDK接口全部执行完毕后，用户需调用MxDeInit\(\)接口对初始化的全局资源进行去初始化操作。相关使用流程请参考[初始化与去初始化](../../user_guide.md#初始化与去初始化)。

该接口不支持<term>Atlas 800I A2推理产品</term>。

- 预加载的算子配置，需包含以下字段。
    - 算子名称（name）
    - 算子预加载列表（preload\_list）
    - 算子类型（type），预留字段，暂不建议使用。

- 预加载列表中，需包含以下字段。
    - 算子输入形状（input\_shape）
    - 输入类型（input\_type）
    - 输出形状（output\_shape）
    - 输出类型（output\_type）

        若算子预加载时需要设置属性，还需包含以下字段。

        - 属性名称（attr\_name）
        - 属性数据类型（attr\_type）
        - 属性值（attr\_val）

- 各算子输入输出形状及数据类型限制条件参照[TensorOperations](./media_data_processing.md#tensoroperations)中对应算子限制条件，各Tensor的形状与对应的类型数量应保持一致并用分号分割，Tensor中的各个维度用逗号分割。
- 调用MxInitFromConfig后，在初次调用ImageProcessor类图像处理接口和TensorOperations的部分接口会初始化DVPP资源池，再次调用接口时不会进行初始化，会直接使用DVPP通道资源池，其中DVPP资源池大小可通过“globalCfg”或“globalCfgExtra”进行设置。第一次调用ImageProcessor和TensorOperations的部分接口时间会包含DVPP资源池初始化的时间，导致接口调用的总体时间增长。

> [!NOTE]
>
>- 一个进程内只能调用一次MxInitFromConfig接口。
>- DVPP资源池所申请的底层通道资源为进程共享，多进程场景可能导致资源耗尽，请谨慎使用。
>- DVPP资源池与VideoEncoder及VideoDecoder共享底层通道资源，极端场景可能导致资源耗尽，建议根据实际场景的需要进行资源池大小的初始化。
>- 该接口会注册监测日志数量和监测日志时间的线程，线程命名分别为“log\_filenum”和“log\_time”。

**函数原型<a name="section1235164015518"></a>**

```cpp
APP_ERROR MxInitFromConfig(const std::string &configFile);
```

```cpp
APP_ERROR MxInitFromConfig(const std::string &configFile, const AppGlobalCfg &globalCfg);
```

```cpp
APP_ERROR MxInitFromConfig(const std::string &configFile, const AppGlobalCfgExtra &globalCfgExtra);
```

**参数说明<a name="section2853321918"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|configFile|输入|std::string类型，预加载配置JSON文件路径，JSON要求属主为当前用户且权限不大于640。|
|globalCfg|输入|[AppGlobalCfg](./data_structures_and_enumeration_types.md#appglobalcfg)类型，应用全局配置。|
|globalCfgExtra|输入|[AppGlobalCfgExtra](./data_structures_and_enumeration_types.md#appglobalcfgextra)类型，应用全局配置。|

**返回参数说明<a name="section819710191484"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

## MxDeInit<a name="ZH-CN_TOPIC_0000001813361028"></a>

**函数功能<a name="section1432565411361"></a>**

全局去初始化，释放Log、Device等相关资源，需与全局初始化接口（[MxInit](#mxinit)或[MxInitFromConfig](#mxinitfromconfig)）配套使用。相关使用流程请参考[初始化与去初始化](../../user_guide.md#初始化与去初始化)。

> [!NOTE]
>
>- 涉及申请Device侧资源的类（[Tensor](./media_data_processing.md#ZH-CN_TOPIC_0000001860000645)、[Model](./model_inference.md#ZH-CN_TOPIC_0000001860000893)、[Image](./media_data_processing.md#ZH-CN_TOPIC_0000001860001341)、[ImageProcessor](./media_data_processing.md#ZH-CN_TOPIC_0000001813201028)、[VideoEncoder](./media_data_processing.md#ZH-CN_TOPIC_0000001860001181)和[VideoDecoder](./media_data_processing.md#ZH-CN_TOPIC_0000001860000857)）与MxDeInit的作用域冲突，因此，这些类的作用域不能大于或等于MxDeInit的作用域。
>- 执行完MxDeInit后，日志级别将会失效，且日志无法落盘。

**函数原型<a name="section15011354173612"></a>**

```cpp
APP_ERROR MxDeInit();
```

**返回参数说明<a name="section2693254123618"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|
