cmake_minimum_required(VERSION 3.14.1)
project(ProtoFile)

set(proto_base ${OPENSOURCE_DIR})
set(project_src_dir ${PROJECT_SOURCE_DIR})
set(out_path ${PROJECT_SOURCE_DIR})
set(work_dir ${CMAKE_CURRENT_SOURCE_DIR})

set(ARG_CPPFILES
    ${PROJECT_SOURCE_DIR}/MxpiDataType.proto
    ${PROJECT_SOURCE_DIR}/MxpiDumpData.proto
    ${PROJECT_SOURCE_DIR}/MxpiOSDType.proto
    ${PROJECT_SOURCE_DIR}/MxpiInternalType.proto)

set(ARG_PYFILES ${PROJECT_SOURCE_DIR}/MxpiDataType.proto
                ${PROJECT_SOURCE_DIR}/MxpiOSDType.proto)

# protoc path
set(PROTOBUF_PROTOC_EXECUTABLE ${proto_base}/bin/protoc)
include_directories(${proto_base}/include)
link_directories(${proto_base}/lib)

# protoc flags
set(PROTO_FLAGS -I${project_src_dir})

# cpp files
foreach(cpp_out_file IN ITEMS ${ARG_CPPFILES})
  execute_process(
    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${PROTO_FLAGS} --cpp_out=${out_path}
            ${cpp_out_file} WORKING_DIRECTORY ${work_dir})
endforeach()

# python files
foreach(py_out_file IN ITEMS ${ARG_PYFILES})
  execute_process(
    COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${PROTO_FLAGS}
            --python_out=${out_path} ${py_out_file}
    WORKING_DIRECTORY ${work_dir})
endforeach()
file(GLOB CUR_DIR_SRCS "*.cc")
add_library(mxpidatatype SHARED ${CUR_DIR_SRCS})
target_link_libraries(mxpidatatype ${LIB_PREFIX}protobuf ${test_deps})

install(TARGETS mxpidatatype DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES MxpiDataType_pb2.py MxpiOSDType_pb2.py
        DESTINATION ${CMAKE_INSTALL_PREFIX}/python)
install(FILES MxpiDataType.pb.h MxpiDumpData.pb.h MxpiOSDType.pb.h
              MxpiDataTypeDeleter.h MxpiDataTypeConverter.h
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MxTools/Proto)
