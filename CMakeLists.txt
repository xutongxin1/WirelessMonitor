cmake_minimum_required(VERSION 3.22)
#project(Qt)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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



find_package(QT NAMES Qt6 Qt5 REQUIRED PrintSupport COMPONENTS Network SerialPort Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED PrintSupport COMPONENTS Network SerialPort Widgets)

file(GLOB HEADER "*.h")
file(GLOB RSOURCE "*.cpp")
file(GLOB UI "*.ui")
file(GLOB QRC "*.qrc")
file(GLOB_RECURSE Resource "./thirdLib/qmetarial/resources.qrc")
file(GLOB_RECURSE SOURCES ${SOURCES} "./config/*.ini")

#if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux") #XXX平台可以是“Linux”等
#    EXECUTE_PROCESS(COMMAND cp -f ${PROJECT_SOURCE_DIR}/config/xxxx1.config ${PROJECT_SOURCE_DIR}/config/xxxx2.config)
#else()
#    EXECUTE_PROCESS(COMMAND cp -f ${PROJECT_SOURCE_DIR}/main/config/*.ini ${CMAKE_CURRENT_BINARY_DIR}/config/*.config)
#endif()


add_executable(${PROJECT_NAME} ${HEADER} ${RSOURCE} ${UI} ${QRC} ${Resource})
add_subdirectory(api)
add_subdirectory(components)
add_subdirectory(thirdLib)
add_library(whatever STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ./components)
target_include_directories(${PROJECT_NAME} PUBLIC ./config)


target_link_libraries(
  ${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                          Qt${QT_VERSION_MAJOR}::Core Qt5::PrintSupport Qt5::SerialPort Qt5::Network components)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp -r -f ${PROJECT_SOURCE_DIR}/config ${CMAKE_CURRENT_BINARY_DIR}/Debug
        )#给Clion等win环境使用

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp -r -f ${PROJECT_SOURCE_DIR}/config ${CMAKE_CURRENT_BINARY_DIR}
        )#QTCreator或者linux环境使用
