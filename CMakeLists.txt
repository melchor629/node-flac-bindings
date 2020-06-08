cmake_minimum_required(VERSION 3.10)
project(flac-bindings)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS}")
    message("-- Enabled code coverage")
endif()

if(SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined -static-libasan")
    message("-- Enabled sanitizers")
endif()

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

execute_process(
    COMMAND node -p "require('node-addon-api').include"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

file(GLOB_RECURSE SOURCE_FILES "src/**/*.cpp" "src/**/*.hpp" "src/**/*.h" "src/*.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

set(napi_build_version 6)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12.0)
    add_compile_definitions(NAPI_VERSION=${napi_build_version} NAPI_EXPERIMENTAL NODE_ADDON_API_DISABLE_DEPRECATED)
else()
    add_definitions(NAPI_VERSION=${napi_build_version} NAPI_EXPERIMENTAL NODE_ADDON_API_DISABLE_DEPRECATED)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC} ${NODE_ADDON_API_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
