cmake_minimum_required(VERSION 3.4)
project(pugixml VERSION 1.11 LANGUAGES CXX)

include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(GNUInstallDirs)
include(CTest)


cmake_dependent_option(USE_VERSIONED_LIBDIR
  "Use a private subdirectory to install the headers and libraries" OFF
  "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

cmake_dependent_option(USE_POSTFIX
  "Use separate postfix for each configuration to make sure you can install multiple build outputs" OFF
  "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

cmake_dependent_option(STATIC_CRT
  "Use static MSVC RT libraries" OFF
  "MSVC" OFF)

cmake_dependent_option(BUILD_TESTS
  "Build pugixml tests" OFF
  "BUILD_TESTING;CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

option(BUILD_SHARED_AND_STATIC_LIBS "Build both shared and static libraries" OFF)
 # Technically not needed for this file. This is builtin.
option(BUILD_SHARED_LIBS "Build shared instead of static library" OFF) 

set(BUILD_DEFINES CACHE STRING "Build defines")

# This is used to backport a CMake 3.15 feature, but is also forwards compatible
if (NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
  set(CMAKE_MSVC_RUNTIME_LIBRARY
    MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<NOT:$<BOOL:${STATIC_CRT}>>:DLL>)
endif()

if (NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

if (USE_POSTFIX)
  set(CMAKE_RELWITHDEBINFO_POSTFIX _r)
  set(CMAKE_MINSIZEREL_POSTFIX _m)
  set(CMAKE_DEBUG_POSTFIX _d)
endif()

if (CMAKE_VERSION VERSION_LESS 3.15)
  set(msvc-rt $<TARGET_PROPERTY:MSVC_RUNTIME_LIBRARY>)

  set(msvc-rt-mtd-shared $<STREQUAL:${msvc-rt},MultiThreadedDebugDLL>)
  set(msvc-rt-mtd-static $<STREQUAL:${msvc-rt},MultiThreadedDebug>)
  set(msvc-rt-mt-shared $<STREQUAL:${msvc-rt},MultiThreadedDLL>)
  set(msvc-rt-mt-static $<STREQUAL:${msvc-rt},MultiThreaded>)
  unset(msvc-rt)

  set(msvc-rt-mtd-shared $<${msvc-rt-mtd-shared}:-MDd>)
  set(msvc-rt-mtd-static $<${msvc-rt-mtd-static}:-MTd>)
  set(msvc-rt-mt-shared $<${msvc-rt-mt-shared}:-MD>)
  set(msvc-rt-mt-static $<${msvc-rt-mt-static}:-MT>)
endif()

set(versioned-dir $<$<BOOL:${USE_VERSIONED_LIBDIR}>:/pugixml-${PROJECT_VERSION}>)

set(libs)

if (BUILD_SHARED_LIBS OR BUILD_SHARED_AND_STATIC_LIBS)
  add_library(pugixml-shared SHARED
    ${PROJECT_SOURCE_DIR}/scripts/pugixml_dll.rc
    ${PROJECT_SOURCE_DIR}/src/pugixml.cpp)
  add_library(pugixml::shared ALIAS pugixml-shared)
  list(APPEND libs pugixml-shared)

  set_property(TARGET pugixml-shared PROPERTY EXPORT_NAME shared)
  target_include_directories(pugixml-shared
    PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)
  target_compile_definitions(pugixml-shared
    PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:PUGIXML_API=__declspec\(dllexport\)>)
  target_compile_options(pugixml-shared
    PRIVATE
      ${msvc-rt-mtd-shared}
      ${msvc-rt-mtd-static}
      ${msvc-rt-mt-shared}
      ${msvc-rt-mt-static})
endif()

if (NOT BUILD_SHARED_LIBS OR BUILD_SHARED_AND_STATIC_LIBS)
  add_library(pugixml-static STATIC
    ${PROJECT_SOURCE_DIR}/src/pugixml.cpp)
  add_library(pugixml::static ALIAS pugixml-static)
  list(APPEND libs pugixml-static)

  set_property(TARGET pugixml-static PROPERTY EXPORT_NAME static)
  target_include_directories(pugixml-static
    PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)
  target_compile_options(pugixml-static
    PRIVATE
      ${msvc-rt-mtd-shared}
      ${msvc-rt-mtd-static}
      ${msvc-rt-mt-shared}
      ${msvc-rt-mt-static})
endif()

if (BUILD_SHARED_LIBS)
  set(pugixml-alias pugixml-shared)
else()
  set(pugixml-alias pugixml-static)
endif()
add_library(pugixml INTERFACE)
target_link_libraries(pugixml INTERFACE ${pugixml-alias})
add_library(pugixml::pugixml ALIAS pugixml)

set_target_properties(${libs}
  PROPERTIES
    MSVC_RUNTIME_LIBRARY ${CMAKE_MSVC_RUNTIME_LIBRARY}
    EXCLUDE_FROM_ALL ON
    POSITION_INDEPENDENT_CODE ON
    SOVERSION ${PROJECT_VERSION_MAJOR}
    VERSION ${PROJECT_VERSION}
    OUTPUT_NAME pugixml)

set_target_properties(${libs}
  PROPERTIES
    EXCLUDE_FROM_ALL OFF)
set(install-targets pugixml ${libs})

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/scripts/pugixml-config.cmake.in"
  "${PROJECT_BINARY_DIR}/pugixml-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  NO_SET_AND_CHECK_MACRO)

write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/pugixml-config-version.cmake"
  COMPATIBILITY SameMajorVersion)

if (USE_POSTFIX)
  if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    set(LIB_POSTFIX ${CMAKE_RELWITHDEBINFO_POSTFIX})
  elseif(CMAKE_BUILD_TYPE MATCHES MinSizeRel)
    set(LIB_POSTFIX ${CMAKE_MINSIZEREL_POSTFIX})
  elseif(CMAKE_BUILD_TYPE MATCHES Debug)
    set(LIB_POSTFIX ${CMAKE_DEBUG_POSTFIX})
  endif()
endif()

configure_file(scripts/pugixml.pc.in pugixml.pc @ONLY)

if (NOT DEFINED PUGIXML_RUNTIME_COMPONENT)
  set(PUGIXML_RUNTIME_COMPONENT Runtime)
endif()

if (NOT DEFINED PUGIXML_LIBRARY_COMPONENT)
  set(PUGIXML_LIBRARY_COMPONENT Library)
endif()

if (NOT DEFINED PUGIXML_DEVELOPMENT_COMPONENT)
  set(PUGIXML_DEVELOPMENT_COMPONENT Development)
endif()

set(namelink-component)
if (NOT CMAKE_VERSION VERSION_LESS 3.12)
  set(namelink-component NAMELINK_COMPONENT ${PUGIXML_DEVELOPMENT_COMPONENT})
endif()
install(TARGETS ${install-targets}
  EXPORT pugixml-targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${PUGIXML_RUNTIME_COMPONENT}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${PUGIXML_LIBRARY_COMPONENT} ${namelink-component}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ${PUGIXML_DEVELOPMENT_COMPONENT}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}${versioned-dir})

install(EXPORT pugixml-targets
  NAMESPACE pugixml::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pugixml COMPONENT ${PUGIXML_DEVELOPMENT_COMPONENT})

export(EXPORT pugixml-targets
  NAMESPACE pugixml::)

install(FILES
  "${PROJECT_BINARY_DIR}/pugixml-config-version.cmake"
  "${PROJECT_BINARY_DIR}/pugixml-config.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pugixml COMPONENT ${PUGIXML_DEVELOPMENT_COMPONENT})

install(FILES ${PROJECT_BINARY_DIR}/pugixml.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig COMPONENT ${PUGIXML_DEVELOPMENT_COMPONENT})

install(
  FILES
    "${PROJECT_SOURCE_DIR}/src/pugiconfig.hpp"
    "${PROJECT_SOURCE_DIR}/src/pugixml.hpp"
  DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}${versioned-dir} COMPONENT ${PUGIXML_DEVELOPMENT_COMPONENT})

if (BUILD_TESTS)
  set(fuzz-pattern "tests/fuzz_*.cpp")
  set(test-pattern "tests/*.cpp")
  if (CMAKE_VERSION VERSION_GREATER 3.11)
    list(INSERT fuzz-pattern 0 CONFIGURE_DEPENDS)
    list(INSERT test-pattern 0 CONFIGURE_DEPENDS)
  endif()
  file(GLOB test-sources ${test-pattern})
  file(GLOB fuzz-sources ${fuzz-pattern})
  list(REMOVE_ITEM test-sources ${fuzz-sources})

  add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

  add_executable(pugixml-check ${test-sources})
  add_test(NAME pugixml::test
    COMMAND pugixml-check
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
  add_dependencies(check pugixml-check)
  target_link_libraries(pugixml-check
    PRIVATE
      pugixml::pugixml)
endif()
