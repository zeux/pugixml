project(pugixml)

cmake_minimum_required(VERSION 2.6)

option(BUILD_SHARED_LIBS "Build shared instead of static library" OFF)
option(BUILD_TESTS "Build tests" OFF)
set(BUILD_DEFINES "" CACHE STRING "Build defines")

if(MSVC)
	option(STATIC_CRT "Use static CRT libraries" OFF)

	# Rewrite command line flags to use /MT if necessary
	if(STATIC_CRT)
		foreach(flag_var
				CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
				CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
			if(${flag_var} MATCHES "/MD")
				string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
			endif(${flag_var} MATCHES "/MD")
		endforeach(flag_var)
	endif()
endif()

# Pre-defines standard install locations on *nix systems.
include(GNUInstallDirs)
mark_as_advanced(CLEAR CMAKE_INSTALL_LIBDIR CMAKE_INSTALL_INCLUDEDIR)

set(INSTALL_BIN_DIR "bin" CACHE PATH "Installation directory for executables")
set(INSTALL_LIB_DIR "lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDE_DIR "include" CACHE PATH "Installation directory for header files")
set(INSTALL_PKGCONFIG_DIR "lib/pkgconfig" CACHE PATH "Installation directory for pkg-config file")

set(HEADERS src/pugixml.hpp src/pugiconfig.hpp)
set(SOURCES ${HEADERS} src/pugixml.cpp)

if(DEFINED BUILD_DEFINES)
	foreach(DEFINE ${BUILD_DEFINES})
		add_definitions("-D" ${DEFINE})
	endforeach()
endif()

if(BUILD_SHARED_LIBS)
	add_library(pugixml SHARED ${SOURCES})
else()
	add_library(pugixml STATIC ${SOURCES})
endif()

# Enable C++11 long long for compilers that are capable of it
if(NOT ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} STRLESS 3.1 AND ";${CMAKE_CXX_COMPILE_FEATURES};" MATCHES ";cxx_long_long_type;")
	target_compile_features(pugixml PUBLIC cxx_long_long_type)
endif()

set_target_properties(pugixml PROPERTIES VERSION 1.7 SOVERSION 1)
get_target_property(PUGIXML_VERSION_STRING pugixml VERSION)

# Install shared library into its own directory under LIBDIR
install(TARGETS pugixml EXPORT pugixml-config
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}/pugixml-${PUGIXML_VERSION_STRING}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/pugixml-${PUGIXML_VERSION_STRING}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install header files into their own directory under INCLUDEDIR
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pugixml-${PUGIXML_VERSION_STRING})
install(EXPORT pugixml-config DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pugixml)

if(BUILD_SHARED_LIBS)
	###############################################################################
	# prepare pkg-config file

	configure_file(misc/cmake/pugixml.pc
	"${PROJECT_BINARY_DIR}/pugixml.pc" @ONLY)

	# copy the pugixml.pc file into lib/pkgconfig
	if(INSTALL_PKGCONFIG_DIR)
		install(
			FILES ${PROJECT_BINARY_DIR}/pugixml.pc
			DESTINATION ${INSTALL_PKGCONFIG_DIR})
	endif()
	###############################################################################
endif()

if(BUILD_TESTS)
	file(GLOB TEST_SOURCES tests/*.cpp)
	file(GLOB FUZZ_SOURCES tests/fuzz_*.cpp)
	list(REMOVE_ITEM TEST_SOURCES ${FUZZ_SOURCES})

	add_executable(check ${TEST_SOURCES})
	target_link_libraries(check pugixml)
	add_custom_command(TARGET check POST_BUILD COMMAND check WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()