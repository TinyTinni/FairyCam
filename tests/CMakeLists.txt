find_package(OpenCV REQUIRED)
find_package(Poco REQUIRED Foundation Net)


add_library(Tests_lib OBJECT
     FileCamera.cpp
     DirectoryCamera.cpp
     HttpCamera.cpp
     AnyCamera.cpp
     MemoryCamera.cpp
     DirectoryTriggerCamera.cpp
     chaos/BernoulliSequence.cpp
     chaos/ChaosCamera.cpp
     chaos/BinaryBlobSequence.cpp
     )

target_include_directories(Tests_lib PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(Tests_lib PUBLIC Poco::Foundation Poco::Net doctest FairyCam)

target_compile_definitions(Tests_lib PRIVATE "TEST_DATA=\"${CMAKE_CURRENT_SOURCE_DIR}/data/\"")
target_compile_features(Tests_lib PUBLIC cxx_std_20)

target_compile_options(Tests_lib PUBLIC
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -Wconversion -pedantic-errors -Wsign-conversion>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4>)

add_executable(Tests main.cpp)
target_link_libraries(Tests PUBLIC Tests_lib)
add_test(NAME Tests COMMAND Tests --order-by=rand)


if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
     target_link_libraries(Tests PUBLIC -fsanitize=address,undefined)

     add_executable(Tests_ThreadSanitizer main.cpp)     
     target_link_libraries(Tests_ThreadSanitizer PUBLIC Tests_lib -fsanitize=thread)
     add_test(NAME Tests_ThreadSanitizer COMMAND Tests_ThreadSanitizer --order-by=rand)
     set_tests_properties(Tests_ThreadSanitizer PROPERTIES ENVIRONMENT "TSAN_OPTIONS=suppressions=${CMAKE_CURRENT_SOURCE_DIR}/tsan_suppressions.txt")
endif()
