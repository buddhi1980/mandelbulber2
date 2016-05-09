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

# find required other libraries
find_package(PNG REQUIRED)
find_package(GSL REQUIRED)

# find optional other libraries
find_package(TIFF)
find_package(ECM NO_MODULE)

IF(ECM_FOUND)
    set(CMAKE_MODULE_PATH ${ECM_FIND_MODULE_DIR})
    find_package(OpenEXR)
ENDIF()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


# find all sources
file(GLOB srcHeader "mandelbulber2/src/*.h" "mandelbulber2/src/*.hpp")
file(GLOB srcDef "mandelbulber2/src/*.c" "mandelbulber2/src/*.cpp")
file(GLOB qtHeader "mandelbulber2/qt/*.h" "mandelbulber2/qt/*.hpp")
file(GLOB qtDef "mandelbulber2/qt/*.c" "mandelbulber2/qt/*.cpp")
set(SOURCE_FILES ${srcHeader} ${srcDef} ${qtHeader} ${qtDef})

# set additional files for compilation
set(ADDITIONAL_EXECUTABLE mandelbulber2/qt/icons.qrc)

# find all ui files in qt/*.ui and add for header generation and linking
file(GLOB qt_UI "mandelbulber2/qt/*.ui")
QT5_WRAP_UI(qt_UI_H ${qt_UI})

include_directories(mandelbulber2/qt)
include_directories(mandelbulber2/src)

# add source files to the build target
add_executable(mandelbulber2 ${SOURCE_FILES} ${ADDITIONAL_EXECUTABLE} ${qt_UI_H})

# link qt libraries
target_link_libraries(mandelbulber2 Qt5::Widgets)
target_link_libraries(mandelbulber2 Qt5::Core)
target_link_libraries(mandelbulber2 Qt5::Network)
target_link_libraries(mandelbulber2 Qt5::Gui)
target_link_libraries(mandelbulber2 Qt5::UiTools)

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

# generate proper GUI program on specified platform
IF(WIN32 AND MSVC) # Check if we are using the Visual Studio compiler on windows
	set_target_properties(${PROJECT_NAME} PROPERTIES
		WIN32_EXECUTABLE YES
		LINK_FLAGS "/SUBSYSTEM:WINDOWS"
	)
ENDIF()