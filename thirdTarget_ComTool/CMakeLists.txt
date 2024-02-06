
project(
        thirdTarget_ComTool
        VERSION 0.1
        LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)


add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PROJECT_VERSION 1.1)

file(GLOB HEADER "*.h")
file(GLOB SOURCE "*.cpp")
file(GLOB UI "*.ui")
file(GLOB QRC "*.qrc")

find_package(QT NAMES Qt6 REQUIRED PrintSupport COMPONENTS Network SerialPort Widgets OpenGL Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Core5Compat)       # QT6
find_package(Qt6 REQUIRED COMPONENTS Gui)               # QT6
find_package(Qt6 REQUIRED PrintSupport COMPONENTS Network SerialPort Widgets OpenGL Concurrent)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_executable(${PROJECT_NAME} WIN32  ${HEADER} ${SOURCE} ${UI} ${QRC})
else ()
    add_executable(${PROJECT_NAME}  ${HEADER} ${SOURCE} ${UI} ${QRC})
endif ()
if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_link_options(${PROJECT_NAME} PRIVATE -mwindows)
endif ()

file(GLOB_RECURSE HEADER "../components/ComTool/*.h")
file(GLOB_RECURSE SOURCE "../components/ComTool/*.cpp")
file(GLOB_RECURSE  UI "../components/ComTool/*.ui")
file(GLOB_RECURSE  QRC "../components/ComTool/*.qrc")
target_sources(${PROJECT_NAME} PUBLIC ${HEADER} ${SOURCE} ${UI} ${QRC})
target_include_directories(${PROJECT_NAME} PUBLIC ../components/ComTool/)

add_subdirectory(../api/Highlighter Highlighter)
add_subdirectory(../api/RepeaterWidget RepeaterWidget)
add_subdirectory(../api/TCPHandle TCPHandle)
add_subdirectory(../api/ConfigClass ConfigClass)
add_subdirectory(../api/QTextEditWithKey QTextEditWithKey)
add_subdirectory(../api/ComToolDepend ComToolDepend)

target_link_libraries(
        ${PROJECT_NAME} PRIVATE
        Qt6::Core Qt6::PrintSupport Qt6::SerialPort Qt6::Widgets
        Qt6::Network qmetarialComponents Qt6::Core5Compat Qt6::Gui Qt6::OpenGL Qt6::Concurrent)

#set_target_properties(
#        ${PROJECT_NAME}
#        PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#        MACOSX_BUNDLE_SHORT_VERSION_STRING
#        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#        MACOSX_BUNDLE TRUE)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp.exe -r -f ${PROJECT_SOURCE_DIR}/config ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../thirdPartyTool/cp
)

if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/resource")
    message(STATUS "resource exists")

else ()
    message(STATUS "resource does not exist")
    file(COPY ../thirdLib/qmetarial/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resource FOLLOW_SYMLINK_CHAIN)
endif ()