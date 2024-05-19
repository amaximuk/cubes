set(TARGET_NAME test_cubes)

include_directories(../src)

# добавляем исходники и хидеры
file(GLOB TEST_SOURCES src/*.cpp) 
file(GLOB TEST_HEADERS src/*.h)

# распределяем файлы на манер вижуал студии
source_group("Header Files" FILES ${TEST_HEADERS})
source_group("Source Files" FILES ${TEST_SOURCES})
source_group("Header Files/Cubes" FILES ${PROJ_HDRS})
source_group("Source Files/Cubes" FILES ${PROJ_SRCS})
source_group("Header Files/Cubes/File" FILES ${FILE_HDRS})
source_group("Source Files/Cubes/File" FILES ${FILE_SRCS})
source_group("Header Files/Cubes/Log" FILES ${LOG_HDRS})
source_group("Source Files/Cubes/Log" FILES ${LOG_SRCS})
source_group("Header Files/Cubes/Parameters" FILES ${PARAMETERS_HDRS})
source_group("Source Files/Cubes/Parameters" FILES ${PARAMETERS_SRCS})
source_group("Header Files/Cubes/Properties" FILES ${PROPERTIES_HDRS})
source_group("Source Files/Cubes/Properties" FILES ${PROPERTIES_SRCS})
source_group("Header Files/Cubes/Property Browser" FILES ${PROPERTY_BROWSER_HDRS})
source_group("Source Files/Cubes/Property Browser" FILES ${PROPERTY_BROWSER_SRCS})
source_group("Header Files/Cubes/Diagram" FILES ${DIAGRAM_HDRS})
source_group("Source Files/Cubes/Diagram" FILES ${DIAGRAM_SRCS})
source_group("Header Files/Cubes/Xml" FILES ${XML_HDRS})
source_group("Source Files/Cubes/Xml" FILES ${XML_SRCS})
source_group("Header Files/Cubes/Analysis" FILES ${ANALYSIS_HDRS})
source_group("Source Files/Cubes/Analysis" FILES ${ANALYSIS_SRCS})
source_group("Header Files/Cubes/Top" FILES ${TOP_HDRS})
source_group("Source Files/Cubes/Top" FILES ${TOP_SRCS})
source_group("Header Files/Cubes/Unit" FILES ${UNIT_HDRS})
source_group("Source Files/Cubes/Unit" FILES ${UNIT_SRCS})
source_group("Header Files/Cubes/Tree" FILES ${TREE_HDRS})
source_group("Source Files/Cubes/Tree" FILES ${TREE_SRCS})
source_group("Header Files/Cubes/Utils" FILES ${UTILS_HDRS})
source_group("Source Files/Cubes/Utils" FILES ${UTILS_SRCS})
source_group("Header Files/Cubes/Windows" FILES ${WINDOWS_HDRS})
source_group("Source Files/Cubes/Windows" FILES ${WINDOWS_SRCS})

# добавление библиотеки в проект
add_executable(${TARGET_NAME} ${TEST_HEADERS} ${TEST_SOURCES}
	${FILE_HDRS} ${FILE_SRCS} ${LOG_HDRS} ${LOG_SRCS} ${PARAMETERS_HDRS} ${PARAMETERS_SRCS}
	${PROPERTIES_HDRS} ${PROPERTIES_SRCS} ${PROPERTY_BROWSER_HDRS} ${PROPERTY_BROWSER_SRCS}
	${DIAGRAM_HDRS} ${DIAGRAM_SRCS} ${XML_HDRS} ${XML_SRCS} ${ANALYSIS_HDRS} ${ANALYSIS_SRCS}
	${TOP_HDRS} ${TOP_SRCS} ${UNIT_HDRS} ${UNIT_SRCS} ${TREE_HDRS} ${TREE_SRCS}
	${UTILS_HDRS} ${UTILS_SRCS} ${WINDOWS_HDRS} ${WINDOWS_SRCS})

set_target_properties(${TARGET_NAME} PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$(Configuration))
	

if("${PACKAGE_MANAGER}" STREQUAL "CONAN")
	set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
	
	find_package(Boost REQUIRED CONFIG)
	find_package(Qt5 COMPONENTS Core Gui Widgets Xml REQUIRED CONFIG)
	find_package(yaml-cpp REQUIRED CONFIG)
	find_package(jsoncpp REQUIRED CONFIG)
	find_package(minizip REQUIRED CONFIG)
	find_package(GTest REQUIRED CONFIG)
	
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
	find_package(GTest REQUIRED CONFIG)
endif()

if("${PACKAGE_MANAGER}" STREQUAL "CONAN")
	target_link_libraries(${TARGET_NAME} parameters QtPropertyBrowser Boost::boost Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml yaml-cpp jsoncpp_lib_static minizip::minizip GTest::gtest_main)
	target_include_directories(${TARGET_NAME} PRIVATE ../submodules/QtPropertyBrowser/src)
elseif("${PACKAGE_MANAGER}" STREQUAL "3RDPARTY")
	target_link_libraries(${TARGET_NAME} PRIVATE tpv2::parameters tpv2::qtpropertybrowser tpv2::boost Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml tpv2::yamlcpp tpv2::jsoncpp tpv2::gtest)
else()
	target_link_libraries(${TARGET_NAME} PRIVATE parameters::parameters QtPropertyBrowser Boost::boost Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Xml yaml-cpp jsoncpp_lib_static minizip::minizip GTest::gtest_main)
endif()

include(GoogleTest)
gtest_discover_tests(${TARGET_NAME})
