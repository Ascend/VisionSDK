[toc]

> **注1** 下面的内容以MxpiImageDecoder插件为例来说明。

## 插件DT框架介绍

插件DT框架的关键点：
1. CMakeLists.txt中申明宏GST_STATIC_COMPILATION，使插件注册代码gst_plugin_PLUGIN_NAME_register()可被测试代码直接编译链接
   - 使用add_compile_definitions(GST_STATIC_COMPILATION)来申明宏GST_STATIC_COMPILATION
   - 这里PLUGIN_NAME对MxpiImageDecoder插件来说是mxpi_imagedecoder。这是在源代码的CMakeLists.txt中用set(PLUGIN_NAME "mxpi_imagedecoder")定义的
   - 源代码中gst_plugin_mxpi_imagedecoder__register()函数是用宏MX_PLUGIN_GENERATE(MxpiImageDecoder)来定义和实现的

2. CMakeLists.txt中引入GTest测试框架
   - 使用哪种测试框架是可选的，用户可以自己定义，这里使用常见的GTest框架
   - 使用find_package(GTest REQUIRED)可以查找系统中已安装的GTest框架
     如果找到GTest框架，则
     GTEST_INCLUDE_DIRS => GTest头文件搜索路径
     GTEST_LIBRARY => libgtest
     GTEST_MAIN_LIBRARY => libgtest-main
     GTEST_BOTH_LIBRARIES => libgtest & libgtest-main}

3. 增加该测试框架的原因：
   - 常见的插件测试，采用构建pipeline的方式来进行功能验证，存在一些不便
     插件代码采用动态库的形式提供，无法直接调试控制
     对被测插件的测试输入构造能力弱（对应功能覆盖验证能力低），依赖pipeline中前置插件的能力
     其它插件存在的运行依赖
   - gstreamer提供了开发者测试的建议代码，但缺少可用的测试框架

测试的通用方式：
   - 构造被测对象（主函数）的输入
   - 调用被测对象
   - 检查被测对象的输出，以及其它外部可见的状态变化

插件测试的说明：
   - 创建/获取插件对应的类实例
   - 构造插件处理方法的输入，调用类实例方法，检查输出和其它状态
   - 这里主要测试插件的Process(std::vector<MxpiBuffer *>& mxpiBuffer)方法
     设置输入的MxpiBuffer，经过Process处理之后，检查输出的MxpiBuffer是否符合要求
   - 为了兼容gstreamer的框架，调用gst_plugin_mxpi_imagedecoder__register()，注册插件的property和pad
     对应调用源代码类函数：*DefineProperties() & DefineInputPorts() & DefineOutputPorts()*

如何获取插件对应的类实例：
   - 调用gst_element_factory_make("mxpi_imagedecoder", NULL)创建插件
   - 调用gst_element_set_state(element, GST_STATE_PLAYING)让插件进入可用状态
     element是gst_element_factory_make的返回值
     对应调用源代码的类函数：*Init()*
   - 插件的实例既类实例MxpiImageDecoder *pD = (MxpiImageDecoder *)(GST_MXBASE(element)->pluginInstance)

其它：
   - 因为已经获得了类的实例，也可以直接调用该类的其它方法按照测试的通用方式进行测试
   - 调用gst_element_set_state(decoder, GST_STATE_NULL)，对应调用源代码的类函数：*DeInit()*

### 文件说明

```
.
├-- CMakeLists.txt                - 插件DT框架 cmake配置文件
├-- DTFrameHelper.cmake           - 插件DT框架 cmake辅助宏（当前包含 提取子目录名为列表）
└-- mxpi_imagedecoder             - 插件DT 示例，建议用插件名命名
    ├-- CMakeLists.txt            - 插件DT cmake配置文件
    └-- DvppImageDecoder_test.cpp - 插件DT 测试代码
```

### stub acllib 

（待建设）可去除昇腾硬件的依赖，仅关心插件代码的行为

## 插件DT测试建议

### 准备插件不同场景的输入和输出

不同场景的输入和输出，对应的MxpiBuffer（protobuf）内容，以json文件的格式保存。

