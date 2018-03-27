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

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)

# Project's name
project(rsyn)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 11)

set(PATH_RSYN ${CMAKE_CURRENT_SOURCE_DIR}/../rsyn)
set(PATH_ISPD18 ${CMAKE_CURRENT_SOURCE_DIR})
set(PATH_QT ${CMAKE_CURRENT_SOURCE_DIR}/../rsyn/src/rsyn/qt)

###################
# Warnings/Errors #
###################

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror=return-type")

######################
# Check Dependencies #
######################
find_package(Boost COMPONENTS system filesystem program_options REQUIRED)
find_package(PNG REQUIRED)

# OpenMP (not working)
#find_package(OpenMP)
#if(OPENMP_FOUND)
#  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

# OpenMP (work around)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5OpenGL REQUIRED)

#find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

###############
# Source Code #
###############
file(GLOB_RECURSE SRC_FILES_RSYN 
    ${PATH_RSYN}/src/*.cpp 
    ${PATH_RSYN}/src/*.cc 
    ${PATH_RSYN}/src/*.c)

file(GLOB_RECURSE SRC_FILES_ISPD18
    ${PATH_ISPD18}/src/*.cpp 
    ${PATH_ISPD18}/src/*.cc 
    ${PATH_ISPD18}/src/*.c)

set(SRC_FILES ${SRC_FILES_RSYN} ${SRC_FILES_ISPD18})

#####################
# Qt Pre-Processing #
#####################

file(GLOB_RECURSE UI_FILES
    ${PATH_QT}/ui/*.ui)

file(GLOB_RECURSE QRC_FILES
    ${PATH_QT}/resource/*.qrc)

# Generate code from ui files
qt5_wrap_ui(UI_HEADERS ${UI_FILES})

# Generate rules for building source files from the resources
qt5_add_resources(QRCS ${QRC_FILES})

# Tell CMake to create the executable
list(APPEND SRC_FILES ${UI_HEADERS})
list(APPEND SRC_FILES ${QRCS})
list(APPEND SRC_FILES ${GUI_FILES})

#message(STATUS ${SRC_FILES})

#################
# Library Paths #
#################
# Need to come before target is created.

if(LINUX)
link_directories(${PATH_RSYN}/lib/linux)
link_directories(${PATH_ISPD18}/lib/linux)
endif()

if(MACOSX)
link_directories(${PATH_RSYN}/lib/macosx)
link_directories(${PATH_ISPD18}/lib/macosx)
endif()

###########
# Targets #
###########

add_executable(rsyn ${SRC_FILES})

#######################
# Include Directories #
#######################

target_include_directories(rsyn PUBLIC "${PATH_RSYN}/src/rsyn/export")

target_include_directories(rsyn PUBLIC "${PATH_RSYN}/src")
target_include_directories(rsyn PUBLIC "${PATH_RSYN}/include")
target_include_directories(rsyn PUBLIC "${PATH_ISPD18}/src")
target_include_directories(rsyn PUBLIC "${PATH_ISPD18}/include")

##################
# Compiler Flags #
##################

target_compile_options(rsyn PUBLIC "-Wno-unused-result -Wno-format -Wno-return-type -Wno-unused-local-typedefs")

#################
# Linker Flags  #
#################

target_link_libraries(rsyn lef)
target_link_libraries(rsyn def)


# Boost   
target_include_directories(rsyn PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(rsyn ${Boost_LIBRARIES})
    
# PNG
target_include_directories(rsyn PUBLIC ${PNG_INCLUDE_DIR})
target_link_libraries(rsyn ${PNG_LIBRARY})

# Qt5 Widgets for linking
target_link_libraries(rsyn Qt5::Widgets)
target_link_libraries(rsyn Qt5::PrintSupport)
target_link_libraries(rsyn Qt5::OpenGL)
