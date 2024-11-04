#你的CMake构建选项应该如下样式


#设置最低版本
cmake_minimum_required(VERSION 3.24)
#递归查找最近的符合条件的 Git 标签
function(find_valid_version_tag OUTPUT_VAR)
    set(COMMAND_RESULT)
    execute_process(
            COMMAND git describe --tags --abbrev=0 ${COMMAND_RESULT}
            OUTPUT_VARIABLE TAG
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
    )

    if (TAG MATCHES "^v[0-9]+")
        string(REGEX REPLACE "^v" "" VERSION_NUMBER "${TAG}")
        set(${OUTPUT_VAR} ${VERSION_NUMBER} PARENT_SCOPE)
    else ()
        # 如果未找到或不符合条件，使用 `git describe --tags --abbrev=0 --skip=x` 递归查找
        set(SKIP_COUNTER 1)
        while (NOT TAG MATCHES "^v[0-9]+" AND SKIP_COUNTER LESS 100)
            execute_process(
                    COMMAND git describe --tags --abbrev=0 --skip=${SKIP_COUNTER}
                    OUTPUT_VARIABLE TAG
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    ERROR_QUIET
            )
            if (TAG MATCHES "^v[0-9]+")
                string(REGEX REPLACE "^v" "" VERSION_NUMBER "${TAG}")
                set(${OUTPUT_VAR} ${VERSION_NUMBER} PARENT_SCOPE)
                break()
            endif ()
            math(EXPR SKIP_COUNTER "${SKIP_COUNTER} + 1")
        endwhile ()

        if (NOT TAG MATCHES "^v[0-9]+")
            message(FATAL_ERROR "No valid version tag found.")
        endif ()
    endif ()
endfunction()


#project(Qt)
find_valid_version_tag(PROJECT_VERSION)
message(STATUS "PROJECT_VERSION: ${PROJECT_VERSION}")

#获取git信息
## get git hash
#macro(get_git_hash _git_hash)
#    find_package(Git QUIET)
#    if (GIT_FOUND)
#        execute_process(
#                COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
#                OUTPUT_VARIABLE ${_git_hash}
#                OUTPUT_STRIP_TRAILING_WHITESPACE
#                ERROR_QUIET
#                WORKING_DIRECTORY
#                ${PROJECT_SOURCE_DIR}
#        )
#    endif ()
#endmacro()
#
## get git branch
#macro(get_git_branch _git_branch)
#    find_package(Git QUIET)
#    if (GIT_FOUND)
#        execute_process(
#                COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
#                OUTPUT_VARIABLE ${_git_branch}
#                OUTPUT_STRIP_TRAILING_WHITESPACE
#                ERROR_QUIET
#                WORKING_DIRECTORY
#                ${PROJECT_SOURCE_DIR}
#        )
#    endif ()
#endmacro()

string(TIMESTAMP COMPILE_TIME %Y%m%d.%H%M)
set(build_time ${COMPILE_TIME})

#git号
set(GIT_HASH "")
#get_git_hash(GIT_HASH)
#git分支
set(GIT_BRANCH "")
#get_git_branch(GIT_BRANCH)
#message(STATUS "${build_time} ${GIT_BRANCH} ${GIT_HASH}")
set(big_version "${PROJECT_VERSION}")
set(bat_version "${PROJECT_VERSION}.${build_time}")


message(STATUS "big_version: ${big_version}")
message(STATUS "bat_version: ${bat_version}")
file(WRITE ${CMAKE_BINARY_DIR}/bat_version ${bat_version})

#准备构建无线调试器
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(
        WirelessMonitor
        VERSION ${PROJECT_VERSION}
        LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)

#set(BOOST_INCLUDEDIR "C:/Users/xtx/.vcpkg-clion/vcpkg/installed/x64-windows/include/")
#set(BOOST_LIBRARYDIR "C:/Users/xtx/.vcpkg-clion/vcpkg/installed/x64-windows/lib")
#set(Boost_NO_SYSTEM_PATHS ON)
#set(Boost_LIBRARY_DIR_DEBUG "C:/Users/xtx/.vcpkg-clion/vcpkg/installed/x64-windows/debug/lib")

add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_PROJECT_VERSION 1.1)

set(Boost_NO_WARN_NEW_VERSIONS 1)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    if (NOT DEFINED Boost_LIBRARY_DIR_DEBUG)
        message(FATAL_ERROR "Boost_LIBRARY_DIR_DEBUG 未定义，请强制手动定义")
    endif ()
else ()
    if (NOT DEFINED Boost_LIBRARY_DIR_RELEASE)
        message(FATAL_ERROR "Boost_LIBRARY_DIR_RELEASE 未定义，请强制手动定义")
    endif ()
endif ()

#set(CMAKE_PREFIX_PATH "C:/Qt/6.2.4/msvc2019_64/")

