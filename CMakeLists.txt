cmake_minimum_required(VERSION 3.22)

project(proyectoI_test)

set(CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage")

enable_testing()

add_subdirectory(test)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)

add_executable(Proyecto_I_GTest Server.cpp)
target_link_libraries(Proyecto_I_GTest
        gtest
        gtest_main
        gmock_main
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network)