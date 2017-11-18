# Rsyn
# CMake Examples: https://github.com/ttroy50/cmake-examples

#########
# Setup #
#########

# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.1)

# Message
message(STATUS "Rsyn")
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

# Project's name
project(rsyn)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ../bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 11)

set(PATH_RSYN ${CMAKE_CURRENT_SOURCE_DIR}/../rsyn)
set(PATH_X ${CMAKE_CURRENT_SOURCE_DIR}/../x)

######################
# Check Dependencies #
######################
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(wxWidgets COMPONENTS core base gl propgrid REQUIRED)
find_package(PNG REQUIRED)
find_package(COIN)

###############
# Source Code #
###############
file(GLOB_RECURSE SRC_FILES_RSYN 
    ${PATH_RSYN}/src/*.cpp 
    ${PATH_RSYN}/src/*.cc 
    ${PATH_RSYN}/src/*.c)

file(GLOB_RECURSE SRC_FILES_X
    ${PATH_X}/src/*.cpp 
    ${PATH_X}/src/*.cc 
    ${PATH_X}/src/*.c)

set(SRC_FILES ${SRC_FILES_RSYN} ${SRC_FILES_X})

if(NOT COIN_FOUND)
  list(REMOVE_ITEM SRC_FILES ${PATH_X}/src/x/opto/ufrgs/qpdp/OverlapRemover.h)
  list(REMOVE_ITEM SRC_FILES ${PATH_X}/src/x/opto/ufrgs/qpdp/OverlapRemover.cpp)
else()
  add_definitions(-DRSYN_ENABLE_OVERLAP_REMOVER=1)
endif()

#################
# Library Paths #
#################
# Need to come before target is created.

link_directories(${PATH_RSYN}/lib)
link_directories(${PATH_X}/lib)

###########
# Targets #
###########

add_executable(rsyn ${SRC_FILES})

#######################
# Include Directories #
#######################

target_include_directories(rsyn PUBLIC "${PATH_RSYN}/src")
target_include_directories(rsyn PUBLIC "${PATH_RSYN}/include")
target_include_directories(rsyn PUBLIC "${PATH_X}/src")
target_include_directories(rsyn PUBLIC "${PATH_X}/include")

##################
# Compiler Flags #
##################

target_compile_options(rsyn PUBLIC "-O3")
target_compile_options(rsyn PUBLIC "-Wno-unused-result -Wno-format -Wno-return-type -Wno-unused-local-typedefs")

#################
# Linker Flags  #
#################

target_link_libraries(rsyn lef)
target_link_libraries(rsyn def)
target_link_libraries(rsyn emon) # not a typo

# OpenGL
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(rsyn ${OPENGL_gl_LIBRARY})
target_link_libraries(rsyn ${OPENGL_glu_LIBRARY})

# GLEW
target_include_directories(rsyn PUBLIC ${GLEW_INCLUDE_DIRS})
target_link_libraries(rsyn ${GLEW_LIBRARIES})

# wxWidgets
include(${wxWidgets_USE_FILE})
target_link_libraries(rsyn ${wxWidgets_LIBRARIES})

# Boost   
target_include_directories(rsyn PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(rsyn ${Boost_LIBRARIES})
    
# PNG
target_include_directories(rsyn PUBLIC ${PNG_INCLUDE_DIR})
target_link_libraries(rsyn ${PNG_LIBRARY})

# COIN
if(COIN_FOUND)
  target_include_directories(rsyn PUBLIC ${COIN_INCLUDE_DIR})
  target_link_libraries(rsyn ${COIN_LIBRARIES})
endif()

