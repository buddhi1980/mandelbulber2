cmake_minimum_required(VERSION 3.5)
project(mandelbulber2)

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
set(CMAKE_AUTOUIC ON)

# Find necessary qt libraries.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5UiTools REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Multimedia REQUIRED)

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
set(SOURCE_FILES ${srcHeader} ${srcDef} ${qtHeader} ${qtDef} ../qt/icons.qrc)
set(CMAKE_AUTOUIC_SEARCH_PATHS ../qt/)

# Find all ui files in qt/*.ui and add for header generation and linking.
file(GLOB qt_UI "../qt/*.ui")
#QT5_WRAP_UI(qt_UI_H ${qt_UI})

include_directories(..)
include_directories(../qt)

# Add source files to the build target.
add_executable(mandelbulber2 WIN32
	${SOURCE_FILES} ${ADDITIONAL_EXECUTABLE} ${qt_UI_H})

# Link qt libraries.
qt5_use_modules(mandelbulber2 Core Widgets Network Gui UiTools Test Multimedia)

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

# Check if we are using the Visual Studio compiler on windows,
# disable CRT warnings for libc functions marked as insecure
IF(WIN32 AND MSVC AND NOT USING_INTEL)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
ENDIF()

# Installation
# TODO: macOS

if(WIN32 OR APPLE)
	set(RELATIVE_DEFAULT ON)
else()
	set(RELATIVE_DEFAULT OFF)
endif()

option(INSTALL_RELATIVE "Make Mandelbulber search for data files in paths relative to the executable" ${RELATIVE_DEFAULT})

set(INSTALL_PATH bin)
set(SHARED_DIR "${CMAKE_INSTALL_PREFIX}/share/mandelbulber2")

if(INSTALL_RELATIVE)
	if(NOT UNIX)
		set(INSTALL_PATH .)
	endif()
	set(SHARED_DIR_DEF "share/mandelbulber2")
else()
	set(SHARED_DIR_DEF "${SHARED_DIR}")
endif()

if(UNIX AND NOT APPLE)
	install(FILES ../deploy/linux/mandelbulber2.desktop
			DESTINATION share/applications)
	install(FILES ../deploy/share/mandelbulber2/icons/mandelbulber.png
			DESTINATION share/icons/hicolor/256x256/apps/
			RENAME mandelbulber2.png)
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_PATH})

if(WIN32)
	install(FILES ../deploy/win64/mandelbulber2.ico
			DESTINATION .)
endif()

install(DIRECTORY ../deploy/share/mandelbulber2/ DESTINATION "${SHARED_DIR}")
install(DIRECTORY ../formula DESTINATION "${SHARED_DIR}")
install(DIRECTORY ../language DESTINATION "${SHARED_DIR}")
install(DIRECTORY ../opencl DESTINATION "${SHARED_DIR}")
