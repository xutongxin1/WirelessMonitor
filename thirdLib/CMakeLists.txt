add_subdirectory(qcustomplot)
add_subdirectory(qmetarial)
#add_subdirectory(qfluentwidgets)
add_subdirectory("CppQtFluentUi888/3rdparty")
add_subdirectory("CppQtFluentUi888/FluControls")
#add_subdirectory("CppQtFluentUi888/FluExamples")
add_subdirectory("CppQtFluentUi888/FluIconTool")
add_subdirectory("CppQtFluentUi888/FluUtils")
add_subdirectory("CppQtFluentUi888/FluWinGallery")

target_include_directories(${PROJECT_NAME} PUBLIC ./qmetarial/include/qmetarial/)

add_custom_target(resource_copy
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/CppQtFluentUi888/res
        ${PROJECT_BINARY_DIR}/res
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/CppQtFluentUi888/StyleSheet
        ${PROJECT_BINARY_DIR}/StyleSheet
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/CppQtFluentUi888/code
        ${PROJECT_BINARY_DIR}/code
)