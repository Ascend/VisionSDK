set(PERELEMENT_EXECUTABLE "PerElementOperationTest")
set(REDUTION_EXECUTABLE "MatrixReductionsTest")
set(COREOPERATIONS_EXECUTABLE "CoreOperationsOnTensorsTest")
set(WARPING_EXECUTABLE "TensorWarpingTest")
set(FUSION_EXECUTABLE "TensorFusionTest")
set(FEATURES_EXECUTABLE "TensorFeaturesTest")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/E2eInfer/Tensor/TensorOperationsTest)

file(GLOB_RECURSE PerElementSRC PerElementOperationTest.cpp)
add_executable(${PERELEMENT_EXECUTABLE} ${PerElementSRC})
target_link_libraries(${PERELEMENT_EXECUTABLE} mxbase gtest mockcpp)

add_test(NAME ${PERELEMENT_EXECUTABLE}
        COMMAND ${PERELEMENT_EXECUTABLE} --gtest_output=xml:PerElementOperationTest.xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB_RECURSE ReductionSRC MatrixReductionsTest.cpp)
add_executable(${REDUTION_EXECUTABLE} ${ReductionSRC})
target_link_libraries(${REDUTION_EXECUTABLE} mxbase gtest mockcpp)

add_test(NAME ${REDUTION_EXECUTABLE}
        COMMAND ${REDUTION_EXECUTABLE} --gtest_output=xml:MatrixReductionsTest.xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB_RECURSE CoreOperationsSRC CoreOperationsOnTensorsTest.cpp)
add_executable(${COREOPERATIONS_EXECUTABLE} ${CoreOperationsSRC})
target_link_libraries(${COREOPERATIONS_EXECUTABLE} mxbase gtest mockcpp)

add_test(NAME ${COREOPERATIONS_EXECUTABLE}
        COMMAND ${COREOPERATIONS_EXECUTABLE} --gtest_output=xml:CoreOperationsOnTensorsTest.xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB_RECURSE TensorWarpingSRC TensorWarpingTest.cpp)
add_executable(${WARPING_EXECUTABLE} ${TensorWarpingSRC})
target_link_libraries(${WARPING_EXECUTABLE} mxbase gtest mockcpp)

add_test(NAME ${WARPING_EXECUTABLE}
        COMMAND ${WARPING_EXECUTABLE} --gtest_output=xml:TensorWarpingTest.xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB_RECURSE FusionSRC TensorFusionTest.cpp)
add_executable(${FUSION_EXECUTABLE} ${FusionSRC})
target_link_libraries(${FUSION_EXECUTABLE} mxbase gtest mockcpp)

add_test(NAME ${FUSION_EXECUTABLE}
        COMMAND ${FUSION_EXECUTABLE} --gtest_output=xml:TensorFusionTest.xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include_directories(${PROJECT_SOURCE_DIR}/../../src/mxbase)

file(GLOB_RECURSE FeaturesSRC TensorFeaturesTest.cpp)
add_executable(${FEATURES_EXECUTABLE} ${FeaturesSRC})
target_link_libraries(${FEATURES_EXECUTABLE} mxbase opencv_world gtest mockcpp)

add_test(NAME ${FEATURES_EXECUTABLE}
        COMMAND ${FEATURES_EXECUTABLE} --gtest_output=xml:TensorFeaturesTest.xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})