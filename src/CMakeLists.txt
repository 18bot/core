cmake_minimum_required(VERSION 3.1)
project(hexbot)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_CXX_STANDARD 11)
set(JSONCPP_WITH_TESTS "OFF" CACHE STRING "Do not need that.")

file(GLOB HEXBOT_SRC
    "*.h"
    "*.cpp"
)

add_compile_options(-std=c++11)
include_directories(hexbot "../external/jsoncpp/include")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../external/jsoncpp" "jsoncpp")

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(OUTPUT_DIR "${ROOT_DIR}/bin")
set(CORE_OUTPUT_DIR "${OUTPUT_DIR}/core")

if(APPLE)
	set(MACOSX_BUNDLE_BUNDLE_NAME hexbot)
	add_library(hexbot MODULE ${HEXBOT_SRC})
	set_target_properties(hexbot PROPERTIES 
		BUNDLE TRUE
		LIBRARY_OUTPUT_DIRECTORY "${CORE_OUTPUT_DIR}"
		LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CORE_OUTPUT_DIR}"
		LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CORE_OUTPUT_DIR}"
	)

	# copy the build to the unity assets folder
	add_custom_command(TARGET hexbot POST_BUILD
	    COMMAND cp -R "${CORE_OUTPUT_DIR}/" "${ROOT_DIR}/unity/Assets/Plugins"
	)

	# copy the build to the probably built unity application
	add_custom_command(TARGET hexbot POST_BUILD
	    COMMAND cp -R "${CORE_OUTPUT_DIR}/" "${OUTPUT_DIR}/hexbot.app/Contents/Plugins"
	)

else()
	add_library(hexbot SHARED ${HEXBOT_SRC})

	set_target_properties(hexbot PROPERTIES 
		LIBRARY_OUTPUT_DIRECTORY "${CORE_OUTPUT_DIR}"
		LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CORE_OUTPUT_DIR}"
		LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CORE_OUTPUT_DIR}"
	)
endif()

target_link_libraries(hexbot jsoncpp_lib_static)


