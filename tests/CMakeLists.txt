find_package(OpenCV REQUIRED)
find_package(Poco REQUIRED Foundation Net)

add_executable(Tests
    main.cpp
    FileCamera.cpp
    DirectoryCamera.cpp
    HttpCamera.cpp
    AnyCamera.cpp
    DirectoryTriggerCamera.cpp
    chaos/BernoulliSequence.cpp
    chaos/ChaosCamera.cpp
    MemoryCamera.cpp
    chaos/BinaryBlobSequence.cpp
)

target_include_directories(Tests PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(Tests PRIVATE Poco::Foundation Poco::Net doctest FairyCam)

target_compile_definitions(Tests PRIVATE "TEST_DATA=\"${CMAKE_CURRENT_SOURCE_DIR}/data/\"")
target_compile_features(Tests PUBLIC cxx_std_20)

target_compile_options(Tests PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -Wconversion -pedantic-errors -Wsign-conversion>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
   target_link_libraries(Tests PUBLIC -fsanitize=address,undefined)
endif()

add_test(NAME Tests COMMAND Tests)
