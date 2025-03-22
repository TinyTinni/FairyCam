
find_package(OpenCV REQUIRED)

set(FUZZ_RUNTIME
    1
    CACHE STRING "Number of seconds to run fuzz tests during ctest run")


add_executable(Example_Fuzzer
    main.cpp
)

target_include_directories(Example_Fuzzer PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_compile_features(Example_Fuzzer PRIVATE cxx_std_20)

target_link_libraries(Example_Fuzzer PRIVATE FairyCam)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    target_link_libraries(Example_Fuzzer PUBLIC -coverage -fsanitize=fuzzer,address,undefined)
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
    # disable annotations or possible annotation mismatch with librapidcheck. It is not really required by the vdf parser
    target_compile_definitions(Example_Fuzzer PRIVATE "_DISABLE_VECTOR_ANNOTATION" "_DISABLE_STRING_ANNOTATION") 
    target_compile_options(Example_Fuzzer PRIVATE /fsanitize=fuzzer /fsanitize=address)
endif()

add_test(NAME Example_Fuzzer COMMAND Example_Fuzzer -max_total_time=${FUZZ_RUNTIME} -timeout=${FUZZ_RUNTIME})
