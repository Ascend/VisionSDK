# Initialization and Deinitialization

## Overview

**Supported Models**

Atlas 200I/500 A2 inference products.

Atlas inference series products.

## `mx_init`

**Function Description**

Performs global initialization and allocates device resources and log resources.

You can call the `mx_init` interface only once in a process, and you must pair it with the global deinitialization interface (`mx_deinit`).

For Atlas inference series products and Atlas 800I A2 inference servers, you can use all function prototypes of `mx_init`. For Atlas 200I/500 A2 inference products, use `mx_init()`.

For details about the usage process, see [Initialization and Deinitialization](../user_guide.md#initialization-and-deinitialization).

**Function Prototype**

```python
mx_init()
```

```python
mx_init(config: AppGlobalCfgExtra)
```

**Input Parameter Description**

|Parameter|Type|Description|
|--|--|--|
|config|AppGlobalCfgExtra|`AppGlobalCfgExtra` type, global application configuration.|

**Returns**

|Data Structure|Description|
|--|--|
|int|Error code returned by the program execution.|

## `mx_deinit`

**Function Description**

Performs global deinitialization and releases resources related to logs and devices. You must use this function together with the global initialization interface ([mx_init()](#mx_init)).

> [!NOTE]
>
>- The scope of classes that allocate device-side resources, such as [Tensor](#tensor), [Model](#model), [Image](#image), [ImageProcessor](#imageprocessor), [VideoEncoder](#videoencoder), and [VideoDecoder](#videodecoder), conflicts with the scope of `mx_deinit()`. Therefore, the scope of these classes cannot be greater than or equal to the scope of `mx_deinit()`.
>- After `mx_deinit()` is executed, the log level becomes invalid and logs can no longer be written to disk.

**Function Prototype**

```python
mx_deinit()
```

**Returns**

|Data Structure|Description|
|--|--|
|int|Error code returned by the program execution.|
