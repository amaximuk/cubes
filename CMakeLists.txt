CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

project(cubes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

#set(QT_ROOT /Users/pbondarenko/Qt5.12.5/5.12.5/clang_64)
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
#find_package(QtPropertyBrowser REQUIRED PATHS c:/QtProjects/QtPropertyBrowser/install)

if(TARGET QtPropertyBrowser)
    message("!!!!!!!!!!!! Have target QtPropertyBrowser")
else()
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/build)
    execute_process(COMMAND ${CMAKE_COMMAND} -B ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/build
        -DQt5Widgets_DIR=${Qt5Widgets_DIR} -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=install -G ${CMAKE_GENERATOR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser)
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/build)
    execute_process(COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/build)
    find_package(QtPropertyBrowser REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/install)
endif()

# execute_process(COMMAND build.cmd WORKING_DIRECTORY 3rdparty)
# find_package(QtPropertyBrowser REQUIRED PATHS 3rdparty/QtPropertyBrowser/install)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB PROJ_HDRS *.h)
file(GLOB PROJ_SRCS *.cpp)

add_executable(cubes ${PROJ_HDRS} ${PROJ_SRCS})

target_link_libraries(cubes Qt5::Core Qt5::Gui Qt5::Widgets QtPropertyBrowser)
