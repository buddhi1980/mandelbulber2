cmake_minimum_required(VERSION 3.5)
project(mandelbulber2)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# qt intermediate build config and setup
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

# find necessary qt libraries
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Network)
find_package(Qt5Gui)
find_package(Qt5UiTools)
find_package(Qt5Test)
find_package(Qt5Multimedia)

# find required other libraries
find_package(PNG REQUIRED)
find_package(GSL REQUIRED)

# find optional other libraries
find_package(TIFF)
find_package(JPEG)
find_package(ZLIB)
find_package(ECM NO_MODULE)

IF(ECM_FOUND)
    set(CMAKE_MODULE_PATH ${ECM_FIND_MODULE_DIR})
    find_package(OpenEXR)
ENDIF()

IF(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GL /Ox")
ELSE()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math -fopenmp -std=c++11")
ENDIF()

# find all sources
file(GLOB srcHeader "../src/*.h" "../src/*.hpp")
file(GLOB srcDef "../src/*.c" "../src/*.cpp")
file(GLOB qtHeader "../qt/*.h" "../qt/*.hpp")
file(GLOB qtDef "../qt/*.c" "../qt/*.cpp")
set(SOURCE_FILES ${srcHeader} ${srcDef} ${qtHeader} ${qtDef})

# set additional files for compilation
set(ADDITIONAL_EXECUTABLE ../qt/icons.qrc)

# find all ui files in qt/*.ui and add for header generation and linking
file(GLOB qt_UI "../qt/*.ui")
QT5_WRAP_UI(qt_UI_H ${qt_UI})

include_directories(../qt)
include_directories(../src)

# add source files to the build target
add_executable(mandelbulber2 ${SOURCE_FILES} ${ADDITIONAL_EXECUTABLE} ${qt_UI_H})

# link qt libraries
qt5_use_modules(mandelbulber2 Core Widgets Network
    Gui UiTools Test Multimedia)

# link required other libraries
include_directories(${PNG_INCLUDE_DIR} ${GSL_INCLUDE_DIRS})
target_link_libraries(mandelbulber2 ${PNG_LIBRARY} ${GSL_LIBRARIES})

# link optional other libraries and set corresponding define flag
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

# generate proper GUI program on specified platform
IF(WIN32 AND MSVC) # Check if we are using the Visual Studio compiler on windows
	add_definitions(-D_CRT_SECURE_NO_WARNINGS=1)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		WIN32_EXECUTABLE YES
		LINK_FLAGS "/SUBSYSTEM:WINDOWS"
	)
ENDIF()
