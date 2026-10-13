###################################################################################################
#####                                                                                         #####
#####               LiteFX.Core - Defines the common includes for the project.                #####
#####                                                                                         #####
###################################################################################################

PROJECT(LiteFX.Core VERSION ${LITEFX_VERSION} LANGUAGES CXX)
MESSAGE(STATUS "Initializing: ${PROJECT_NAME}...")

CONFIGURE_FILE("config.tmpl" "${CMAKE_CURRENT_BINARY_DIR}/include/litefx/config.h")
CONFIGURE_FILE("version.tmpl" "${CMAKE_CURRENT_BINARY_DIR}/include/litefx/version.h")

# Collect header & source files.
SET(CORE_HEADERS
    "include/litefx/containers.hpp"
    "include/litefx/core.h"
    "include/litefx/string.hpp"
    "include/litefx/traits.hpp"
    "include/litefx/exceptions.hpp"
    "include/litefx/litefx.h"
)

SET(CORE_GENERATED_HEADERS
    "${CMAKE_CURRENT_BINARY_DIR}/include/litefx/config.h"
    "${CMAKE_CURRENT_BINARY_DIR}/include/litefx/version.h"
)

SET(CORE_SOURCES
    "src/core.cpp"
)

# Add static library project.
LITEFX_ADD_LIBRARY(${PROJECT_NAME} STATIC
    EXPORT_NAME Core
    SOURCES ${CORE_SOURCES}
    HEADERS ${CORE_HEADERS} ${CORE_GENERATED_HEADERS}
    HEADER_BASE_DIRS "include" "${CMAKE_CURRENT_BINARY_DIR}/include"
    NATVIS ".natvis"
)

# Create source groups for better code organization.
SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CORE_HEADERS} ${CORE_SOURCES})

# Setup project properties.
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
    FOLDER "SDK"
)

# Setup target include directories.
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "src/")

# Link experimental libraries until <stacktrace> support is fully implemented by GCC.
IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC -lstdc++exp)
ENDIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

# Setup installer.
INSTALL(TARGETS ${PROJECT_NAME} EXPORT LiteFX FILE_SET HEADERS)
