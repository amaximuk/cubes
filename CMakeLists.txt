CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

set(TARGET_NAME config_maker)
project(${TARGET_NAME})

set(CMAKE_CXX_STANDARD 14)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB PROJ_HDRS *.h)
file(GLOB PROJ_SRCS *.cpp)

add_executable(${TARGET_NAME} ${PROJ_HDRS} ${PROJ_SRCS})

target_link_libraries(${TARGET_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)