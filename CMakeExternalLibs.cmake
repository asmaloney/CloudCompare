# ------------------------------------------------------------------------------
# Qt
# ------------------------------------------------------------------------------
## we will use cmake automoc feature
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set( QT5_ROOT_PATH CACHE PATH "Qt5 root directory (i.e. where the 'bin' folder lies)" )
if ( QT5_ROOT_PATH )

	list( APPEND CMAKE_PREFIX_PATH ${QT5_ROOT_PATH} )
	
endif()

# find qt5 components
# find_package(Qt5 COMPONENTS OpenGL Widgets Core Gui PrintSupport Concurrent REQUIRED)
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5PrintSupport)
find_package(Qt5Concurrent)
find_package(Qt5OpenGL)
if( APPLE )
	find_package(Qt5OpenGLExtensions)
endif()

# in the case no Qt5Config.cmake file could be found, cmake will explicitly ask the user for the QT5_DIR containing it!
# thus no need to keep additional variables and checks

if ( MSVC )
	# Where to find OpenGL libraries
	set(WINDOWS_OPENGL_LIBS "C:\\Program Files (x86)\\Windows Kits\\8.0\\Lib\\win8\\um\\x64" CACHE PATH "WindowsSDK libraries" )
	list( APPEND CMAKE_PREFIX_PATH ${WINDOWS_OPENGL_LIBS} )
endif()

get_target_property(QT5_LIB_LOCATION Qt5::Core LOCATION_${CMAKE_BUILD_TYPE})
get_filename_component(QT_BINARY_DIR ${QT5_LIB_LOCATION} DIRECTORY)
	
if ( NOT QT5_ROOT_PATH )
	set(QT5_ROOT_PATH ${QT_BINARY_DIR}/../)
endif()

include_directories(${Qt5OpenGL_INCLUDE_DIRS}
                    ${Qt5Widgets_INCLUDE_DIRS}
                    ${Qt5Core_INCLUDE_DIRS}
                    ${Qt5Gui_INCLUDE_DIRS}
                    ${Qt5Concurrent_INCLUDE_DIRS}
                    ${Qt5PrintSupport_INCLUDE_DIRS})

if( APPLE )
    include_directories(${Qt5OpenGL_INCLUDE_DIRS})
endif()
                

# ------------------------------------------------------------------------------
# OpenGL
# ------------------------------------------------------------------------------

#find_package( OpenGL REQUIRED )
#if( NOT OPENGL_FOUND )
#    message( SEND_ERROR "OpenGL required, but not found with 'find_package()'" )
#endif()

#include_directories(${OpenGL_INCLUDE_DIR})

# ------------------------------------------------------------------------------
# OpenMP
# ------------------------------------------------------------------------------
find_package(OpenMP)
if (OPENMP_FOUND)
	message("OpenMP found")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# ------------------------------------------------------------------------------
# Some macros for easily passing from qt4 to qt5 when we will be ready
# ------------------------------------------------------------------------------
macro(qt_wrap_ui)
    qt5_wrap_ui(${ARGN})
endmacro()


macro(qt_add_resources)
    qt5_add_resources(${ARGN})
endmacro()
