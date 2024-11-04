
project(
        thirdTarget_I2CTool
        VERSION ${PROJECT_VERSION}
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
file(GLOB RC "*.rc")

find_package(QT NAMES Qt6 REQUIRED PrintSupport COMPONENTS Network SerialPort Widgets OpenGL Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Core5Compat)       # QT6
find_package(Qt6 REQUIRED COMPONENTS Gui)               # QT6
find_package(Qt6 REQUIRED PrintSupport COMPONENTS Network SerialPort Widgets OpenGL Concurrent HttpServer)
qt_standard_project_setup()

find_package(Python
        REQUIRED COMPONENTS Development)
find_package(Boost
        REQUIRED COMPONENTS python311)
#find_package(Boost)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_executable(${PROJECT_NAME} WIN32 ${HEADER} ${SOURCE} ${UI} ${QRC} ${RC})
else ()
    add_executable(${PROJECT_NAME} ${HEADER} ${SOURCE} ${UI} ${QRC} ${RC})
endif ()

#file(GLOB_RECURSE HEADER "../components/IICDeviceBasic/*.h")
#file(GLOB_RECURSE SOURCE "../components/IICDeviceBasic/*.cpp")
#file(GLOB_RECURSE UI "../components/IICDeviceBasic/*.ui")
#file(GLOB_RECURSE QRC "../components/IICDeviceBasic/*.qrc")
target_sources(${PROJECT_NAME} PUBLIC ${HEADER} ${SOURCE} ${UI} ${QRC})
target_include_directories(${PROJECT_NAME} PUBLIC ../thirdLib/CppQtFluentUi888/FluControls/)
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}/python3.11)

add_subdirectory(../api/Highlighter Highlighter)
add_subdirectory(../api/RepeaterWidget RepeaterWidget)
add_subdirectory(../api/TCPHandle TCPHandle)
add_subdirectory(../api/ConfigClass ConfigClass)
add_subdirectory(../api/QTextEditWithKey QTextEditWithKey)
add_subdirectory(../api/PythonWork PythonWork)
add_subdirectory(../api/USBHandle USBHandle)
add_subdirectory(../api/CH339DLL CH339DLL)
add_subdirectory(../thirdLib/QXlsx/QXlsx QXlsx)

message(STATUS "Boost_LIBRARY_DIR_RELEASE is ${Boost_LIBRARY_DIR_RELEASE}")
message(STATUS "Boost_LIBRARIES is ${Boost_LIBRARIES}")

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    get_target_property(Boost_DLL_LOCATION ${Boost_LIBRARIES} IMPORTED_LOCATION_RELEASE)
    target_link_libraries(
            ${PROJECT_NAME} PRIVATE
            Qt6::Core Qt6::PrintSupport Qt6::SerialPort Qt6::Widgets FluControls QXlsx::QXlsx Qt6::HttpServer
            Qt6::Network qmetarialComponents Qt6::Core5Compat Qt6::Gui Qt6::OpenGL Qt6::Concurrent ${CMAKE_CURRENT_SOURCE_DIR}/../api/USBHandle/ATL/lib/atls.lib ${Boost_LIBRARIES})
    install(FILES ${Boost_LIBRARY_DIR_RELEASE}/../bin/zlib1.dll
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT I2CTool
    )

else ()
    get_target_property(Boost_DLL_LOCATION ${Boost_LIBRARIES} IMPORTED_LOCATION_DEBUG)
    target_link_libraries(
            ${PROJECT_NAME} PRIVATE
            Qt6::Core Qt6::PrintSupport Qt6::SerialPort Qt6::Widgets FluControls QXlsx::QXlsx Qt6::HttpServer
            Qt6::Network qmetarialComponents Qt6::Core5Compat Qt6::Gui Qt6::OpenGL Qt6::Concurrent ${CMAKE_CURRENT_SOURCE_DIR}/../api/USBHandle/ATL/lib/atls.lib ${Boost_LIBRARIES})
    install(FILES ${Boost_LIBRARY_DIR_DEBUG}/../bin/zlibd1.dll
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            COMPONENT I2CTool
    )

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND cp -r ${Boost_LIBRARY_DIR_DEBUG}/../bin/{python311_d.dll,boost_python311-vc142-mt-gd-x64-1_85.dll,zlibd1.dll} ${CMAKE_CURRENT_BINARY_DIR}/
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../thirdPartyTool/cp
    )

endif ()

#set_target_properties(
#        ${PROJECT_NAME}
#        PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#        MACOSX_BUNDLE_SHORT_VERSION_STRING
#        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#        MACOSX_BUNDLE TRUE)

#add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
#        COMMAND cp.exe -r -f ${PROJECT_SOURCE_DIR}/config ${CMAKE_CURRENT_BINARY_DIR}
#        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../thirdPartyTool/cp
#)

#复制资源与配置文件
if (EXISTS "${CMAKE_CURRENT_BINARY_DIR}/resource")
    message(STATUS "resource exists")

else ()
    message(STATUS "resource does not exist")
    file(COPY ../thirdLib/qmetarial/icons DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resource FOLLOW_SYMLINK_CHAIN)
endif ()



add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND cp.exe -r -f ${PROJECT_SOURCE_DIR}/config ${CMAKE_CURRENT_BINARY_DIR}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../thirdPartyTool/cp
)#不是说不能用-E，但是不能覆写确实不好用


install(TARGETS ${PROJECT_NAME}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT I2CTool
)

install(FILES ${Boost_DLL_LOCATION} ${Python_RUNTIME_LIBRARY_DEBUG}
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT I2CTool
)


#set(deploy_tool_options_arg --no-compiler-runtime)
qt_generate_deploy_app_script(
        TARGET ${PROJECT_NAME}
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
        #        DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
)
install(SCRIPT ${deploy_script} COMPONENT I2CTool)


#if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/ComTool/
#    )
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/ComTool/
#    )
#
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#            COMMAND cp.exe -r -f ${CMAKE_CURRENT_BINARY_DIR}/{${PROJECT_NAME}.exe,config/,resource/} ${CMAKE_CURRENT_BINARY_DIR}/ComTool/
#            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../thirdPartyTool/cp
#
#    )
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#            COMMAND windeployqt.exe ${CMAKE_CURRENT_BINARY_DIR}/ComTool/${PROJECT_NAME}.exe
#            WORKING_DIRECTORY ${QT6_INSTALL_PREFIX}/bin
#    )
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#            COMMAND ${CMAKE_COMMAND} -E rename ${CMAKE_CURRENT_BINARY_DIR}/ComTool/${PROJECT_NAME}.exe ${CMAKE_CURRENT_BINARY_DIR}/ComTool/ComTool.exe
#    )
#endif ()


add_custom_target(install_I2CTool
        COMMAND ${CMAKE_COMMAND} --install . --component I2CTool
)
add_custom_target(package_I2CTool
        COMMAND ${CMAKE_CPACK_COMMAND}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Packaging I2CTool"
)



#
#add_custom_target(resource_copy_I2C
#        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
#        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdLib/CppQtFluentUi888/res
#        ${PROJECT_BINARY_DIR}/res
#        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
#        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdLib/CppQtFluentUi888/StyleSheet
#        ${PROJECT_BINARY_DIR}/StyleSheet
#        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
#        ${CMAKE_CURRENT_SOURCE_DIR}/../thirdLib/CppQtFluentUi888/code
#        ${PROJECT_BINARY_DIR}/code
#)