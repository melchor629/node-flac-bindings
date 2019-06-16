cmake_minimum_required(VERSION 3.1)
project(flac-bindings)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCE_FILES "src/**/*.cpp" "src/**/*.hpp" "src/**/*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
