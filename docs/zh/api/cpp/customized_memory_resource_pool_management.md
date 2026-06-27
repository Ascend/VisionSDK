# 自定义内存资源池管理<a name="ZH-CN_TOPIC_0000001860120285"></a>

## 总体说明<a name="ZH-CN_TOPIC_0000001929562397"></a>

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

## DeviceFreeFuncHookReg<a name="ZH-CN_TOPIC_0000001813361140"></a>

**函数功能<a name="section1439125764912"></a>**

注册自定义内存管理接口-Device内存释放。需与DeviceMallocFuncHookReg接口成对使用，若不匹配，或只注册了其一，会使用默认方式，直接申请/释放内存。该功能仅支持<term>Atlas 推理系列产品</term>。

相关使用流程请参考[自定义内存资源池管理](../../user_guide.md#自定义内存资源池管理)。

**函数原型<a name="section7977171713530"></a>**

```cpp
APP_ERROR DeviceFreeFuncHookReg(g_deviceFreeFuncType pFun);
```

**参数说明<a name="section622542001517"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|pFun|输入|Device内存释放的函数类型。传入的函数需和以下函数类型保持一致：<br>APP_ERROR (\*)(void*);<br>需要输入的参数依次为：dev_ptr：待释放内存的指针。|

**返回参数说明<a name="section92661820181518"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

## DeviceMallocFuncHookReg<a name="ZH-CN_TOPIC_0000001813200992"></a>

**函数功能<a name="section1439125764912"></a>**

注册自定义内存管理接口-Device内存申请。需与DeviceFreeFuncHookReg接口成对使用，若不匹配，或只注册了其一，会使用默认方式，直接申请/释放内存。该功能仅支持<term>Atlas 推理系列产品</term>。

相关使用流程请参考[自定义内存资源池管理](../../user_guide.md#自定义内存资源池管理)。

**函数原型<a name="section7977171713530"></a>**

```cpp
APP_ERROR DeviceMallocFuncHookReg(g_deviceMallocFuncType pFun);
```

**参数说明<a name="section622542001517"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|pFun|输入|Device内存申请的函数类型。传入的函数需和以下函数类型保持一致：<br>APP_ERROR (*)(void**, unsigned int, MxMemMallocPolicy);<br>需要输入的参数依次为：<li>dev_ptr：“DVPP上已分配内存的指针”的指针。</li><li>Size：申请内存的大小，单位Byte。</li><li>Policy：内存分配规则，如果用户自定义Device内存分配函数中不涉及CANN底层Device内存的分配策略，可以传任意参数。</li><br>MxMemMallocPolicy结构定义请见[MxMemMallocPolicy](./data_structures_and_enumeration_types.md#mxmemmallocpolicy)。|

**返回参数说明<a name="section92661820181518"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

## DVPPFreeFuncHookReg<a name="ZH-CN_TOPIC_0000001813200728"></a>

**函数功能<a name="section1439125764912"></a>**

注册自定义内存管理接口-DVPP内存释放。需与DVPPMallocFuncHookReg接口成对使用，若不匹配，或只注册了其一，会使用默认方式，直接申请/释放内存。该功能仅支持<term>Atlas 推理系列产品</term>。

相关使用流程请参考[自定义内存资源池管理](../../user_guide.md#自定义内存资源池管理)。

**函数原型<a name="section7977171713530"></a>**

```cpp
APP_ERROR DVPPFreeFuncHookReg(g_dvppFreeFuncType pFun);
```

**参数说明<a name="section622542001517"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|pFun|输入|DVPP内存释放的函数类型。传入的函数需和以下函数类型保持一致：<br>APP_ERROR (*)(void*);<br>需要输入的参数为：dev_ptr：待释放内存的指针|

**返回参数说明<a name="section92661820181518"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|

## DVPPMallocFuncHookReg<a name="ZH-CN_TOPIC_0000001860120821"></a>

**函数功能<a name="section1439125764912"></a>**

注册自定义内存管理接口DVPP内存申请。需与DVPPFreeFuncHookReg接口成对使用，若不匹配或只注册了其一，会使用默认方式，直接申请/释放内存。该功能仅支持<term>Atlas 推理系列产品</term>。

相关使用流程请参考[自定义内存资源池管理](../../user_guide.md#自定义内存资源池管理)。

**函数原型<a name="section7977171713530"></a>**

```cpp
APP_ERROR DVPPMallocFuncHookReg(g_dvppMallocFuncType pFun);
```

**参数说明<a name="section622542001517"></a>**

|参数名|输入/输出|说明|
|--|--|--|
|pFun|输入|DVPP内存分配的函数类型。传入的函数需和以下函数类型保持一致：<br>APP_ERROR (*)(unsigned int, void**, unsigned long long);<br>需要输入的参数依次为：<li>dev_id：根据用户实际内存资源池配合使用。</li><li>dev_ptr：“DVPP上已分配内存的指针”的指针。</li><li>Size：申请内存的大小，单位Byte。</li>|

**返回参数说明<a name="section92661820181518"></a>**

|数据结构|说明|
|--|--|
|APP_ERROR|程序执行返回的错误码，请参考[APP_ERROR说明](./basic_component_layer.md#app_error说明)。|
