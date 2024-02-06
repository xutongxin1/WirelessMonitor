cmake_minimum_required(VERSION 3.5)

project(
  qmetarialComponents
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
# QT6 cmake改变
find_package(Qt6 REQUIRED COMPONENTS StateMachine)

file(GLOB THELIB "materiallib/*")
file(GLOB COMPONENTSHEADER "*.h")
file(GLOB COMPONENTSSOURCE "*.cpp")
file(GLOB HEADER "./include/qmetarial/*")
file(GLOB HEADER2 "./include/qmetarial/lib/*")

#target_include_directories(${PROJECT_NAME} PRIVATE ./include)




set(RCC_FILES resources.qrc)
add_library(
  ${PROJECT_NAME} STATIC ${THELIB} ${COMPONENTSHEADER} ${COMPONENTSSOURCE}
                         ${RCC_FILES} ${HEADER} ${HEADER2})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets)
target_include_directories(${PROJECT_NAME} PUBLIC ./include/qmetarial/)
target_link_directories(${PROJECT_NAME} PUBLIC .)
# QT6 cmake改变
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::StateMachine)