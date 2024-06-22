
find_package(OpenCV REQUIRED)

set(FUZZ_RUNTIME
    1
    CACHE STRING "Number of seconds to run fuzz tests during ctest run")


add_executable(Example_Fuzzer
    main.cpp
)

target_include_directories(Example_Fuzzer PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(Example_Fuzzer PRIVATE FairyCam)

target_compile_features(Example_Fuzzer PUBLIC cxx_std_20)

target_link_libraries(Example_Fuzzer PRIVATE -coverage -fsanitize=fuzzer)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_compile_options(Example_Fuzzer PRIVATE -fsanitize=fuzzer)
#    target_link_libraries(Example_Fuzzer PUBLIC -fsanitize=address,undefined)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    target_compile_options(Example_Fuzzer PRIVATE /fsanitize=fuzzer)
    target_compile_definitions(Example_Fuzzer PRIVATE "-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS")
endif()

add_test(NAME Example_Fuzzer COMMAND Example_Fuzzer -max_total_time=${FUZZ_RUNTIME} -timeout=${FUZZ_RUNTIME})
