# Customized Memory Resource Pool Management<a name="ZH-CN_TOPIC_0000001860120285"></a>

## Overview<a name="ZH-CN_TOPIC_0000001929562397"></a>

**Supported Models<a name="section1714913853014"></a>**

<term>Atlas 200I/500 A2 inference products</term>

<term>Atlas inference products</term>

## `DeviceFreeFuncHookReg`<a name="ZH-CN_TOPIC_0000001813361140"></a>

**Function<a name="section1439125764912"></a>**

Registers the customized memory management interface for releasing Device memory. You must use it together with the DeviceMallocFuncHookReg interface. If they do not match, or only one of them is registered, the system uses the default method to directly allocate or release memory. This function is supported only on <term>Atlas inference products</term>.

For the related usage process, see [Customized Memory Resource Pool Management](../../user_guide.md#custom-memory-resource-pool-management).

**Function Prototype<a name="section7977171713530"></a>**

```cpp
APP_ERROR DeviceFreeFuncHookReg(g_deviceFreeFuncType pFun);
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|pFun|Input|Function type for releasing Device memory. The function passed in must be consistent with the following function type:<br>`APP_ERROR (*)(void*);`<br>The parameter to be passed in is `dev_ptr`, a pointer to the memory to be released.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

## `DeviceMallocFuncHookReg`<a name="ZH-CN_TOPIC_0000001813200992"></a>

**Function<a name="section1439125764912"></a>**

Registers the customized memory management interface for allocating Device memory. You must use it together with the DeviceFreeFuncHookReg interface. If they do not match, or only one of them is registered, the system uses the default method to directly allocate or release memory. This function is supported only on <term>Atlas inference products</term>.

For the related usage process, see [Customized Memory Resource Pool Management](../../user_guide.md#custom-memory-resource-pool-management).

**Function Prototype<a name="section7977171713530"></a>**

```cpp
APP_ERROR DeviceMallocFuncHookReg(g_deviceMallocFuncType pFun);
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|pFun|Input|Function type for allocating Device memory. The function passed in must be consistent with the following function type:<br>`APP_ERROR (*)(void**, unsigned int, MxMemMallocPolicy);`<br>The parameters to be passed in are as follows:<li>`dev_ptr`: Pointer to the pointer to the memory allocated on DVPP.</li><li>`Size`: Size of the memory to be allocated, in bytes.</li><li>`Policy`: Memory allocation rule. If your customized Device memory allocation function does not involve the allocation policy of the underlying CANN Device memory, you can pass any parameter.</li><br>For the MxMemMallocPolicy structure definition, see [MxMemMallocPolicy](./data_structures_and_enumeration_types.md#mxmemmallocpolicy).|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

## `DVPPFreeFuncHookReg`<a name="ZH-CN_TOPIC_0000001813200728"></a>

**Function<a name="section1439125764912"></a>**

Registers the customized memory management interface for releasing DVPP memory. You must use it together with the DVPPMallocFuncHookReg interface. If they do not match, or only one of them is registered, the system uses the default method to directly allocate or release memory. This function is supported only on <term>Atlas inference products</term>.

For the related usage process, see [Customized Memory Resource Pool Management](../../user_guide.md#custom-memory-resource-pool-management).

**Function Prototype<a name="section7977171713530"></a>**

```cpp
APP_ERROR DVPPFreeFuncHookReg(g_dvppFreeFuncType pFun);
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|pFun|Input|Function type for releasing DVPP memory. The function passed in must be consistent with the following function type:<br>`APP_ERROR (*)(void*);`<br>The parameter to be passed in is `dev_ptr`, a pointer to the memory to be released.|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|

## `DVPPMallocFuncHookReg`<a name="ZH-CN_TOPIC_0000001860120821"></a>

**Function<a name="section1439125764912"></a>**

Registers the customized memory management interface for allocating DVPP memory. You must use it together with the DVPPFreeFuncHookReg interface. If they do not match, or only one of them is registered, the system uses the default method to directly allocate or release memory. This function is supported only on <term>Atlas inference products</term>.

For the related usage process, see [Customized Memory Resource Pool Management](../../user_guide.md#custom-memory-resource-pool-management).

**Function Prototype<a name="section7977171713530"></a>**

```cpp
APP_ERROR DVPPMallocFuncHookReg(g_dvppMallocFuncType pFun);
```

**Parameters<a name="section622542001517"></a>**

|Parameter|Input/Output|Description|
|--|--|--|
|pFun|Input|Function type for allocating DVPP memory. The function passed in must be consistent with the following function type:<br>`APP_ERROR (*)(unsigned int, void**, unsigned long long);`<br>The parameters to be passed in are as follows:<li>`dev_id`: Used together with your actual memory resource pool.</li><li>`dev_ptr`: Pointer to the pointer to the memory allocated on DVPP.</li><li>`Size`: Size of the memory to be allocated, in bytes.</li>|

**Returns<a name="section92661820181518"></a>**

|Data Structure|Description|
|--|--|
|APP_ERROR|Error code returned by the program execution. See [APP_ERROR description](./basic_component_layer.md#app_error-description).|
