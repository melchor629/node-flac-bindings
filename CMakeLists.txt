cmake_minimum_required(VERSION 3.10)
project(flac-bindings)

include(CheckCCompilerFlag)

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
  if (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
    check_c_compiler_flag("-march=x86-64-v2" COMPILER_SUPPORTS_x86_64_V2_MICRO_ARCH)
    check_c_compiler_flag("-march=sandybridge" COMPILER_SUPPORTS_SANDYBRIDGE_MICRO_ARCH)
    if (COMPILER_SUPPORTS_x86_64_V2_MICRO_ARCH)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64-v2")
    elseif(COMPILER_SUPPORTS_SANDYBRIDGE_MICRO_ARCH)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=sandybridge")
    endif()
  elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
    check_c_compiler_flag("-march=armv8-a" COMPILER_SUPPORTS_ARM_V8_MICRO_ARCH)
    if (COMPILER_SUPPORTS_ARM_V8_MICRO_ARCH)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a")
    endif()
  elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm")
    check_c_compiler_flag("-march=armv7-a" COMPILER_SUPPORTS_ARM_V7_MICRO_ARCH)
    if (COMPILER_SUPPORTS_ARM_V7_MICRO_ARCH)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a")
    endif()
  endif()
endif()

option(FLAC_BINDINGS_USE_EXTERNAL_LIBRARY NO)
set(FLAC_TARGET FLAC)

if(FLAC_BINDINGS_USE_EXTERNAL_LIBRARY)
  if(MSVC)
    find_package(FLAC 1.3 REQUIRED)
  else()
    find_package(FLAC 1.3 QUIET)
    if(NOT FLAC_FOUND)
      include(FindPkgConfig)

      pkg_check_modules(FLAC REQUIRED IMPORTED_TARGET flac>=1.3)
      set(FLAC_TARGET PkgConfig::FLAC)
    endif()
  endif()
else()
  include(FetchContent)

  option(BUILD_TESTING NO)

  # fetch libogg (post v1.3.4 commit to fix compilation in macOS)
  FetchContent_Declare(
    Ogg
    GIT_REPOSITORY  https://gitlab.xiph.org/xiph/ogg.git
    GIT_TAG         369657ca2eb36131cbd53359b4d365e2b48054de
  )

  # polyfill for FetchContent_MakeAvailable
  FetchContent_GetProperties(Ogg)
  if(NOT ogg_POPULATED)
    FetchContent_Populate(Ogg)
    add_subdirectory(${ogg_SOURCE_DIR} ${ogg_BINARY_DIR})
  endif()
  # tell CMake ogg already exists
  set(OGG_FOUND YES)
  # expected target is different in FLAC, so creating an alias to be able to use it
  add_library(Ogg::ogg ALIAS ogg)

  # fetch flac (post v1.3.3 - to reduce compilation time by disabling lots of targets)
  FetchContent_Declare(
    FLAC
    GIT_REPOSITORY  https://gitlab.xiph.org/xiph/flac.git
    GIT_TAG         27c615706cedd252a206dd77e3910dfa395dcc49
  )

  option(WITH_OGG YES)
  option(BUILD_PROGRAMS NO)
  option(BUILD_EXAMPLES NO)
  option(BUILD_CXXLIBS NO)
  FetchContent_GetProperties(FLAC)
  if(NOT flac_POPULATED)
    FetchContent_Populate(FLAC)
    add_subdirectory(${flac_SOURCE_DIR} ${flac_BINARY_DIR})
  endif()
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

set(napi_build_version 8)
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12.0)
  add_compile_definitions(NAPI_VERSION=${napi_build_version} NAPI_EXPERIMENTAL NODE_ADDON_API_DISABLE_DEPRECATED)
else()
  add_definitions(NAPI_VERSION=${napi_build_version} NAPI_EXPERIMENTAL NODE_ADDON_API_DISABLE_DEPRECATED)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC} ${NODE_ADDON_API_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${FLAC_TARGET})

if(NOT FLAC_BINDINGS_USE_EXTERNAL_LIBRARY)
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set_target_properties("${FLAC_TARGET}" PROPERTIES POSITION_INDEPENDENT_CODE ON)
  endif()
endif()
