CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

project(cubes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

#set(QT_ROOT /Users/pbondarenko/Qt5.12.5/5.12.5/clang_64)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB PROJ_HDRS *.h)
file(GLOB PROJ_SRCS *.cpp)

add_executable(cubes ${PROJ_HDRS} ${PROJ_SRCS})

target_link_libraries(cubes Qt5::Core Qt5::Gui Qt5::Widgets)
