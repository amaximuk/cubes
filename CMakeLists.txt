cmake_minimum_required(VERSION 2.8...3.28)

project(cubes
    VERSION   1.0.0
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(PACKAGE_MANAGER "NONE" CACHE STRING "Type of package manager to be used for configure project")
set_property(CACHE PACKAGE_MANAGER PROPERTY STRINGS NONE CONAN 3RDPARTY)

if("${PACKAGE_MANAGER}" STREQUAL "CONAN")
	set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
	
	find_package(Boost REQUIRED CONFIG)
	find_package(Qt5 COMPONENTS Core Gui Widgets Xml REQUIRED CONFIG)
	find_package(yaml-cpp REQUIRED CONFIG)
	find_package(jsoncpp REQUIRED CONFIG)
	find_package(minizip REQUIRED CONFIG)
	
	set(QT_ROOT_DEBUG "${Qt5_Core_LIB_DIRS_DEBUG}/..")
	set(QT_ROOT_RELEASE "${Qt5_Core_LIB_DIRS_RELEASE}/..")
elseif("${PACKAGE_MANAGER}" STREQUAL "3RDPARTY")
	include("${CMAKE_CURRENT_LIST_DIR}/3rdparty_v2/cmake/configure.cmake")
	find_package(Qt5 COMPONENTS Core Gui Widgets Xml REQUIRED PATHS ${QT_ROOT} NO_SYSTEM_ENVIRONMENT_PATH)
else()
	find_package(parameters REQUIRED PATHS ${PARAMETERS_ROOT} CONFIG)
	find_package(QtPropertyBrowser REQUIRED PATHS ${QTPROPERTYBROWSER_ROOT} CONFIG)
	find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)
	find_package(yaml-cpp REQUIRED)
	find_package(jsoncpp REQUIRED)
	find_package(minizip REQUIRED CONFIG)
endif()

if("${PACKAGE_MANAGER}" STREQUAL "CONAN")
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${PROJECT_SOURCE_DIR}/patches/submodules/QtPropertyBrowser/CMakeLists.txt
		${PROJECT_SOURCE_DIR}/submodules/QtPropertyBrowser/CMakeLists.txt)
	execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${PROJECT_SOURCE_DIR}/patches/submodules/QtPropertyBrowser/cmake/InstallProjectConfig.cmake
		${PROJECT_SOURCE_DIR}/submodules/QtPropertyBrowser/cmake/InstallProjectConfig.cmake)
	add_subdirectory(submodules/QtPropertyBrowser)
	add_subdirectory(submodules/parameters)
endif()

include(GNUInstallDirs)

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
file(GLOB DIAGRAM_HDRS src/diagram/*.h)
file(GLOB DIAGRAM_SRCS src/diagram/*.cpp)
file(GLOB XML_HDRS src/xml/*.h)
file(GLOB XML_SRCS src/xml/*.cpp)

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
source_group("Header Files/Diagram" FILES ${DIAGRAM_HDRS})
source_group("Source Files/Diagram" FILES ${DIAGRAM_SRCS})
source_group("Header Files/Xml" FILES ${XML_HDRS})
source_group("Source Files/Xml" FILES ${XML_SRCS})

if(UNIX)
	set(CMAKE_SKIP_BUILD_RPATH FALSE)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(CMAKE_INSTALL_RPATH "$ORIGIN/:$ORIGIN/lib/:./:./lib/")
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
endif()

add_executable(${PROJECT_NAME} ${PROJ_HDRS} ${PROJ_SRCS} ${PROJ_RES}
	${FILE_ITEM_HDRS} ${FILE_ITEM_SRCS} ${LOG_TABLE_ITEM_HDRS} ${LOG_TABLE_ITEM_SRCS}
	${PARAMETERS_COMPIER_HDRS} ${PARAMETERS_COMPIER_SRCS} ${PROPERTIES_ITEM_HDRS} ${PROPERTIES_ITEM_SRCS}
	${PROPERTY_BROWSER_HDRS} ${PROPERTY_BROWSER_SRCS} ${DIAGRAM_HDRS} ${DIAGRAM_SRCS}
    ${XML_HDRS} ${XML_SRCS})

if("${PACKAGE_MANAGER}" STREQUAL "CONAN")
	target_link_libraries(${PROJECT_NAME} parameters QtPropertyBrowser Boost::boost Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml yaml-cpp jsoncpp_lib_static minizip::minizip)
	target_include_directories(${PROJECT_NAME} PRIVATE submodules/QtPropertyBrowser/src)
elseif("${PACKAGE_MANAGER}" STREQUAL "3RDPARTY")
	target_link_libraries(${PROJECT_NAME} tpv2::parameters tpv2::qtpropertybrowser tpv2::boost Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml tpv2::yamlcpp tpv2::jsoncpp)
else()
	target_link_libraries(${PROJECT_NAME} parameters::parameters QtPropertyBrowser Boost::boost Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml yaml-cpp jsoncpp_lib_static minizip::minizip)
endif()

if(WIN32)
	target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:/SUBSYSTEM:CONSOLE>)
	target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>)
	target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/ENTRY:mainCRTStartup>)
	target_compile_options(${PROJECT_NAME} PRIVATE /MP)
endif()

configure_file(${PROJECT_SOURCE_DIR}/deploy_linux.sh.in ${PROJECT_SOURCE_DIR}/deploy_linux.sh @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/deploy_windows_debug.cmd.in ${PROJECT_SOURCE_DIR}/deploy_windows_debug.cmd @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/deploy_windows_release.cmd.in ${PROJECT_SOURCE_DIR}/deploy_windows_release.cmd @ONLY)
