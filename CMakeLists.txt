cmake_minimum_required(VERSION 3.28)

set(PROJECT_VERSION 1.0.0)
project(FairyCam LANGUAGES CXX VERSION ${PROJECT_VERSION})

option (FAIRYCAM_ENABLE_TESTING "Create Testing targets" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#######
# dependencies

# Poco
find_package(Poco QUIET COMPONENTS Foundation Net)

if (NOT ${Poco_FOUND})
  message("Poco not found. Download Poco")
  include(cmake/CPM.cmake)
  CPMAddPackage(
    NAME Poco
    VERSION 1.13.3
    URL https://github.com/pocoproject/poco/archive/refs/tags/poco-1.13.3-release.tar.gz
    OPTIONS
    "BUILD_SHARED_LIBS OFF"
    "ENABLE_FOUNDATION ON"
    "ENABLE_NET ON"
    "ENABLE_NETSSL OFF"
    "ENABLE_XML OFF"
    "ENABLE_DATA_SQLITE OFF"
    "ENABLE_ENCODINGS OFF"
    "ENABLE_ENCODINGS_COMPILER OFF"
    "ENABLE_UTIL OFF"
    "ENABLE_JSON OFF"
    "ENABLE_MONGODB OFF"
    "ENABLE_REDIS OFF"
    "ENABLE_PROMETHEUS OFF"
    "ENABLE_JWT OFF"
    "ENABLE_CRYPTO OFF"
    "ENABLE_DATA OFF"
    "ENABLE_ACTIVERECORD OFF"
    "ENABLE_ACTIVERECORD_COMPILER OFF"
    "ENABLE_ZIP OFF"
    "ENABLE_PAGECOMPILER OFF"
    "ENABLE_PAGECOMPILER_FILE2PAGE OFF"
    )
endif()

# doctest
add_library(doctest INTERFACE)
target_sources(doctest PUBLIC
  FILE_SET HEADERS
    BASE_DIRS doctest
    FILES doctest/doctest.h
)

#######
add_subdirectory(src/)

# tests
if(FAIRYCAM_ENABLE_TESTING)
    include(CTest)
    enable_testing()

    add_subdirectory(tests)
    
    if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC" AND NOT APPLE)
        add_subdirectory(fuzzer_example)
    endif()
endif()
