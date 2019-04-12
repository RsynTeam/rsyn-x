# Rsyn
# CMake Examples: https://github.com/ttroy50/cmake-examples

#########
# Setup #
#########

# Specify the minimum version for CMake
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Message
MESSAGE(STATUS "Rsyn")
MESSAGE(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

# Find includes in the build directories
SET(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
SET(CMAKE_AUTOMOC ON)

# Project's name
PROJECT(rsyn)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        SET(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        SET(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        SET(MACOSX TRUE)
endif()

# SET the output folder where your program will be created
SET(CMAKE_BINARY_DIR bin)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

SET(CMAKE_CXX_STANDARD 11)

SET(PATH_RSYN ${CMAKE_CURRENT_SOURCE_DIR}/src)

SET(PATH_QT ${CMAKE_CURRENT_SOURCE_DIR}/src/gui)

###################
# Warnings/Errors #
###################

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Werror=return-type")

######################
# Check Dependencies #
######################

FIND_PACKAGE(Boost COMPONENTS system filesystem program_options REQUIRED)
FIND_PACKAGE(PNG REQUIRED)

# OpenMP (work around)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5PrintSupport REQUIRED)
FIND_PACKAGE(Qt5OpenGL REQUIRED)

###############
# Source Code #
###############

FILE(GLOB_RECURSE SRC_FILES
        ${PATH_RSYN}/*.cpp
        ${PATH_RSYN}/*.cc
        ${PATH_RSYN}/*.c)

#####################
# Qt Pre-Processing #
#####################

FILE(GLOB_RECURSE UI_FILES
        ${PATH_QT}/ui/*.ui)

FILE(GLOB_RECURSE QRC_FILES
        ${PATH_QT}/resource/*.qrc)

# Generate code from ui files
qt5_wrap_ui(UI_HEADERS ${UI_FILES})

# Generate rules for building source files from the resources
qt5_add_resources(QRCS ${QRC_FILES})

# Tell CMake to create the executable
LIST(APPEND SRC_FILES ${UI_HEADERS})
LIST(APPEND SRC_FILES ${QRCS})
LIST(APPEND SRC_FILES ${GUI_FILES})

#message(STATUS ${SRC_FILES})

#################
# Library Paths #
#################

if(LINUX)
        LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/linux)
endif()

if(MACOSX)
        LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib/macosx)
endif()

###########
# Targets #
###########

ADD_EXECUTABLE(rsyn ${SRC_FILES})

#######################
# Include Directories #
#######################

TARGET_INCLUDE_DIRECTORIES(rsyn PUBLIC "./3rdparty")
TARGET_INCLUDE_DIRECTORIES(rsyn PUBLIC "./db")
TARGET_INCLUDE_DIRECTORIES(rsyn PUBLIC "./db/3rdparty")
TARGET_INCLUDE_DIRECTORIES(rsyn PUBLIC "./include")
TARGET_INCLUDE_DIRECTORIES(rsyn PUBLIC "./include/export")
TARGET_INCLUDE_DIRECTORIES(rsyn PUBLIC "./src")
TARGET_INCLUDE_DIRECTORIES(rsyn PUBLIC "./src/contests")

##################
# Compiler Flags #
##################

TARGET_COMPILE_OPTIONS(rsyn PUBLIC "-O3")
TARGET_COMPILE_OPTIONS(rsyn PUBLIC "-Wno-unused-result -Wno-format -Wno-return-type -Wno-unused-local-typedefs")

#################
# Linker Flags  #
#################

TARGET_LINK_LIBRARIES(rsyn lef)
TARGET_LINK_LIBRARIES(rsyn def)
TARGET_LINK_LIBRARIES(rsyn db)
TARGET_LINK_LIBRARIES(rsyn flute)

# Boost
TARGET_INCLUDE_DIRECTORIES(rsyn PUBLIC ${Boost_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(rsyn ${Boost_LIBRARIES})

# PNG
TARGET_INCLUDE_DIRECTORIES(rsyn PUBLIC ${PNG_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(rsyn ${PNG_LIBRARY})

# Qt5 Widgets for linking
TARGET_LINK_LIBRARIES(rsyn Qt5::Widgets)
TARGET_LINK_LIBRARIES(rsyn Qt5::PrintSupport)
TARGET_LINK_LIBRARIES(rsyn Qt5::OpenGL)
