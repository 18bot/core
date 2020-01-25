cmake_minimum_required(VERSION 3.1)
project(hexbot)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_CXX_STANDARD 11)
set(JSONCPP_WITH_TESTS "OFF" CACHE STRING "Do not need that.")

file(GLOB HEXBOT_SRC
    "*.h"
    "*.cpp"
)

include_directories(hexbot "${CMAKE_CURRENT_SOURCE_DIR}/../external/jsoncpp/include")
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
	    COMMAND test -d "${ROOT_DIR}/unity" && cp -R "${CORE_OUTPUT_DIR}/" "${ROOT_DIR}/unity/Assets/Plugins"
	)

	# copy the build to the probably built unity application
	add_custom_command(TARGET hexbot POST_BUILD
	    COMMAND test -d "${OUTPUT_DIR}/hexbot.app" && cp -R "${CORE_OUTPUT_DIR}/" "${OUTPUT_DIR}/hexbot.app/Contents/Plugins"
	)

else()
	add_library(hexbot SHARED ${HEXBOT_SRC})

	set_target_properties(hexbot PROPERTIES 
		RUNTIME_OUTPUT_DIRECTORY "${CORE_OUTPUT_DIR}"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CORE_OUTPUT_DIR}"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CORE_OUTPUT_DIR}"
	)

	get_filename_component(CORE_OUTPUT_FLATTER "${CORE_OUTPUT_DIR}" ABSOLUTE)

	get_filename_component(TARGET_A "${ROOT_DIR}/unity/Assets/Plugins/hexbot.dll" ABSOLUTE)
	get_filename_component(TARGET_B "${OUTPUT_DIR}/hexbot/hexbot_Data/Plugins/hexbot.dll" ABSOLUTE)

	# copy the build to the unity assets folder
	add_custom_command(TARGET hexbot POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy "${CORE_OUTPUT_FLATTER}/hexbot.dll" "${TARGET_A}"
	)

	# copy the build to the probably built unity application
	add_custom_command(TARGET hexbot POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E copy "${CORE_OUTPUT_FLATTER}/hexbot.dll" "${TARGET_B}"
	)
endif()

target_link_libraries(hexbot jsoncpp_lib_static)


