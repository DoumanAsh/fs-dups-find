###########################
# Init
##########################
cmake_minimum_required(VERSION 3.9)
set(MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.cmake/Modules")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MODULE_DIR})
include(LazyUtils)
include(msvc)
include(gcc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

project(fs-dups-find LANGUAGES CXX)

msvc_remove_warning_level()
msvc_set_flags_if("/utf-8")

include(conan.cmake)
###########################
# Output dir
##########################
set_output_dir("${CMAKE_BINARY_DIR}/dist")

# Max compile warnings for own code only
msvc_set_flags_if("/W4 /std:c++latest /permissive-")
gcc_set_flags_if("-Wall -Wextra")

###########################
# Source code
##########################
add_subdirectory("src/")

###########################
# Linter
##########################
option(CLANG_TIDY "Enable Clang tidy checks" OFF)
if (CLANG_TIDY)
    include(ClangTidy)
    enable_clang_tidy_for(${PROJECT_NAME})
endif()
