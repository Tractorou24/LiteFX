###################################################################################################
#####                                                                                         #####
#####               LiteFX.Core - Defines the common includes for the project.                #####
#####                                                                                         #####
###################################################################################################

PROJECT(LiteFX.Core VERSION ${LITEFX_VERSION} LANGUAGES CXX)
MESSAGE(STATUS "Initializing: ${PROJECT_NAME}...")

CONFIGURE_FILE("config.tmpl" "${CMAKE_CURRENT_BINARY_DIR}/include/litefx/config.h")
CONFIGURE_FILE("version.tmpl" "${CMAKE_CURRENT_BINARY_DIR}/include/litefx/version.h")
CONFIGURE_FILE("core.tmpl" "${CMAKE_CURRENT_BINARY_DIR}/include/litefx/core.h")

# Collect header & source files.
SET(CORE_HEADERS
    "include/litefx/containers.hpp"
    "include/litefx/generator.hpp"
    "include/litefx/string.hpp"
    "include/litefx/traits.hpp"
    "include/litefx/exceptions.hpp"
    "include/litefx/litefx.h"
)

SET(CORE_SOURCES
    "src/core.cpp"
)

ADD_LIBRARY(${PROJECT_NAME} STATIC
    ${CORE_HEADERS}
    ${CORE_SOURCES}
    "${CMAKE_CURRENT_BINARY_DIR}/include/litefx/config.h"
    "${CMAKE_CURRENT_BINARY_DIR}/include/litefx/version.h"
    "${CMAKE_CURRENT_BINARY_DIR}/include/litefx/core.h"

    ".natvis"
)

# Create source groups for better code organization.
SOURCE_GROUP(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CORE_HEADERS} ${CORE_SOURCES})

# Setup project properties. Relevant properties:
# - FOLDER: IDE folder (if supported).
# - VERSION/SOVERSION: The current build version.
# - DEFINE_SYMBOL: Disables the "_EXPORTS" define, which would conflict with the precompiled header.
# - PUBLIC_HEADER: Specifies a list of headers to copy during install.
# Do not export
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
    FOLDER "SDK"
    VERSION ${LITEFX_VERSION}
    SOVERSION ${LITEFX_YEAR}
    DEFINE_SYMBOL ""
    MAP_IMPORTED_CONFIG_RELWITHDEBINFO "Debug"
    MAP_IMPORTED_CONFIG_MINSIZEREL "Release"
    PUBLIC_HEADER "${CORE_HEADERS};${CMAKE_CURRENT_BINARY_DIR}/include/litefx/config.h;${CMAKE_CURRENT_BINARY_DIR}/include/litefx/version.h;${CMAKE_CURRENT_BINARY_DIR}/include/litefx/core.h"
)

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME}
    PRIVATE 
        "src/"
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)

# Pre-compile core header.
IF(LITEFX_BUILD_PRECOMPILED_HEADERS)
    TARGET_PRECOMPILE_HEADERS(${PROJECT_NAME} PRIVATE <litefx/core.h>)
ENDIF(LITEFX_BUILD_PRECOMPILED_HEADERS)

# Setup installer.
INSTALL(TARGETS ${PROJECT_NAME} EXPORT EXPORT LiteFX
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBRARY_DIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBRARY_DIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINARY_DIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDE_DIR}/litefx/
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDE_DIR}
)

# Export config.
INSTALL(EXPORT LiteFX DESTINATION ${CMAKE_INSTALL_EXPORT_DIR})
EXPORT(TARGETS ${PROJECT_NAME} FILE LiteFXCoreConfig.cmake)