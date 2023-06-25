cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

project(flac-bindings)

include(CheckCCompilerFlag)
include(CheckIPOSupported)

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
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "11.0.0")
      # GCC does not warn about invalid target so we need to disabled it manually
      set(COMPILER_SUPPORTS_x86_64_V2_MICRO_ARCH NO)
    else()
      check_c_compiler_flag("-march=x86-64-v2" COMPILER_SUPPORTS_x86_64_V2_MICRO_ARCH)
    endif()
    check_c_compiler_flag("-march=nehalem" COMPILER_SUPPORTS_NEHALEM_MICRO_ARCH)
    if (COMPILER_SUPPORTS_x86_64_V2_MICRO_ARCH)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64-v2")
    elseif(COMPILER_SUPPORTS_NEHALEM_MICRO_ARCH)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=nehalem")
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

check_ipo_supported(RESULT LTO_SUPPORTED)
if(LTO_SUPPORTED AND CMAKE_BUILD_TYPE STREQUAL "Release")
  message("-- LTO/IPO supported and enabled")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

option(USE_FLAC_SOURCES OFF)
set(FLAC_TARGET FLAC)

if(NOT USE_FLAC_SOURCES)
  if(MSVC)
    find_package(FLAC 1.3 QUIET)
  else()
    find_package(FLAC 1.3 QUIET)
    if(NOT FLAC_FOUND)
      include(FindPkgConfig)

      pkg_check_modules(FLAC QUIET IMPORTED_TARGET flac>=1.3)
      set(FLAC_TARGET PkgConfig::FLAC)
    endif()
  endif()
endif()

if(USE_FLAC_SOURCES OR NOT FLAC_FOUND)
  if(NOT USE_FLAC_SOURCES)
    message("-- No libflac detected on system that can be used, fall back to compile from sources")
  endif()

  include(FetchContent)

  option(BUILD_TESTING OFF)

  # fetch libogg (v1.3.5)
  FetchContent_Declare(
    Ogg
    GIT_REPOSITORY  https://github.com/xiph/ogg
    GIT_TAG         v1.3.5
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

  # fetch flac (v1.4.1)
  FetchContent_Declare(
    FLAC
    GIT_REPOSITORY  https://github.com/xiph/flac
    GIT_TAG         1.4.3
  )

  option(WITH_OGG YES)
  option(BUILD_PROGRAMS OFF)
  option(BUILD_EXAMPLES OFF)
  option(BUILD_CXXLIBS OFF)
  option(BUILD_TESTING OFF)
  option(BUILD_DOCS OFF)
  option(INSTALL_MANPAGES OFF)
  FetchContent_GetProperties(FLAC)
  if(NOT flac_POPULATED)
    FetchContent_Populate(FLAC)
    add_subdirectory(${flac_SOURCE_DIR} ${flac_BINARY_DIR})
  endif()

  set(FLAC_TARGET FLAC)
endif()

file(GLOB_RECURSE SOURCE_FILES "src/**/*.cpp" "src/**/*.hpp" "src/**/*.h" "src/*.cpp")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})

set(napi_build_version 8 CACHE STRING "N-API Version")
message("-- Using N-API ${napi_build_version}")
add_compile_definitions(NAPI_VERSION=${napi_build_version} NAPI_EXPERIMENTAL NODE_ADDON_API_DISABLE_DEPRECATED)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${FLAC_TARGET})

if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()

if(USE_FLAC_SOURCES OR NOT FLAC_FOUND)
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set_target_properties("${FLAC_TARGET}" PROPERTIES POSITION_INDEPENDENT_CODE ON)
  endif()
endif()
