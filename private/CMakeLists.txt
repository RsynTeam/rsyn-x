# CMake Examples: https://github.com/ttroy50/cmake-examples

# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.0)

# Message
message(STATUS "Hello World")
message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

# Project's name
project(rsyn)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ../bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(PATH_RSYN ${CMAKE_CURRENT_SOURCE_DIR}/../rsyn)
set(PATH_X ${CMAKE_CURRENT_SOURCE_DIR}/../x)

# Source files.
file(GLOB_RECURSE SRC_FILES_RSYN 
    ${PATH_RSYN}/src/*.cpp 
    ${PATH_RSYN}/src/*.cc 
    ${PATH_RSYN}/src/*.c)

file(GLOB_RECURSE SRC_FILES_X
    ${PATH_X}/src/*.cpp 
    ${PATH_X}/src/*.cc 
    ${PATH_X}/src/*.c)

set(SRC_FILES ${SRC_FILES_RSYN} ${SRC_FILES_X})
#message(STATUS ${SRC_FILES})

# Library paths. Need to come before target is created.
link_directories(${PATH_RSYN}/lib)
link_directories(${PATH_X}/lib)

# Create a target executable.
add_executable(rsyn ${SRC_FILES})

# Includes paths.
target_include_directories(rsyn PUBLIC "${PATH_RSYN}/src")
target_include_directories(rsyn PUBLIC "${PATH_RSYN}/include")
target_include_directories(rsyn PUBLIC "${PATH_X}/src")
target_include_directories(rsyn PUBLIC "${PATH_X}/include")

# Compiler flags.
#set(CMAKE_CXX_STANDARD 11) #not working
target_compile_options(rsyn PUBLIC "-std=c++11")
target_compile_options(rsyn PUBLIC "-O3")
target_compile_options(rsyn PUBLIC "-Wno-unused-result -Wno-format -Wno-return-type -Wno-unused-local-typedefs")

# Linker flags.
target_link_libraries(rsyn lef)
target_link_libraries(rsyn def)
target_link_libraries(rsyn emon) # not a typo

# Compiler definitions.
target_compile_definitions(rsyn PRIVATE FOO=1)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(rsyn ${OPENGL_gl_LIBRARY})
target_link_libraries(rsyn ${OPENGL_glu_LIBRARY})

#GLEW
find_package(GLEW REQUIRED)
target_include_directories(rsyn PUBLIC ${GLEW_INCLUDE_DIRS})
target_link_libraries(rsyn ${GLEW_LIBRARIES})

# wxWidgets
find_package(wxWidgets COMPONENTS core base gl propgrid REQUIRED)
include(${wxWidgets_USE_FILE})
target_link_libraries(rsyn ${wxWidgets_LIBRARIES})

# Boost   
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
target_include_directories(rsyn PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(rsyn ${Boost_LIBRARIES})
    
# PNG
find_package(PNG REQUIRED)
target_include_directories(rsyn PUBLIC ${PNG_INCLUDE_DIR})
target_link_libraries(rsyn ${PNG_LIBRARY})

# COIN
find_package(COIN)
target_include_directories(rsyn PUBLIC ${COIN_INCLUDE_DIR})
target_link_libraries(rsyn ${COIN_LIBRARIES})

if(COIN_FOUND)
  set(HAS_COIN TRUE)
endif()