例如：
1. err_jpeg_size_input.json, err_jpeg_size_output.json
    ```json
    {
        "buffer":{},
        "metaData":[
                {"key":"ExternalObjects","protoDataType":"MxpiObjectList"},
                ...
        ]
    }
    ```

2. normal_jpeg_input.json, normal_jpeg_output.json
...


不同场景的输入和输出，可用MxpiBufferDump::DoDump/DoLoad辅助。
1. 接入mxpi_dumpdata插件获取基础的输入和输出
   {previous plugin} <-> {mxpi_dumpdata} => input.json
   {DUT: current plugin} <-> {mxpi_dumpdata} => output.json

2. 对input.json/output.json进行必要的修改


### 测试过程

1. 采用MxpiBufferDump::DoLoad功能，把input.json转换为被测插件的MxpiBuffer输入
2. 调用被测插件的Process函数
3. 捕获被测插件的输出MxpiBuffer，就output.json中的关键内容进行比较。
   最基础的比较可以把输出部分用DoDump转换为json格式，然后直接进行文件内容的差异比较。


### 准备测试代码

    ```cpp
    // 基础gtest包含
    #include <gtest/gtest.h>

    // 输出捕获函数：保留buffer指针，以便下一阶段比较的需要
    GstBuffer * g_GstBuffer;
    GstFlowReturn m_chain(GstPad * pad, GstObject * parent, GstBuffer * buffer)
    {
        g_GstBuffer = buffer;
        return GST_FLOW_OK;
    }

    // 测试环境初始化：gstreamer初始化，以及插件注册
    // 这部分会覆盖被测插件的 DefineProperties DefineInputPorts DefineOutputPorts
    void PrepareEnv()
    {
        gst_init(NULL, NULL);

        //manual register
        gst_plugin_mxpi_imagedecoder_register();
    }

    // 定义一个测试场景
    TEST(DvppImageDecoder, err_jpeg_size_decoder)
    {
        ...
    }
    ```

#### 定义一个测试场景

    ```cpp
    // --- 预置条件 ---
    PrepareEnv()

    // 准备被测插件，设置为运行态
    // 这部分会覆盖被测插件的 Init 
    GstElement *decoder = gst_element_factory_make("mxpi_imagedecoder", NULL);
    gst_element_set_state(decoder, GST_STATE_PLAYING);
    MxpiImageDecoder *pD = (MxpiImageDecoder *)(GST_MXBASE(decoder)->pluginInstance);
    ASSERT_TRUE(pD != NULL);

    // 准备一个输出捕获插件，激活，把它的输入和被测插件的输出连接起来
    // 并设置收到数据时的回调函数m_chain
    GstElement *fakesink = gst_element_factory_make("fakesink", NULL);
    gst_element_set_state(fakesink, GST_STATE_PLAYING);
    GstPad * pCatch = gst_element_get_static_pad(fakesink, "sink");
    gst_pad_link(((MxGstBase*)decoder)->srcPadVec[0], pCatch);
    gst_pad_set_chain_function_full(pCatch, m_chain, NULL, NULL);

    // --- 准备输入 ---
    MxpiBuffer *resultBuffer = NULL;
    FillInput(resultBuffer);
    std::vector<MxpiBuffer *> v_mxpiBuffer;
    v_mxpiBuffer.push_back(resultBuffer);

    // 调用被测插件处理函数
    pD->Process(v_mxpiBuffer);

    // --- 检查输出 ---
    // 前面的m_chain会把输出赋值给g_GstBuffer，检查它的内容
    // 也可以在m_chain里面直接检查传入的buffer内容
    CheckResult();

    // 这会覆盖被测插件的 DeInit 
    gst_element_set_state(decoder, GST_STATE_NULL);
    ```

