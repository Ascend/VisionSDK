cmake_minimum_required(VERSION 3.14.1)
project(
  MXBASE_PYTHON
  LANGUAGES CXX C
  VERSION 1.0.0)

option(COVERAGE "enable code coverage" OFF)
option(BUILD_TESTS "Build the tests" OFF)

set(CMAKE_CXX_STANDARD 14)
add_compile_options(-std=c++14 -fPIE -fstack-protector-all -fPIC -Wall -Wfloat-equal -pipe -fno-common -Wextra)
set(CMAKE_CXX_FLAGS
    "-Wno-error=deprecated-declarations -Wno-deprecated-declarations -Wno-error=pointer-arith -Wno-pointer-arith -fstack-protector-all"
)

if(HIGH_TESTS)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE hitestwrapper)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK hitestwrapper)
endif()

if(BUILD_TESTS)
  # Add compile option -g when build test
  add_compile_options(-g -O2)
  # Remove link option -s when build test
  add_link_options(-Wl,-z,relro,-z,now,-z,noexecstack -pie)
else()
  add_link_options(-Wl,-z,relro,-z,now,-z,noexecstack -s -pie)
  add_compile_options(-O2)
endif()

add_definitions(-DENABLE_DVPP_INTERFACE)
# Set namespace of glog from google to mindxsdk_private
add_definitions(-Dgoogle=mindxsdk_private)
add_definitions(-DENABLE_DVPP_INTERFACE)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../mxbase/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../postprocess/include)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../output/${FRAME_PROJECT}/mxBase/lib)
link_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/../../output/${FRAME_PROJECT}/mxBase/lib/modelpostprocessors
)

# ascend dependency
set(ASCEND_HOME /usr/local/Ascend)
set(ASCEND_VERSION ascend-toolkit/latest)
set(ASCEND_LIB $ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/acllib)
include_directories(${ASCEND_LIB}/include)
include_directories($ENV{ASCEND_HOME_PATH}/include)
include_directories($ENV{ASCEND_HOME}/driver/include)
include_directories(/usr/local/Ascend/driver/include/)
set(MSL_ROOT /opt/buildtools/mindspore-lite-2.1.0/mindspore-lite-2.1.0-linux)
if(EXISTS "${MSL_ROOT}-x64/runtime")
    set(MSL_RUNTIME "${MSL_ROOT}-x64/runtime")
elseif(EXISTS "${MSL_ROOT}-aarch64/runtime")
    set(MSL_RUNTIME "${MSL_ROOT}-aarch64/runtime")
else()
    message(FATAL_ERROR "MindSpore-Lite runtime directory not found!")
endif()
include_directories("${MSL_RUNTIME}/include")
include_directories("${MSL_RUNTIME}")
link_directories("${MSL_RUNTIME}/lib")
include_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/$ENV{ARCH_PATTERN}/include)
include_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/opp/include)
link_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/opp/lib64)
link_directories($ENV{ASCEND_HOME_PATH}/lib64)
link_directories($ENV{ASCEND_HOME}/driver/lib64)
link_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/compiler/lib64)

get_filename_component(
  OPENSOURCE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../opensource/output/${FRAME_PROJECT}/opensource
  ABSOLUTE)
include_directories(${OPENSOURCE_DIR}/include)
include_directories(${OPENSOURCE_DIR}/include/opencv4)
link_directories(${OPENSOURCE_DIR}/lib)

set(test_deps)
if(COVERAGE)
  add_compile_options(-fprofile-arcs -ftest-coverage)
  list(APPEND test_deps gcov)
endif()

add_definitions(-DPy_LIMITED_API=0x03090000)
find_package(SWIG REQUIRED)
include(UseSWIG)
find_package(Python3 ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Interpreter
                                     Development)
# add base target
set(PYTHON_BASE_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/module/PyImage/PyImage.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyImageProcessor/PyImageProcessor.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyInit/PyInit.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyModel/PyModel.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyTensor/PyTensor.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyPostProcessDataType/PyPostProcessDataType.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyTensorOperations/PyMatricesCoreOperationsOnTensors.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyTensorOperations/PyPerElementOperations.cpp
)

set(PYTHON_BASE_TARGET base)
set_property(SOURCE SwigInterface/Base.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE SwigInterface/Base.i PROPERTY SWIG_MODULE_NAME
                    ${PYTHON_BASE_TARGET})


