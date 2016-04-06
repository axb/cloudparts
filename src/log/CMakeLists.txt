cmake_minimum_required(VERSION 3.3)

set (PROJECT cloudparts.log)

project(${PROJECT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#-------------------------------------
#
# deps
#
#-------------------------------------

#	Boost
#
SET(Boost_USE_STATIC_LIBS ON)
find_package( Boost COMPONENTS log program_options system date_time thread regex chrono filesystem iostreams REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

#  Protobuf
#
find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

#  GTest
#
set(GTEST_ROOT "/Users/alexbarakhtyan/works/3rdparty/googletest/googletest")
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

#  TODO: CppRestSDK
#

#  sources
#  FIXME: make it cleaner
#
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB proto ${CMAKE_CURRENT_SOURCE_DIR}/*.proto)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${proto})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

source_group( "proto" FILES ${proto} )

#-------------------------------------
#
#  targets
#
#-------------------------------------

#
# TODO: library
#

#
# TODO: server
#

#
# tests
#
add_executable(${PROJECT}
                ${sources}
                ${headers}
                ${proto}
                ${PROTO_SRCS} )

target_link_libraries(	${PROJECT}
                        ${Boost_LIBRARIES} 
                        ${PROTOBUF_LIBRARIES}
                        ${GTEST_LIBRARIES} )
#						 ws2_32 wsock32)