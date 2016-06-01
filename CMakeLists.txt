# CMake file for Quicksort speedtest project

project(quicksort)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(WITH_MALLOC_COUNT "Use malloc_count for memory profiling" OFF)

option(WITH_PAPI "Use PAPI library for performance counting" OFF)

# disallow in-source builds

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

# read .git directory (if it exists) and find git sha

include(GetGitRevisionDescription)
#get_git_head_revision(GIT_VERSION_REFSPEC GIT_VERSION_SHA1)
if(GIT_VERSION_REFSPEC)
  message(STATUS "Detected git refspec ${GIT_VERSION_REFSPEC} sha ${GIT_VERSION_SHA1}")
  add_definitions("-DGIT_VERSION_SHA1=\"${GIT_VERSION_SHA1}\"")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# Enable warnings

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")

# Save Assembler Code

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -S")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -funroll-loops")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
#string(REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

find_package(Boost COMPONENTS random REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# Use OpenMP

#include(FindOpenMP)
#
#if(OPENMP_FOUND)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#else()
#  message(FATAL_ERROR "OpenMP not found.")
#endif()

# Find PAPI library

if(WITH_PAPI)

  find_path(PAPI_INCLUDE_DIRS papi.h
    PATHS /usr/include/ /usr/local/include/)

  find_library(PAPI_LIBRARIES NAMES papi
    PATHS /usr/lib/ /usr/local/lib/)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(PAPI DEFAULT_MSG
    PAPI_INCLUDE_DIRS
    PAPI_LIBRARIES
    )

endif()

# descend into source
add_subdirectory(src)
#add_subdirectory(java-src)
