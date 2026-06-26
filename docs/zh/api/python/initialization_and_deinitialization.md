# 初始化和去初始化<a name="ZH-CN_TOPIC_0000001860000797"></a>

## 总体说明<a name="ZH-CN_TOPIC_0000001884339454"></a>

**支持的型号<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 推理产品</term>

<term>Atlas 推理系列产品</term>

## mx\_init<a name="ZH-CN_TOPIC_0000001860000217"></a>

**函数功能<a name="section169698281559"></a>**

全局初始化，申请设备资源与日志资源。

一个进程内只能调用一次mx\_init接口，且要与全局去初始化接口（mx\_deinit）配对使用。

对于<term>Atlas 推理系列产品</term>和Atlas 800I A2 推理服务器，可以使用mx\_init的全部函数原型，对于<term>Atlas 200I/500 A2 推理产品</term>：使用mx\_init\(\)。

相关使用流程请参考[初始化与去初始化](../../user_guide.md#初始化与去初始化)。

**函数原型<a name="section1248311218283"></a>**

```python
mx_init()
```

```python
mx_init(config: AppGlobalCfgExtra)
```

**输入参数说明<a name="section1240595711710"></a>**

|参数名|类型|说明|
|--|--|--|
|config|AppGlobalCfgExtra|AppGlobalCfgExtra类型，应用全局配置。|

**返回参数说明<a name="section34928557275"></a>**

|数据结构|说明|
|--|--|
|int|程序执行返回的错误码。|

## mx\_deinit<a name="ZH-CN_TOPIC_0000001813200432"></a>

**函数功能<a name="section1432565411361"></a>**

全局去初始化，释放Log、Device等相关资源，需与全局初始化接口（[mx\_init\(\)](#mx_init)）配套使用。

> [!NOTE]
>
>- 涉及申请Device侧资源的类（[Tensor](./media_data_processing.md#ZH-CN_TOPIC_0000001813201172)、[Model](./model_inference.md#ZH-CN_TOPIC_0000001813201248)、[Image](./media_data_processing.md#ZH-CN_TOPIC_0000001813201280)、[ImageProcessor](./media_data_processing.md#ZH-CN_TOPIC_0000001860120493)、[VideoEncoder](./media_data_processing.md#ZH-CN_TOPIC_0000001813360488)和[VideoDecoder](./media_data_processing.md#ZH-CN_TOPIC_0000001860120573)）与mx\_deinit\(\)的作用域冲突，因此，这些类的作用域不能大于或等于mx\_deinit\(\)的作用域。
>- 执行完mx\_deinit\(\)后，日志级别将会失效，且日志无法落盘。

**函数原型<a name="section997114918358"></a>**

```python
mx_deinit()
```

**返回参数说明<a name="section34928557275"></a>**

|数据结构|说明|
|--|--|
|int|程序执行返回的错误码。|
