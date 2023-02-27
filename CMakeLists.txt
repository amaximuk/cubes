CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

project(cubes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)  # Includes the contents of the conanbuildinfo.cmake file.
conan_basic_setup()  # Prepares the CMakeList.txt for Conan.

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(QtPropertyBrowser PATHS ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/install)

if(TARGET QtPropertyBrowser)
    message("!!!!!!!!!!!! Have target QtPropertyBrowser")
else()
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/build)
    execute_process(COMMAND ${CMAKE_COMMAND} -B ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/build
        -DQt5Widgets_DIR=${Qt5Widgets_DIR} -DCMAKE_DEBUG_POSTFIX=d -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=install -G ${CMAKE_GENERATOR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser)
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/build)
    execute_process(COMMAND ${CMAKE_COMMAND} --install . --config Debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/build)
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/QtPropertyBrowser/build)
    execute_process(COMMAND ${CMAKE_COMMAND} --install . --config Release
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
file(GLOB PROJ_RES *.qrc)

add_executable(${PROJECT_NAME} ${PROJ_HDRS} ${PROJ_SRCS} ${PROJ_RES})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets QtPropertyBrowser ${CONAN_LIBS})
#target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets QtPropertyBrowser)

if(WIN32)
	target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>)
	target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>)
	target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/ENTRY:mainCRTStartup>)
	
	# configure_file(${PROJECT_SOURCE_DIR}/qtlogging.ini ${CMAKE_BINARY_DIR}/Debug/qtlogging.ini COPYONLY)
	# configure_file(${PROJECT_SOURCE_DIR}/qtlogging.ini ${CMAKE_BINARY_DIR}/Release/qtlogging.ini COPYONLY)
# else()
	# configure_file(${PROJECT_SOURCE_DIR}/qtlogging.ini ${CMAKE_BINARY_DIR}/qtlogging.ini COPYONLY)
endif()
