CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(db)

SET(CMAKE_CXX_STANDARD 11)

FILE(GLOB_RECURSE SRC_FILES *.c *.cpp ../include/)

ADD_LIBRARY(db STATIC ${SRC_FILES})

LIST(REMOVE_ITEM SRC_FILES ../include/munkres/matrix.cpp ../include/munkres/matrix.h)

# target_link_libraries(db lef)
# target_link_libraries(db def)


find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
TARGET_INCLUDE_DIRECTORIES(db PUBLIC ${Boost_INCLUDE_DIR})
TARGET_INCLUDE_DIRECTORIES(db PUBLIC ../src)
TARGET_INCLUDE_DIRECTORIES(db PUBLIC ../src/contests)
TARGET_INCLUDE_DIRECTORIES(db PUBLIC ../include)
TARGET_INCLUDE_DIRECTORIES(db PUBLIC ./)
