set(TARGET_EXECUTABLE "ModelInferTest")
set(TARGET_EXECUTABLE2 "MxOmModelDescTest")
set(TARGET_EXECUTABLE3 "MxMindIRModelDescTest")
set(TARGET_EXECUTABLE4 "MxModelDescTest")
set(TARGET_EXECUTABLE5 "ModelInferTestV2")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/ModelInfer)
include_directories(${PROJECT_SOURCE_DIR}/../../src/mxbase/module/Infer/Model)
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/ModelInfer/ModelInferTest.cpp)
file(GLOB_RECURSE SOURCE_FILES2 ${PROJECT_SOURCE_DIR}/ModelInfer/MxOmModelDescTest.cpp)
file(GLOB_RECURSE SOURCE_FILES3 ${PROJECT_SOURCE_DIR}/ModelInfer/MxMindIRModelDescTest.cpp)
file(GLOB_RECURSE SOURCE_FILES4 ${PROJECT_SOURCE_DIR}/ModelInfer/MxModelDescTest.cpp)
file(GLOB_RECURSE SOURCE_FILES5 ${PROJECT_SOURCE_DIR}/ModelInfer/ModelInferTestV2.cpp)
add_executable(${TARGET_EXECUTABLE} ${SOURCE_FILES})
add_executable(${TARGET_EXECUTABLE2} ${SOURCE_FILES2})
add_executable(${TARGET_EXECUTABLE3} ${SOURCE_FILES3})
add_executable(${TARGET_EXECUTABLE4} ${SOURCE_FILES4})
add_executable(${TARGET_EXECUTABLE5} ${SOURCE_FILES5})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp)
target_link_libraries(${TARGET_EXECUTABLE2} mxbase gtest mockcpp)
target_link_libraries(${TARGET_EXECUTABLE3} mxbase gtest mockcpp)
target_link_libraries(${TARGET_EXECUTABLE4} mxbase gtest mockcpp)
target_link_libraries(${TARGET_EXECUTABLE5} mxbase gtest mockcpp)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/ModelInfer/model DESTINATION ${PROJECT_SOURCE_DIR}/dist/ModelInfer)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/ModelInfer/Input DESTINATION ${PROJECT_SOURCE_DIR}/dist/ModelInfer)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/ModelInfer)

add_test(NAME ${TARGET_EXECUTABLE2}
        COMMAND ${TARGET_EXECUTABLE2} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/ModelInfer)

add_test(NAME ${TARGET_EXECUTABLE3}
        COMMAND ${TARGET_EXECUTABLE3} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/ModelInfer)

add_test(NAME ${TARGET_EXECUTABLE4}
        COMMAND ${TARGET_EXECUTABLE4} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/ModelInfer)
add_test(NAME ${TARGET_EXECUTABLE5}
        COMMAND ${TARGET_EXECUTABLE5} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/ModelInfer)
