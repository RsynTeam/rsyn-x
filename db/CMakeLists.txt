cmake_minimum_required(VERSION 3.1)
project(db)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BINARY_DIR lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fpermissive")

file(GLOB_RECURSE SRC_FILES *.c *.cpp *.cc)

add_LIBRARY(db STATIC ${SRC_FILES})

find_package(Qt5 COMPONENTS Core)
target_include_directories(db PUBLIC ./)
target_include_directories(db PUBLIC ../include/)
target_include_directories(db PUBLIC ${Boost_INCLUDE_DIR})
