cmake_minimum_required(VERSION 3.3)

set (PROJECT cloudparts.data)

project(${PROJECT})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

#	Boost
#
SET(Boost_USE_STATIC_LIBS ON)
find_package( Boost COMPONENTS log program_options system date_time thread regex chrono filesystem iostreams REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})


#
# library
#

#
# server
#

### TEMP
file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
add_executable(	${PROJECT}
                ${sources}
                ${headers} )

target_link_libraries(	${PROJECT}
                        ${Boost_LIBRARIES} )
#						 ws2_32 wsock32)