#set(Python_ROOT_DIR C:/Users/xtx/.vcpkg-clion/vcpkg/installed/x64-windows/include/python3.11)
#set(Boost_INCLUDE_DIR C:/Users/xtx/.vcpkg-clion/vcpkg/installed/x64-windows/include/)
#set(Python3_DIR "C:/Users/xtx/.vcpkg-clion/vcpkg/installed/x64-windows/include/boost/python")

#include(C:/Users/xtx/.vcpkg-clion/vcpkg/scripts/buildsystems/vcpkg.cmake)

find_package(QT NAMES Qt6 REQUIRED PrintSupport COMPONENTS Network SerialPort Widgets OpenGL Concurrent Svg)
find_package(Qt6 REQUIRED COMPONENTS Core5Compat)       # QT6
find_package(Qt6 REQUIRED COMPONENTS Gui)               # QT6
find_package(Qt${QT_VERSION_MAJOR} REQUIRED PrintSupport COMPONENTS Network SerialPort Widgets OpenGL Concurrent Svg HttpServer)
qt_standard_project_setup()

find_package(Python
        REQUIRED COMPONENTS Development)
find_package(Boost
        REQUIRED COMPONENTS python311)
#find_package(Boost)

file(GLOB HEADER "*.h")
file(GLOB RSOURCE "*.cpp")
file(GLOB UI "*.ui")
file(GLOB QRC "*.qrc")
#file(GLOB_RECURSE Resource "./thirdLib/qmetarial/resources.qrc")
file(GLOB_RECURSE SOURCES ${SOURCES} "./config/*.ini")

#if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux") #XXX平台可以是“Linux”等
#    EXECUTE_PROCESS(COMMAND cp -f ${PROJECT_SOURCE_DIR}/config/xxxx1.config ${PROJECT_SOURCE_DIR}/config/xxxx2.config)
#else()
#    EXECUTE_PROCESS(COMMAND cp -f ${PROJECT_SOURCE_DIR}/main/config/*.ini ${CMAKE_CURRENT_BINARY_DIR}/config/*.config)
#endif()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_executable(${PROJECT_NAME} WIN32 ${HEADER} ${RSOURCE} ${UI} ${QRC} ${Resource})
else ()
    add_executable(${PROJECT_NAME} ${HEADER} ${RSOURCE} ${UI} ${QRC} ${Resource})
endif ()
add_subdirectory(api)
add_subdirectory(components)
add_subdirectory(thirdLib)
#add_library(whatever STATIC ${SOURCES})
#if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
#    add_link_options(${PROJECT_NAME} PRIVATE -mwindows)
#endif ()
target_include_directories(${PROJECT_NAME} PUBLIC ./components)
target_include_directories(${PROJECT_NAME} PUBLIC ./config)
MESSAGE(STATUS ${Boost_LIBRARY_DIR_DEBUG})
target_include_directories(${PROJECT_NAME} PUBLIC ../thirdLib/CppQtFluentUi888/FluControls/)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}/python3.11)

#if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
#    if (NOT DEFINED Boost_LIBRARY_DIR_DEBUG)
#        message(FATAL_ERROR "Boost_LIBRARY_DIR_DEBUG 未定义，请强制手动定义")
#    endif ()
#
#    if (EXISTS "${Boost_LIBRARY_DIR_DEBUG}/python311.lib")
#        message(STATUS "python311.lib exists, is OK!?")
#        message(STATUS ${Boost_LIBRARY_DIR_DEBUG}/python311_d.lib)
#
#    else ()
#        execute_process(COMMAND cp.exe -f ${Boost_LIBRARY_DIR_DEBUG}/python311_d.lib ${Boost_LIBRARY_DIR_DEBUG}/python311.lib
#                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/thirdPartyTool/cp)
#        message(STATUS "python311_d.lib copy OK")
#        message(STATUS ${Boost_LIBRARY_DIR_DEBUG}/python311_d.lib)
#    endif ()
#endif ()

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    target_link_libraries(
            ${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Core Qt6::PrintSupport Qt6::SerialPort Qt6::Svg Qt6::HttpServer
            Qt6::Network qmetarialComponents Qt6::Core5Compat Qt6::Gui Qt6::OpenGL Qt6::Concurrent ${CMAKE_CURRENT_SOURCE_DIR}/api/USBHandle/ATL/lib/atls.lib "${CMAKE_CURRENT_SOURCE_DIR}/api/OpenGL32.Lib" ${Boost_LIBRARIES})
else ()
    target_link_libraries(
            ${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Core Qt6::PrintSupport Qt6::SerialPort Qt6::Svg Qt6::HttpServer
            Qt6::Network qmetarialComponents Qt6::Core5Compat Qt6::Gui Qt6::OpenGL Qt6::Concurrent ${CMAKE_CURRENT_SOURCE_DIR}/api/USBHandle/ATL/lib/atls.lib "${CMAKE_CURRENT_SOURCE_DIR}/api/OpenGL32.Lib" ${Boost_LIBRARIES})
endif ()

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE)

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy ${Boost_LIBRARY_DIR_DEBUG}/../bin/python311_d.dll ${CMAKE_CURRENT_BINARY_DIR}/
#)

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#        COMMAND cp -r ${Boost_LIBRARY_DIR_DEBUG}/../bin/{python311_d.dll,boost_python311-vc143-mt-gyd-x64-1_85.dll,zlibd1.dll} ${CMAKE_CURRENT_BINARY_DIR}/
#        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/thirdPartyTool/cp
#)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND cp -r -f ${PROJECT_SOURCE_DIR}/config ${CMAKE_CURRENT_BINARY_DIR}/
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/thirdPartyTool/cp
)


