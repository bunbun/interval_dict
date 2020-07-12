# Copyright Leo Goodstadt 2020
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.16)

# This include defines standard install locations.
# https://cmake.org/cmake/help/v3.11/module/GNUInstallDirs.html
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -fcoroutines-ts")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")

project(interval_dict VERSION 0.0.1 LANGUAGES CXX DESCRIPTION "An interval dict library.")

# This include provides some helper functions that simplify packaging.
# https://cmake.org/cmake/help/v3.11/module/CMakePackageConfigHelpers.html
include(CMakePackageConfigHelpers)

# If no installation prefix is given manually, install locally.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE STRING
            "The install location"
            FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

# see https://cmake.org/cmake/help/latest/module/FindBoost.html
find_package(Boost 1.62 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

#
#   External projects
#       range-v3
#
include_directories(extern/range-v3)
add_library(range_v3 INTERFACE IMPORTED)
set_target_properties(range_v3 PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${interval_dict_SOURCE_DIR}/extern/range-v3/include)
#       cppcoro
include_directories(extern/cppcoro)
add_library(cppcoro INTERFACE IMPORTED)
set_target_properties(cppcoro PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${interval_dict_SOURCE_DIR}/extern/cppcoro/include)
#       catch2
include_directories(extern/catch2)


#
#   Set up library
#
# Only public header files go here.
set(HEADER_FILES
        include/interval_dict/intervaldicticl.h
        include/interval_dict/interval_traits.h
        include/interval_dict/bi_intervaldicticl.h
        include/interval_dict/icl_interval_map_adaptor.h
        include/interval_dict/intervaldict.h
        include/interval_dict/intervaldict_forward.h)
set(SOURCE_FILES src/intervaldicticl.cpp)
add_library(interval_dict
        ${HEADER_FILES}
        ${SOURCE_FILES})



# We also add an alias definition so that we shadown
# the export namespace when using add_subdirectory() instead.
add_library(interval_dict::interval_dict ALIAS interval_dict)

# By prefixing the installation paths with our name and version
# we can have multiple versions installed at the same time.
set(PROJECT_PREFIX interval_dict-${interval_dict_VERSION})

# Defines the headers for this library.
# https://cmake.org/cmake/help/v3.11/command/target_include_directories.html
target_include_directories(interval_dict
        PUBLIC
        # Used when building the library:
        $<BUILD_INTERFACE:${interval_dict_SOURCE_DIR}/include>
        # Used when installing the library:
        $<INSTALL_INTERFACE:include/${PROJECT_PREFIX}/interval_dict>
        PRIVATE
        # Used only when building the library:
        src include/interval_dict)


#
#   external dependencies
#
target_link_libraries(interval_dict PUBLIC range_v3 cppcoro)


# Here we configure variables specifying the target directories.
set(INSTALL_RUNTIME_DIR ${CMAKE_INSTALL_BINDIR})
set(INSTALL_CONFIG_DIR  ${CMAKE_INSTALL_LIBDIR}/${PROJECT_PREFIX}/cmake)
set(INSTALL_LIBRARY_DIR ${CMAKE_INSTALL_LIBDIR}/${PROJECT_PREFIX}/${CMAKE_BUILD_TYPE})
set(INSTALL_ARCHIVE_DIR ${CMAKE_INSTALL_LIBDIR}/${PROJECT_PREFIX}/${CMAKE_BUILD_TYPE}/static)
set(INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_PREFIX}/interval_dict)

# Define the configuration file locations.
set(PROJECT_CONFIG_VERSION_FILE "${PROJECT_BINARY_DIR}/interval_dict-config-version.cmake")
set(PROJECT_CONFIG_FILE         "${PROJECT_BINARY_DIR}/interval_dict-config.cmake")

#configure_package_config_file(cmake/interval_dict-config.cmake.in
#        ${PROJECT_CONFIG_FILE}
#        INSTALL_DESTINATION ${INSTALL_CONFIG_DIR})

# This auto-generates a version file.
write_basic_package_version_file(
        ${PROJECT_CONFIG_VERSION_FILE}
        COMPATIBILITY SameMajorVersion)

# The INSTALL(TARGET) command specifies where to install specific targets.
# By specifying EXPORT here, we add these targets to an export group.
install(TARGETS interval_dict
        EXPORT interval_dict-targets
        RUNTIME DESTINATION ${INSTALL_RUNTIME_DIR}
        LIBRARY DESTINATION ${INSTALL_LIBRARY_DIR}
        ARCHIVE DESTINATION ${INSTALL_ARCHIVE_DIR})

# The INSTALL(FILES) command specifies where to install specific files, such as headers.
install(FILES ${HEADER_FILES}
        DESTINATION ${INSTALL_INCLUDE_DIR})

# The INSTALL(FILES) command specifies where to install specific files, such as headers.
install(FILES
        ${PROJECT_CONFIG_VERSION_FILE}
        ${PROJECT_CONFIG_FILE}
        DESTINATION ${INSTALL_CONFIG_DIR})

# The INSTALL(EXPORT) command installs a previously defined export group to a script.
install(EXPORT interval_dict-targets
        FILE interval_dict-targets.cmake
        NAMESPACE interval_dict::
        DESTINATION ${INSTALL_CONFIG_DIR})

export(EXPORT interval_dict-targets
        FILE ${CMAKE_CURRENT_BINARY_DIR}/interval_dict-targets.cmake
        NAMESPACE interval_dict::)

# Register the package in the user's package registry.
# https://cmake.org/cmake/help/v3.11/command/export.html
export(PACKAGE interval_dict)


#add_library(interval_dict SHARED STATIC ${SOURCE_FILES})
#add_subdirectory(boost_tests)
add_subdirectory(tests)

install(TARGETS interval_dict DESTINATION ${INTERVAL_DICT_INSTALL_LIB_DIR})

