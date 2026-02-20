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
    VERSION 1.15.0
    URL https://github.com/pocoproject/poco/releases/download/poco-1.15.0-release/poco-1.15.0.tar.gz
    OPTIONS
    "BUILD_SHARED_LIBS OFF"
    "POCO_STATIC ON"
    "ENABLE_FOUNDATION ON"
    "ENABLE_NET ON"
    "POCO_MINIMAL_BUILD ON"
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
