cmake_minimum_required(VERSION 3.1)
project(hexbot)

set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(JSONCPP_WITH_TESTS "OFF" CACHE STRING "Do not need that.")

file(GLOB HEXBOT_SRC
    "src/*.h"
    "src/*.cpp"
)

include_directories(hexbot "external/jsoncpp/include")
add_subdirectory("external/jsoncpp" "jsoncpp")

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
set(OUTPUT_DIR "${ROOT_DIR}/bin")
set(CORE_OUTPUT_DIR "${OUTPUT_DIR}/core")

add_library(hexbot SHARED ${HEXBOT_SRC})

get_filename_component(CORE_OUTPUT_FLATTER "${CORE_OUTPUT_DIR}" ABSOLUTE)

# copy the build to the unity assets folder
add_custom_command(TARGET hexbot POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hexbot> "${ROOT_DIR}/unity/Assets/Plugins"
)

# copy the build to the probably built unity application
add_custom_command(TARGET hexbot POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:hexbot> "${OUTPUT_DIR}/hexbot/hexbot_Data/Plugins"
)

target_link_libraries(hexbot jsoncpp_lib_static)


