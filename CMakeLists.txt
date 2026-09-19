cmake_minimum_required(VERSION 3.16)

project(FileSystemSimulator VERSION 2.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    filesystem.cpp
    filesystem.h
    node.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(FileSystemSimulator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(FileSystemSimulator SHARED ${PROJECT_SOURCES})
    else()
        add_executable(FileSystemSimulator ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(FileSystemSimulator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(FileSystemSimulator PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS FileSystemSimulator
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(FileSystemSimulator)
endif()