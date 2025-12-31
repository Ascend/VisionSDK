set(TARGET_PYAPI_EXECUTABLE "test_pyapi")
set(DirectoryName "PyAPITest")

install(DIRECTORY ${PROJECT_SOURCE_DIR}/${DirectoryName} DESTINATION ${PROJECT_SOURCE_DIR}/dist)

add_test(NAME ${TARGET_PYAPI_EXECUTABLE}
        COMMAND python3 "test_pyapi.py"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${DirectoryName})

set_tests_properties(${TARGET_PYAPI_EXECUTABLE} PROPERTIES
        ENVIRONMENT PYTHONPATH=${PROJECT_SOURCE_DIR}/../../mindx/sdk:$ENV{PYTHONPATH})

set(TARGET_DVPP_EXECUTABLE "test_dvpp")
add_test(NAME ${TARGET_DVPP_EXECUTABLE}
        COMMAND python3 "test_dvpp.py"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${DirectoryName})

set_tests_properties(${TARGET_DVPP_EXECUTABLE} PROPERTIES
        ENVIRONMENT PYTHONPATH=${PROJECT_SOURCE_DIR}/../../mindx/sdk:$ENV{PYTHONPATH})

set(TARGET_IMAGE_PROCESSOR_EXECUTABLE "test_image_processor")
add_test(NAME ${TARGET_IMAGE_PROCESSOR_EXECUTABLE}
        COMMAND python3 "test_image_processor.py"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${DirectoryName})

set_tests_properties(${TARGET_IMAGE_PROCESSOR_EXECUTABLE} PROPERTIES
        ENVIRONMENT PYTHONPATH=${PROJECT_SOURCE_DIR}/../../mindx/sdk:$ENV{PYTHONPATH})

set(TARGET_IMAGE_EXECUTABLE "test_image")
add_test(NAME ${TARGET_IMAGE_EXECUTABLE}
        COMMAND python3 "test_image.py"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${DirectoryName})

set_tests_properties(${TARGET_IMAGE_EXECUTABLE} PROPERTIES
        ENVIRONMENT PYTHONPATH=${PROJECT_SOURCE_DIR}/../../mindx/sdk:$ENV{PYTHONPATH})

set(TARGET_LOG_EXECUTABLE "test_log")
add_test(NAME ${TARGET_LOG_EXECUTABLE}
        COMMAND python3 "test_log.py"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${DirectoryName})

set_tests_properties(${TARGET_LOG_EXECUTABLE} PROPERTIES
        ENVIRONMENT PYTHONPATH=${PROJECT_SOURCE_DIR}/../../mindx/sdk:$ENV{PYTHONPATH})

set(TARGET_MODEL_EXECUTABLE "test_model")
add_test(NAME ${TARGET_MODEL_EXECUTABLE}
        COMMAND python3 "test_model.py"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${DirectoryName})

set_tests_properties(${TARGET_MODEL_EXECUTABLE} PROPERTIES
        ENVIRONMENT PYTHONPATH=${PROJECT_SOURCE_DIR}/../../mindx/sdk:$ENV{PYTHONPATH})

set(TARGET_TENSOR_EXECUTABLE "test_tensor")
add_test(NAME ${TARGET_TENSOR_EXECUTABLE}
        COMMAND python3 "test_tensor.py"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${DirectoryName})

set_tests_properties(${TARGET_TENSOR_EXECUTABLE} PROPERTIES
        ENVIRONMENT PYTHONPATH=${PROJECT_SOURCE_DIR}/../../mindx/sdk:$ENV{PYTHONPATH})

set(TARGET_MATRICES_CORE_OPERATIONS_ON_TENSORS_EXECUTABLE "test_matrices_core_operations_on_tensors")
add_test(NAME ${TARGET_MATRICES_CORE_OPERATIONS_ON_TENSORS_EXECUTABLE}
        COMMAND python3 "test_matrices_core_operations_on_tensors.py"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${DirectoryName})

set_tests_properties(${TARGET_MATRICES_CORE_OPERATIONS_ON_TENSORS_EXECUTABLE} PROPERTIES
        ENVIRONMENT PYTHONPATH=${PROJECT_SOURCE_DIR}/../../mindx/sdk:$ENV{PYTHONPATH})

set(TARGET_PER_ELEMENT_OPERATIONS_EXECUTABLE "test_per_element_operations.py")
add_test(NAME ${TARGET_PER_ELEMENT_OPERATIONS_EXECUTABLE}
        COMMAND python3 "test_per_element_operations.py"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${DirectoryName})

set_tests_properties(${TARGET_PER_ELEMENT_OPERATIONS_EXECUTABLE} PROPERTIES
        ENVIRONMENT PYTHONPATH=${PROJECT_SOURCE_DIR}/../../mindx/sdk:$ENV{PYTHONPATH})