#if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/Debug/resource")
#    message(STATUS "resource exists")
#
#else ()
#    message(STATUS "resource does not exist")
#    file(COPY ./thirdLib/qmetarial/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/resource FOLLOW_SYMLINK_CHAIN)
#endif ()

if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/resource")
    message(STATUS "resource exists")

else ()
    message(STATUS "resource does not exist")
    file(COPY ./thirdLib/qmetarial/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resource FOLLOW_SYMLINK_CHAIN)
endif ()


add_subdirectory(thirdTarget_ComTool)
add_subdirectory(thirdTarget_I2CTool)

configure_file(
        "${RepeaterWidgetPath}/VersionConfig.h.in"
        "${RepeaterWidgetPath}/VersionConfig.h"
)

include(GNUInstallDirs)
#set(CPACK_PACKAGE_FILE_NAME "I2CTool")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
#set(CPACK_PACKAGE_FILE_NAME "xtxTools-${build_time}-${GIT_BRANCH}-${GIT_HASH}")

if (DEFINED UPDATE)
    set(CPACK_PACKAGE_FILE_NAME "xtxTools-updatePackage")
else ()
    set(CPACK_PACKAGE_FILE_NAME "xtxTools")
endif ()

include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR "ZIP")
#set(CPACK_GENERATOR "NSIS;ZIP")

#NSIS相关（没开启）
set(CPACK_PACKAGE_VENDOR "xtx Group")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "xtxTool Installer")
set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/icon/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/icon/icon.ico")
#set(CPACK_NSIS_MUI_HEADERIMAGE "${CMAKE_SOURCE_DIR}/icon/backgroud.bmp")
set(CPACK_NSIS_LICENSE_FILE "${CMAKE_SOURCE_DIR}/icon/LICENSE.txt")
set(CPACK_NSIS_INSTALLER_MUI_ICON "${CMAKE_SOURCE_DIR}/icon/icon.ico")
set(CPACK_NSIS_INSTALLER_MUI_UNIICON "${CMAKE_SOURCE_DIR}/icon/icon.ico")
#set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP "${CMAKE_SOURCE_DIR}/icon/backgroud.bmp")
#set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "${CMAKE_SOURCE_DIR}/icon/backgroud.bmp")
set(CPACK_NSIS_HELP_LINK "https://www.myproject.com/help")
set(CPACK_NSIS_URL_INFO_ABOUT "https://www.myproject.com")
set(CPACK_NSIS_INSTALL_ROOT "C:/Program Files/CMake Tests Install Root")

set(CPACK_NSIS_MUI_PAGE_WELCOME "ENABLE")
set(CPACK_NSIS_MUI_PAGE_LICENSE "ENABLE")
set(CPACK_NSIS_MUI_PAGE_DIRECTORY "ENABLE")
set(CPACK_NSIS_MUI_PAGE_INSTFILES "ENABLE")
set(CPACK_NSIS_MUI_PAGE_FINISH "ENABLE")
set(CPACK_NSIS_MODERN_UI "ON")

# 设置支持 COMPONENT
set(CPACK_COMPONENTS_GROUPING ONE_PER_GROUP)
set(CPACK_RPM_COMPONENT_INSTALL ON)
# Settings used when building NSIS installers
#set(CPACK_NSIS_MENU_LINKS
#        "ftp://ftpserver" "Test Ftp Link"
#        "ftps://ftpsserver" "Test Ftps Link"
#        "https://cmake.org" "CMake Web Site"
#        "https://github.com/" "Test Https Link"
#        "mailto:kitware@kitware.com" "Test MailTo Link"
#        "news://newsserver" "Test News Link"
#)

# Suggested default root for end users of the installer:


set(CPACK_COMPONENTS_ALL I2CTool ComTool)
set(CPACK_ARCHIVE_COMPONENT_INSTALL 1)
include(CPack)

add_custom_target(packageCPack
        COMMAND ${CMAKE_CPACK_COMMAND}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Packaging ComTool"
)
