cmake_minimum_required(VERSION 3.4)
project(pugixml VERSION 1.10 LANGUAGES CXX)

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

# This is used to backport a CMake 3.15 feature
if (NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
  set(CMAKE_MSVC_RUNTIME_LIBRARY
    MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<NOT:$<BOOL:${STATIC_CRT}>>:DLL>)
endif()

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

option(BUILD_SHARED_AND_STATIC_LIBS "Build both shared and static libraries" OFF)
option(BUILD_SHARED_LIBS "Build shared instead of static library" OFF)
option(BUILD_TESTS "Build tests" OFF)

set(BUILD_DEFINES "" CACHE STRING "Build defines")

# Pre-defines standard install locations on *nix systems.
mark_as_advanced(CLEAR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR)

set(HEADERS src/pugixml.hpp src/pugiconfig.hpp)
set(SOURCES src/pugixml.cpp)

if(BUILD_SHARED_LIBS AND MSVC)
	set(PUGIXML_WINDLL_SRCS	scripts/pugixml_dll.rc)
endif()

if(DEFINED BUILD_DEFINES)
	foreach(DEFINE ${BUILD_DEFINES})
		add_definitions("-D" ${DEFINE})
	endforeach()
endif()

if(BUILD_SHARED_AND_STATIC_LIBS)
	set(LIBRARY pugixml-static pugixml-shared)
else()
	set(LIBRARY pugixml)
endif()

if(BUILD_SHARED_AND_STATIC_LIBS)
    add_library(pugixml-static STATIC ${HEADERS} ${SOURCES})
    add_library(pugixml-shared SHARED ${HEADERS} ${SOURCES} ${PUGIXML_WINDLL_SRCS})
else()
  if(BUILD_SHARED_LIBS)
    add_library(pugixml SHARED ${HEADERS} ${SOURCES} ${PUGIXML_WINDLL_SRCS})
  else()
    add_library(pugixml STATIC ${HEADERS} ${SOURCES})
  endif()
endif()

# Export symbols for shared library builds
if(BUILD_SHARED_AND_STATIC_LIBS AND MSVC)
	target_compile_definitions(pugixml-shared PRIVATE "PUGIXML_API=__declspec(dllexport)")
endif()

if(BUILD_SHARED_LIBS AND MSVC)
	target_compile_definitions(pugixml PRIVATE "PUGIXML_API=__declspec(dllexport)")
endif()

if(USE_VERSIONED_LIBDIR)
	# Install library into its own directory under LIBDIR
	set(INSTALL_SUFFIX /pugixml-${pugixml_VERSION})
endif()

foreach(TARGET ${LIBRARY})
	# Enable C++11 long long for compilers that are capable of it
	if(NOT ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} STRLESS 3.1 AND ";${CMAKE_CXX_COMPILE_FEATURES};" MATCHES ";cxx_long_long_type;")
	  target_compile_features(${TARGET} PUBLIC cxx_long_long_type)
	endif()

	set_target_properties(${TARGET} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

	target_include_directories(${TARGET} PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}${INSTALL_SUFFIX}>)

	if(USE_POSTFIX)
		set_target_properties(${TARGET} PROPERTIES DEBUG_POSTFIX "_d" MINSIZEREL_POSTFIX "_m" RELWITHDEBINFO_POSTFIX "_r")
	endif()
endforeach()

install(TARGETS ${LIBRARY} EXPORT pugixml-config
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}${INSTALL_SUFFIX}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}${INSTALL_SUFFIX}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}${INSTALL_SUFFIX})
install(EXPORT pugixml-config DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pugixml)

configure_file(scripts/pugixml.pc.in ${PROJECT_BINARY_DIR}/pugixml.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/pugixml.pc DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)

if(BUILD_TESTS)
	file(GLOB TEST_SOURCES tests/*.cpp)
	file(GLOB FUZZ_SOURCES tests/fuzz_*.cpp)
	list(REMOVE_ITEM TEST_SOURCES ${FUZZ_SOURCES})

	add_executable(check ${TEST_SOURCES})
	if(BUILD_SHARED_AND_STATIC_LIBS)
		target_link_libraries(check pugixml-static)
	else()
		target_link_libraries(check pugixml)
	endif()
	add_custom_command(TARGET check POST_BUILD COMMAND check WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
