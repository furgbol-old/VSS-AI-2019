cmake_minimum_required(VERSION 3.10)
project(vss-testing)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") 

find_package(Boost REQUIRED COMPONENTS system)

include_directories("${CMAKE_SOURCE_DIR}/include"
                    "${CMAKE_SOURCE_DIR}/lib"
                    ${Boost_INCLUDE_DIRS})

set(SRCS
    "src/geometry/field_line.cc"
    "src/geometry/vector_2d.cc"
    "src/io/receiver.cc"
    "src/io/sender.cc"
    "src/operation/cb_operation.cc"
    "src/operation/gk_operation.cc"
    "src/operation/st_operation.cc"
    "src/system/system.cc"
    "src/world_model/robot.cc"
    "src/world_model/world_model.cc")

set(VSS_CORE libvss-core.so)

add_library(vss-testing-srcs
            ${SRCS})

add_executable(testing "src/main.cpp")

target_link_libraries(testing
                        vss-testing-srcs
                        ${VSS_CORE}
                        ${Boost_LIBRARIES})