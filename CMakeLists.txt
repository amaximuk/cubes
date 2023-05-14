CMAKE_MINIMUM_REQUIRED(VERSION 3.13)

project(cubes LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)  # Includes the contents of the conanbuildinfo.cmake file.
conan_basic_setup()  # Prepares the CMakeList.txt for Conan.

find_package(Qt5 COMPONENTS Core Gui Widgets Xml REQUIRED)
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

file(GLOB PROJ_HDRS src/*.h)
file(GLOB PROJ_SRCS src/*.cpp)
file(GLOB PROJ_RES *.qrc)
file(GLOB FILE_ITEM_HDRS src/file_item/*.h)
file(GLOB FILE_ITEM_SRCS src/file_item/*.cpp)
file(GLOB LOG_TABLE_ITEM_HDRS src/log_table/*.h)
file(GLOB LOG_TABLE_ITEM_SRCS src/log_table/*.cpp)
file(GLOB PARAMETERS_COMPIER_HDRS src/parameters_compiler/*.h)
file(GLOB PARAMETERS_COMPIER_SRCS src/parameters_compiler/*.cpp)
file(GLOB PROPERTIES_ITEM_HDRS src/properties_item/*.h)
file(GLOB PROPERTIES_ITEM_SRCS src/properties_item/*.cpp)
file(GLOB PROPERTY_BROWSER_HDRS src/property_browser/*.h)
file(GLOB PROPERTY_BROWSER_SRCS src/property_browser/*.cpp)

source_group("Header Files" FILES ${PROJ_HDRS})
source_group("Source Files" FILES ${PROJ_SRCS})
source_group("Header Files/File Item" FILES ${FILE_ITEM_HDRS})
source_group("Source Files/File Item" FILES ${FILE_ITEM_SRCS})
source_group("Header Files/Log Table" FILES ${LOG_TABLE_ITEM_HDRS})
source_group("Source Files/Log Table" FILES ${LOG_TABLE_ITEM_SRCS})
source_group("Header Files/Parameters Compiler" FILES ${PARAMETERS_COMPIER_HDRS})
source_group("Source Files/Parameters Compiler" FILES ${PARAMETERS_COMPIER_SRCS})
source_group("Header Files/Properties Item" FILES ${PROPERTIES_ITEM_HDRS})
source_group("Source Files/Properties Item" FILES ${PROPERTIES_ITEM_SRCS})
source_group("Header Files/Property Browser" FILES ${PROPERTY_BROWSER_HDRS})
source_group("Source Files/Property Browser" FILES ${PROPERTY_BROWSER_SRCS})

add_executable(${PROJECT_NAME} ${PROJ_HDRS} ${PROJ_SRCS} ${PROJ_RES}
	${FILE_ITEM_HDRS} ${FILE_ITEM_SRCS} ${LOG_TABLE_ITEM_HDRS} ${LOG_TABLE_ITEM_SRCS}
	${PARAMETERS_COMPIER_HDRS} ${PARAMETERS_COMPIER_SRCS} ${PROPERTIES_ITEM_HDRS} ${PROPERTIES_ITEM_SRCS}
	${PROPERTY_BROWSER_HDRS} ${PROPERTY_BROWSER_SRCS})

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml QtPropertyBrowser ${CONAN_LIBS})
#target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets QtPropertyBrowser)

if(WIN32)
	target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>)
	target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>)
	target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/ENTRY:mainCRTStartup>)
	target_compile_options(${PROJECT_NAME} PRIVATE /MP)
	
	# configure_file(${PROJECT_SOURCE_DIR}/qtlogging.ini ${CMAKE_BINARY_DIR}/Debug/qtlogging.ini COPYONLY)
	# configure_file(${PROJECT_SOURCE_DIR}/qtlogging.ini ${CMAKE_BINARY_DIR}/Release/qtlogging.ini COPYONLY)
# else()
	# configure_file(${PROJECT_SOURCE_DIR}/qtlogging.ini ${CMAKE_BINARY_DIR}/qtlogging.ini COPYONLY)
endif()
