cmake_minimum_required(VERSION 3.10)
project(vss-testing-serial)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include_directories("${CMAKE_SOURCE_DIR}/include"
                    "${CMAKE_SOURCE_DIR}/lib"
                    "${CMAKE_SOURCE_DIR}/submodules/furgbol-core/dist/include")

set(SRCS 
    "src/communications/receiver.cc"
    "src/communications/sender.cc"
    "src/communications/serial_repository.cc"
    "src/geometry/field_line.cc"
    "src/geometry/vector_2d.cc"
    #"src/operations/centerback_operation.cc"
    "src/operations/goalkeeper_operation.cc"
    "src/operations/operation.cc"
    #"src/operations/striker_operation.cc"
    "src/system/system.cc"
    "src/world_model/robot.cc"
    "src/world_model/world_model.cc")

set(VSS_CORE libvss-core.so)

add_library(vss-testing-serial-srcs ${SRCS})

add_executable(testing "src/main.cpp")

target_link_libraries(vss-testing-serial-srcs
                        "${CMAKE_SOURCE_DIR}/submodules/furgbol-core/dist/lib/libfurgbol-core-io.a"
                        serial)

target_link_libraries(testing
                        vss-testing-serial-srcs
                        ${VSS_CORE})