### 创建插件测试工程文件CMakeLists.txt

    ```ruby
    # *need modify: plugin name*
    set(PLUGIN_NAME "mxpi_imagedecoder")

    file(GLOB_RECURSE SRCS *.cpp)

    # *need modify: plugin source code and test work dir*
    set(DUT_SRCS ${CMAKE_SOURCE_DIR}/src/module/MxpiImageDecoder/MxpiImageDecoder.cpp)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist/dt/MxpiImageDecoder)

    include_directories(
        ...
    )
    link_directories(
        ...
    )

    add_executable(
        ${TARGET_EXECUTABLE}
        ${SRCS}
        ${DUR_SRCS}
    )

    target_link_libraries(
        ...
    )

    # *need modify: plugin test resource*
    file(GLOB_RECURSE TEST_FILES ${CMAKE_SOURCE_DIR}/test/MxpiImageDecoder/*.output)

    install(FILES ${TEST_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME ${TARGET_EXECUTABLE} COMMAND ${TARGET_EXECUTABLE} WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    ```

#### 插件框架基础工程文件CMakeLists.txt

    ```ruby
    cmake_minimum_required(VERSION 3.14.1)

    project(mxPluginsDTCases)
    include(DTFrameHelper.cmake)
    add_compile_definitions(GST_STATIC_COMPILATION)

    find_package(GTest REQUIRED)
    include_directories(GTEST_INCLUDE_DIRS)

    # add all sub directory project at default
    # - if specify one, then just add that one
    # - exclude "build;test;dist;..." directory
    set(EXCLUDEDIRS build test dist)
    SUBDIRLIST_WITH_EXCLUDE(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR} "${EXCLUDEDIRS}")
    if(SPECIFY)
      ADD_SUBDIRECTORY(${SPECIFY})
    else()
      FOREACH(subdir ${SUBDIRS})
        ADD_SUBDIRECTORY(${subdir})
      ENDFOREACH()
    endif()

    ```

## FAQ

### 如何运行插件测试

   采用cmake创建本地工程的时候，需要增加定义ENABLE_DT宏。之后正常的编译、执行即可。

默认情况下会把所有插件测试都加入工程，一般来说只需要测试某个指定插件的测试，这个时候需要定义SPECIFY宏。
比如：cmake -D ENABLE_DT=true -D SPECIFY="mxpi_imagedecoder" <..CMakeLists.txt的路径>，就是创建当前演示插件的本地测试工程

当前该插件测试框架的设计规则是：日常的插件开发过程中，只需要运行本层的插件对应的测试，而无需运行其它插件/其它层面的插件测试。
 - 所以在插件的整体CMakeLists.txt中有，ENABLE_DT时并不包含其它插件源码，以及其它层面插件的测试用例。
```ruby
    if(ENABLE_DT)
        enable_testing()
        add_subdirectory(test/llt)
    else()
        add_subdirectory(src)

        if(ENABLE_TEST)
            enable_testing()
            add_subdirectory(test/hlt)
        endif()
    endif()
```
 - 建议采用SPECIFY宏指定当前开发的插件做测试，如果不设置，则会把本层的所有测试加入到测试中。


### 如何创建一个新的插件测试

1. 新建一个目录，建议采用插件名字命名；
2. 复制mxpi_imagedecoder/CMakeLists.txt文件到这个新目录下
3. 修改CMakeLists.txt中标注 *need modify:* 的部分，至少包括3个部分
   - 插件名
   - 被测代码
   - 测试资源安装路径
   根据被测插件的不同，可能还需要增加链接的库名，请根据原插件的CMakeLists.txt进行适当的修改

### MxpiSynchronize 默认情况下sink口没有创建

    设置GST_STATE_PLAYING时，Init发现sinkPadNum_==0，启动失败

这里的sink pad是Dynamic属性，需要在设置GST_STATE_PLAYING前主动request来创建。

比如：
```cpp
    // 创建request的pad模板，模板是插件创建时注册的
    // 然后通过gst_element_request_pad来申请一个
    auto sink_pad_template = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS (pluginElement_), "sink_%u");
    GstPad *sink_pad = gst_element_request_pad (filter, sink_pad_template, NULL, NULL);
```

### MxpiModelInfer 插件属性如何设置

    插件的一些属性没有默认值，需要在流水线中单独设置，比如宽高，否则功能失败

可以根据当前对插件属性的处理函数，找到对应的原生gstreamer方法来操作，尽量用用原生的gstreamer的方法来操作，避免过多的依赖。

比如： g_object_set(element, property,...
