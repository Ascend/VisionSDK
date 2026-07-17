# Custom Memory Resource Pool Management

## Overview

**Supported Models**

Atlas 200I/500 A2 inference products

Atlas inference products

## DeviceFreeFuncHookReg

**Function Description**

Register the custom memory management interface for Device memory deallocation. Use it together with `DeviceMallocFuncHookReg`. If the two interfaces do not match or if you register only one of them, the system uses the default method to allocate and free memory directly. This function is supported only on Atlas inference products.

For the usage process, see [Custom Memory Resource Pool Management](../../user_guide.md#custom-memory-resource-pool-management).

**Function Prototype**

```cpp
APP_ERROR DeviceFreeFuncHookReg(g_deviceFreeFuncType pFun);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|
|pFun|Input|Function type for Device memory deallocation. The function you pass in must match the following function signature:<br>APP_ERROR (\*)(void*);<br>The required parameter is `dev_ptr`: a pointer to the memory to be freed.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

## DeviceMallocFuncHookReg

**Function Description**

Register the custom memory management interface for Device memory allocation. Use it together with `DeviceFreeFuncHookReg`. If the two interfaces do not match or if you register only one of them, the system uses the default method to allocate and free memory directly. This function is supported only on Atlas inference products.

For the usage process, see [Custom Memory Resource Pool Management](../../user_guide.md#custom-memory-resource-pool-management).

**Function Prototype**

```cpp
APP_ERROR DeviceMallocFuncHookReg(g_deviceMallocFuncType pFun);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|pFun|Input|Function type for Device memory allocation. The function you pass in must match the following function signature:<br>APP_ERROR (*)(void**, unsigned int, MxMemMallocPolicy);<br>The required parameters are, in order:<li>`dev_ptr`: a pointer to a pointer to the memory allocated on the DVPP.</li><li>`Size`: size of the memory to allocate, in bytes.</li><li>`Policy`: memory allocation policy. If your custom Device memory allocation function does not involve the CANN underlying Device memory allocation policy, you can pass any value.</li><br>See [MxMemMallocPolicy](./data_structures_and_enumeration_types.md#mxmemmallocpolicy) for the definition of the `MxMemMallocPolicy` structure.|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

## DVPPFreeFuncHookReg

**Function Description**

Register the custom memory management interface for DVPP memory deallocation. Use it together with `DVPPMallocFuncHookReg`. If the two interfaces do not match or if you register only one of them, the system uses the default method to allocate and free memory directly. This function is supported only on Atlas inference products.

For the usage process, see [Custom Memory Resource Pool Management](../../user_guide.md#custom-memory-resource-pool-management).

**Function Prototype**

```cpp
APP_ERROR DVPPFreeFuncHookReg(g_dvppFreeFuncType pFun);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|pFun|Input|Function type for DVPP memory deallocation. The function you pass in must match the following function signature:<br>APP_ERROR (*)(void*);<br>The required parameter is `dev_ptr`: a pointer to the memory to be freed.|

**Returns**

|Data Type|Description|
|--|--|--|
|APP_ERROR|The error code returned by program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|

## DVPPMallocFuncHookReg

**Function Description**

Register the custom memory management interface for DVPP memory allocation. Use it together with `DVPPFreeFuncHookReg`. If the two interfaces do not match or if you register only one of them, the system uses the default method to allocate and free memory directly. This function is supported only on Atlas inference products.

For the usage process, see [Custom Memory Resource Pool Management](../../user_guide.md#custom-memory-resource-pool-management).

**Function Prototype**

```cpp
APP_ERROR DVPPMallocFuncHookReg(g_dvppMallocFuncType pFun);
```

**Parameters**

|Parameter|Input/Output|Description|
|--|--|--|
|pFun|Input|Function type for DVPP memory allocation. The function you pass in must match the following function signature:<br>APP_ERROR (*)(unsigned int, void**, unsigned long long);<br>The required parameters are, in order:<li>`dev_id`: use it with the user's actual memory resource pool.</li><li>`dev_ptr`: a pointer to a pointer to the memory allocated on the DVPP.</li><li>`Size`: size of the memory to allocate, in bytes.</li>|

**Returns**

|Data Type|Description|
|--|--|
|APP_ERROR|The error code returned by program execution. For details, see [APP_ERROR Description](./basic_component_layer.md#app_error-description).|
