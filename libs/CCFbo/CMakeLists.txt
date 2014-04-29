cmake_minimum_required(VERSION 2.8)

# Eliminate a warning when building in Windows that relates
# to static linking of Qt executables to qtmain.lib.
# This policy was introduced in CMake version 2.8.11.
# CMake version 2.8.11.2 warns when the policy is not set
# and uses OLD behavior.
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 OLD)
endif()

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

if( ${USE_GLEW_LIB} )
    include_directories( ${GLEW_LIB_SOURCE_DIR}/include )
else()
    # include the Qt 5 headers for OpenGL
    include_directories( ${EXTERNAL_LIBS_INCLUDE_DIR} )
endif()

project( CC_FBO_LIB )

file( GLOB header_list include/*.h )
file( GLOB source_list src/*.cpp )

add_library( ${PROJECT_NAME} STATIC ${header_list} ${source_list} )

if( ${USE_GLEW_LIB} )
    target_link_libraries( ${PROJECT_NAME} GLEW_LIB )
endif()

# Default preprocessors
set_default_cc_preproc( ${PROJECT_NAME} )

# Add custom prepocessor definitions
if( ${USE_GLEW_LIB} )
    set_property( TARGET ${PROJECT_NAME} APPEND PROPERTY COMPILE_DEFINITIONS USE_GLEW GLEW_STATIC )
endif()
