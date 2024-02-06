add_subdirectory(qcustomplot)
add_subdirectory(qmetarial)
#add_subdirectory(qfluentwidgets)

target_include_directories(${PROJECT_NAME} PUBLIC ./qmetarial/include/qmetarial/)