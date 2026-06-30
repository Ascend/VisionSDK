# API Reference for Python

## Usage Notes

> [!NOTICE]
>
>- For the classes exposed in this section, use a single class instance in one thread. Do not use the same class instance in different threads.
>- For functions whose input is a collection data type, limit the length based on performance and actual service requirements to prevent possible memory issues.
>- Do not include comments in JSON-related content passed to interfaces. Otherwise, parsing fails.
>- If a single object in JSON-related content contains key-value pairs with the same key, the interface keeps the last set of key-value pairs by default as the parsing result.
>- Before you use Python data structures and methods, ensure that the correct modules are imported.
>- For overloaded functions in this section, such as [SendData](#senddata), or functions with default values, such as [get_result](#get_result), the parameter names in the function are defined only to describe parameter meanings. The actual function input parameter definition is `*args`. When you pass parameters, do not specify parameter names.

**Python Inference Data Process Diagram**

**Figure 1**  Inference data flow diagram
![](../../figures/inference-data-flow-diagram.png "Inference data flow diagram")