swig_add_library(
${PYTHON_BASE_TARGET}
LANGUAGE python
SOURCES SwigInterface/Base.i ${PYTHON_BASE_SRC_FILES})
target_include_directories(${PYTHON_BASE_TARGET} PRIVATE ${Python3_INCLUDE_DIRS})
target_link_directories(${PYTHON_BASE_TARGET} PRIVATE ${Python3_LIBRARIES})
swig_link_libraries(${PYTHON_BASE_TARGET} 
    glog
    ascendcl
    acl_dvpp
    acl_dvpp_mpi
    mxbase
    opencv_world
    ${test_deps}
)

# add dvpp target
set(PYTHON_DVPP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/module/PyDvpp/PyDvpp.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyImage/PyImage.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyImageProcessor/PyImageProcessor.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyTensor/PyTensor.cpp
)

set(PYTHON_DVPP_TARGET dvpp)
set_property(SOURCE SwigInterface/Dvpp.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE SwigInterface/Dvpp.i PROPERTY SWIG_MODULE_NAME
                    ${PYTHON_DVPP_TARGET})


swig_add_library(
${PYTHON_DVPP_TARGET}
LANGUAGE python
SOURCES SwigInterface/Dvpp.i ${PYTHON_DVPP_SRC_FILES})
target_include_directories(${PYTHON_DVPP_TARGET} PRIVATE ${Python3_INCLUDE_DIRS})
target_link_directories(${PYTHON_DVPP_TARGET} PRIVATE ${Python3_LIBRARIES})
swig_link_libraries(${PYTHON_DVPP_TARGET} 
    glog
    ascendcl
    acl_dvpp
    acl_dvpp_mpi
    mxbase
    opencv_world
    ${test_deps}
)

# add post process target
set(PYTHON_POST_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/module/PyPostProcessDataType/PyPostProcessDataType.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyImage/PyImage.cpp
${CMAKE_CURRENT_SOURCE_DIR}/module/PyTensor/PyTensor.cpp
)
set(PYTHON_POST_TARGET post)
set_property(SOURCE SwigInterface/Post.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE SwigInterface/Post.i PROPERTY SWIG_MODULE_NAME
                    ${PYTHON_POST_TARGET})


swig_add_library(
${PYTHON_POST_TARGET}
LANGUAGE python
SOURCES SwigInterface/Post.i ${PYTHON_POST_SRC_FILES})
target_include_directories(${PYTHON_POST_TARGET} PRIVATE ${Python3_INCLUDE_DIRS})
target_link_directories(${PYTHON_POST_TARGET} PRIVATE ${Python3_LIBRARIES})
swig_link_libraries(${PYTHON_POST_TARGET} 
    glog
    mxbase
    opencv_world
    crnnpostprocess
    ctpnpostprocess
    deeplabv3post
    fasterrcnnpostprocess
    higherhrnetpostprocess
    maskrcnnmindsporepost
    openposepostprocess
    psenetpostprocess
    resnet50postprocess
    retinanetpostprocess
    SsdMobilenetFpn_MindsporePost
    ssdmobilenetv1fpnpostprocess
    ssdvgg16postprocess
    transformerpostprocess
    unetmindsporepostprocess
    yolov3postprocess
    ${test_deps}
)

# add log target
set(PYTHON_LOG_TARGET log)
set_property(SOURCE SwigInterface/Log.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE SwigInterface/Log.i PROPERTY SWIG_MODULE_NAME
                    ${PYTHON_LOG_TARGET})


swig_add_library(
${PYTHON_LOG_TARGET}
LANGUAGE python
SOURCES SwigInterface/Log.i)
target_include_directories(${PYTHON_LOG_TARGET} PRIVATE ${Python3_INCLUDE_DIRS})
target_link_directories(${PYTHON_LOG_TARGET} PRIVATE ${Python3_LIBRARIES})
swig_link_libraries(${PYTHON_LOG_TARGET} 
    glog
    mxbase
    ${test_deps}
)

install(TARGETS ${PYTHON_BASE_TARGET} LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../mindx/sdk/base)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/base.py DESTINATION  ${CMAKE_CURRENT_SOURCE_DIR}/../../mindx/sdk/base)
install(TARGETS ${PYTHON_DVPP_TARGET} LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../mindx/sdk/base/dvpp)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dvpp.py DESTINATION  ${CMAKE_CURRENT_SOURCE_DIR}/../../mindx/sdk/base/dvpp)
install(TARGETS ${PYTHON_POST_TARGET} LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../mindx/sdk/base/post)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/post.py DESTINATION  ${CMAKE_CURRENT_SOURCE_DIR}/../../mindx/sdk/base/post)
install(TARGETS ${PYTHON_LOG_TARGET} LIBRARY DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../../mindx/sdk/base/log)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/log.py DESTINATION  ${CMAKE_CURRENT_SOURCE_DIR}/../../mindx/sdk/base/log)