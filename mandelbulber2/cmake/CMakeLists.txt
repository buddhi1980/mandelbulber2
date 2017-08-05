## CMAKE BUILD SYSTEM SPECIFICATION ##
cmake_minimum_required(VERSION 3.5)
project(mandelbulber2)

# The version number.
set (MB_VERSION_MAJOR 2)
set (MB_VERSION_MINOR 10)

# Detect Compilation System.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(USING_CLANG TRUE)
	message(STATUS "Clang Compiler Detected")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(USING_GNU TRUE)
	message(STATUS "GNU Compiler Detected")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	set(USING_INTEL TRUE)
	message(STATUS "Intel Compiler Detected")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(USING_MSVC TRUE)
	message(STATUS "MSVC Compiler Detected")
endif()

# Find includes in corresponding build directories.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set module find scripts
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/")
message(STATUS "${PROJECT_SOURCE_DIR}/modules/")

# Qt intermediate build config and setup.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

# Find necessary qt libraries.
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Gui)
find_package(Qt5UiTools)
find_package(Qt5Test)
find_package(Qt5Multimedia)

# Find other required libraries.
find_package(PNG REQUIRED)
find_package(GSL REQUIRED)
find_package(LZO REQUIRED)

# Find other optional libraries.
find_package(TIFF)
find_package(JPEG)
find_package(ZLIB)
find_package(ECM NO_MODULE)
find_package(OpenCL)

# Extra CMake Modules package required for OpenEXR.
IF(ECM_FOUND)
    set(CMAKE_MODULE_PATH ${ECM_FIND_MODULE_DIR})
    find_package(OpenEXR)
ENDIF()


IF(WIN32 AND NOT USING_INTEL)
	# WIN32 MSVC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GL /Ox")
ELSEIF(NOT USING_INTEL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -fopenmp -std=c++11")
ENDIF()

# Find all sources.
file(GLOB srcHeader "../src/*.h" "../src/*.hpp")
file(GLOB srcDef "../src/*.c" "../src/*.cpp")
file(GLOB qtHeader "../qt/*.h" "../qt/*.hpp")
file(GLOB qtDef "../qt/*.c" "../qt/*.cpp")
set(SOURCE_FILES ${srcHeader} ${srcDef} ${qtHeader} ${qtDef})

# Set additional files for compilation.
set(ADDITIONAL_EXECUTABLE ../qt/icons.qrc)

# Find all ui files in qt/*.ui and add for header generation and linking.
file(GLOB qt_UI "../qt/*.ui")
QT5_WRAP_UI(qt_UI_H ${qt_UI})

include_directories(..)
include_directories(../qt)
include_directories(../src)

# Add source files to the build target.
add_executable(mandelbulber2 ${SOURCE_FILES} ${ADDITIONAL_EXECUTABLE} ${qt_UI_H})

# Link qt libraries.
qt5_use_modules(mandelbulber2 Core Widgets Network
    Gui UiTools Test Multimedia)

# Link other required libraries.
include_directories(${PNG_INCLUDE_DIR} ${GSL_INCLUDE_DIRS} ${LZO_INCLUDE_DIR})
target_link_libraries(mandelbulber2 ${PNG_LIBRARY} ${GSL_LIBRARIES} ${LZO_LIBRARIES})

# Link other optional libraries and set corresponding flag definitions .
IF(TIFF_FOUND)
    include_directories(${TIFF_INCLUDE_DIR})
    target_link_libraries(mandelbulber2 ${TIFF_LIBRARY})
    add_definitions(-DUSE_TIFF)
ENDIF()

IF(OpenEXR_FOUND)
    include_directories(${OpenEXR_INCLUDE_DIRS})
    target_link_libraries(mandelbulber2 ${OpenEXR_LIBRARIES})
    add_definitions(-DUSE_EXR)
ENDIF()

IF(USE_GAMEPAD)
    find_package(Qt5Gamepad)
    target_link_libraries(mandelbulber2 Qt5::Gamepad)
    add_definitions(-DUSE_GAMEPAD=1)
ENDIF()

IF(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(mandelbulber2 ${ZLIB_LIBRARIES})
ENDIF()

IF(JPEG_FOUND)
    include_directories(${JPEG_INCLUDE_DIR})
    target_link_libraries(mandelbulber2 ${JPEG_LIBRARY})
ENDIF()

IF(OpenCL_FOUND)
    MESSAGE(STATUS "Found OpenCL ${OpenCL_VERSION_STRING}")
    include_directories(${OpenCL_INCLUDE_DIRS})
    target_link_libraries(mandelbulber2 ${OpenCL_LIBRARY})
    add_definitions(-DUSE_OPENCL=1)
ELSE()
    MESSAGE("OpenCL not found!")
ENDIF()

IF(USE_OFFLOAD)
    add_definitions(-DUSE_OFFLOAD=1)
ENDIF()

# Generate proper GUI program on specified platform.
# Check if we are using the Visual Studio compiler on windows.
IF(WIN32 AND MSVC AND NOT USING_INTEL)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		WIN32_EXECUTABLE YES
		LINK_FLAGS "/SUBSYSTEM:WINDOWS"
	)
ENDIF()

##########################
# Installation
##########################

install(TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
)

