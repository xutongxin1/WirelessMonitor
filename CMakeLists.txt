cmake_minimum_required(VERSION 3.5)

project(QT VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()
set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.12.12/5.12.12/mingw73_64/lib/cmake/Qt5")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(components)


find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS  Widgets  PrintSupport REQUIRED)
aux_source_directory(./ PROJECT_SOURCES)
aux_source_directory(./components COMPONENTS_SOURCES)
#set(PROJECT_SOURCES
#        main.cpp
#        mainwindow.cpp
#        mainwindow.h
#        mainwindow.ui
#)
file(GLOB HEADER "*.h")
file(GLOB RSOURCE "*.cpp")
file(GLOB UI "*.ui")
include_directories(./components)
#include_directories(./include/qmetarial/)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QT
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(QT SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(QT
                ${HEADER} ${RSOURCE} ${UI} ${COMPONENTS_SOURCES}
        )
    endif()
endif()
target_include_directories(${PROJECT_NAME} PUBLIC ../include/qmetarial/)
target_link_libraries(QT PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt5::PrintSupport components)

set_target_properties(QT PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QT)
endif()
