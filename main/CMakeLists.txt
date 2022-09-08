cmake_minimum_required(VERSION 3.5)

project(
  Main
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PROJECT_VERSION 1.0)
find_package(QT NAMES Qt6 Qt5 REQUIRED PrintSupport COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED PrintSupport COMPONENTS Widgets)

file(GLOB_RECURSE  HEADER "*.h")
file(GLOB_RECURSE  RSOURCE "*.cpp")
file(GLOB_RECURSE  UI "*.ui")
file(GLOB_RECURSE  QRC "*.qrc")
file(GLOB Resource "../components/resources.qrc")

#if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux") #XXX平台可以是“Linux”等
#    EXECUTE_PROCESS(COMMAND cp -f ${PROJECT_SOURCE_DIR}/config/xxxx1.config ${PROJECT_SOURCE_DIR}/config/xxxx2.config)
#else()
#    EXECUTE_PROCESS(COMMAND cp -f ${PROJECT_SOURCE_DIR}/main/config/*.ini ${CMAKE_CURRENT_BINARY_DIR}/config/*.config)
#endif()

add_executable(${PROJECT_NAME} ${HEADER} ${RSOURCE} ${UI} ${QRC} ${Resource})
target_include_directories(${PROJECT_NAME} PUBLIC ../include/qmetarial/)
target_include_directories(${PROJECT_NAME} PUBLIC ./components/)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                          Qt${QT_VERSION_MAJOR}::Core Qt5::PrintSupport components)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp -r -f ${PROJECT_SOURCE_DIR}/config ${CMAKE_CURRENT_BINARY_DIR}
        )