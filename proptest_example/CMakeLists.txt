
find_package(OpenCV REQUIRED)

add_executable(Example_Proptest
    main.cpp
    system.cpp
)

target_include_directories(Example_Proptest PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_compile_features(Example_Proptest PRIVATE cxx_std_20)


if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
   target_link_libraries(Example_Proptest PUBLIC -fsanitize=address,undefined)
endif()

target_link_libraries(Example_Proptest PRIVATE FairyCam)
