cmake_minimum_required(VERSION 3.16)
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

# Qt6 requires C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find includes in corresponding build directories.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set module find scripts
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/modules/")
message(STATUS "${PROJECT_SOURCE_DIR}/modules/")

# Qt intermediate build config and setup.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find necessary qt libraries (Qt6; CMake 4.x no longer supports Qt5 config files).
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network UiTools Test Multimedia Qml Concurrent OpenGLWidgets)

# Find other required libraries.
find_package(PNG REQUIRED)
find_package(GSL REQUIRED)
find_package(LZO REQUIRED)

# Find other optional libraries.
find_package(TIFF)
find_package(JPEG)
find_package(ZLIB)
find_package(OpenCL)
find_package(PkgConfig)

# Extra CMake Modules package required for OpenEXR.
IF(PKG_CONFIG_FOUND)
	pkg_check_modules(OpenEXR OpenEXR)
	# JPEG XL and libsndfile support (same feature macros as the qmake build)
	pkg_check_modules(LIBJXL libjxl)
	pkg_check_modules(LIBJXL_THREADS libjxl_threads)
	pkg_check_modules(SNDFILE sndfile)
ENDIF()

IF(WIN32 AND NOT USING_INTEL)
	# WIN32 MSVC
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GL /Ox")
ELSEIF(NOT USING_INTEL)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -fopenmp")
ENDIF()

# Find all sources.
file(GLOB srcHeader "../src/*.h" "../src/*.hpp")
file(GLOB srcDef "../src/*.c" "../src/*.cpp")
file(GLOB qtHeader "../qt/*.h" "../qt/*.hpp")
file(GLOB qtDef "../qt/*.c" "../qt/*.cpp")
file(GLOB formulaHeader "../formula/definition/*.h" "../formula/definition/*.hpp")
file(GLOB formulaDef "../formula/definition/*.c" "../formula/definition/*.cpp")
file(GLOB stbHeaders "../third-part/stb/*.h")

set(SOURCE_FILES ${srcHeader} ${srcDef} ${qtHeader} ${qtDef} ${formulaHeader} ${formulaDef} ${stbHeaders} ../qt/icons.qrc)
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
target_link_libraries(mandelbulber2
	Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::UiTools
	Qt6::Test Qt6::Multimedia Qt6::Qml Qt6::Concurrent Qt6::OpenGLWidgets)

# Link other required libraries.
include_directories(${PNG_INCLUDE_DIR} ${GSL_INCLUDE_DIRS} ${LZO_INCLUDE_DIR})
target_link_libraries(mandelbulber2 ${PNG_LIBRARY} ${GSL_LIBRARIES} ${LZO_LIBRARIES})

# Link other optional libraries and set corresponding flag definitions .
IF(TIFF_FOUND)
	include_directories(${TIFF_INCLUDE_DIR})
	target_link_libraries(mandelbulber2 ${TIFF_LIBRARY})
	add_definitions(-DUSE_TIFF)
ENDIF()

IF(LIBJXL_FOUND)
	include_directories(${LIBJXL_INCLUDE_DIRS})
	target_link_libraries(mandelbulber2 ${LIBJXL_LIBRARIES})
	IF(LIBJXL_THREADS_FOUND)
		target_link_libraries(mandelbulber2 ${LIBJXL_THREADS_LIBRARIES})
	ENDIF()
	add_definitions(-DUSE_JXL)
ENDIF()

IF(SNDFILE_FOUND)
	include_directories(${SNDFILE_INCLUDE_DIRS})
	target_link_libraries(mandelbulber2 ${SNDFILE_LIBRARIES})
	add_definitions(-DUSE_SNDFILE)
ENDIF()

IF(OpenEXR_FOUND)
	include_directories(${OpenEXR_INCLUDE_DIRS})
	target_link_libraries(mandelbulber2 ${OpenEXR_LIBRARIES})
	add_definitions(-DUSE_EXR)
ENDIF()

IF(USE_GAMEPAD)
	find_package(Qt6Gamepad)
	IF(Qt6Gamepad_FOUND)
		target_link_libraries(mandelbulber2 Qt6::Gamepad)
		add_definitions(-DUSE_GAMEPAD=1)
	ELSE()
		MESSAGE(WARNING "USE_GAMEPAD requested but Qt6 Gamepad module was not found")
	ENDIF()
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

# Documentation: legacy PDF plus the generated html manual (index.html + css + img).
# On unix it is looked up in ${CMAKE_INSTALL_PREFIX}/share/doc/mandelbulber2
if(UNIX AND NOT APPLE)
	set(DOC_DIR "${CMAKE_INSTALL_PREFIX}/share/doc/mandelbulber2")
else()
	set(DOC_DIR "doc")
endif()
install(FILES ../deploy/doc/Mandelbulber_Manual.pdf
		../deploy/doc/index.html
		DESTINATION "${DOC_DIR}")
install(DIRECTORY ../deploy/doc/css ../deploy/doc/img
		DESTINATION "${DOC_DIR}")
install(FILES ../deploy/README ../deploy/NEWS ../deploy/COPYING
		DESTINATION "${DOC_DIR}")
