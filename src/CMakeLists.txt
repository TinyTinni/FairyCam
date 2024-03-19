find_package(OpenCV REQUIRED)
find_package(Poco REQUIRED Foundation Net)

set (HEADERS
    "AnyCamera.hpp"
    "MemoryCamera.hpp"
    "FileCamera.hpp"
    "HttpCamera.hpp"
    "DirectoryCamera.hpp"
    "DirectoryTriggerCamera.hpp"
    "IsAnyCamera.hpp"

    "chaos/ChaosCamera.hpp"
    "chaos/Exceptions.hpp"
    "chaos/Sequence.hpp"
    "chaos/BernoulliSequence.hpp"
    "chaos/BinaryBlobSequence.hpp"
    "chaos/NoErrorSequence.hpp"
)

add_library(FairyCam STATIC
    HttpCamera.cpp
    DirectoryTriggerCamera.cpp
    FileCamera.cpp
    MemoryCamera.cpp
    ${HEADERS}
)

target_sources(FairyCam PUBLIC FILE_SET HEADERS
    FILES
    ${HEADERS}
)

target_include_directories(FairyCam PUBLIC ${OpenCV_INCLUDE_DIRS} )
target_link_libraries(FairyCam PUBLIC ${OpenCV_LIBS})
target_link_libraries(FairyCam PRIVATE  Poco::Foundation Poco::Net)
target_compile_features(FairyCam PUBLIC cxx_std_20)

include(GNUInstallDirs)
install(TARGETS FairyCam
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_target_properties(FairyCam PROPERTIES PUBLIC_HEADER "${HEADERS}")

include(GNUInstallDirs)
install(TARGETS FairyCam
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

include(CMakePackageConfigHelpers)


write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION cmake
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION cmake
)
