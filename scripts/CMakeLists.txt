project(pugixml)

cmake_minimum_required(VERSION 2.6)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared instead of static library")

if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    SET(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
endif()

set(HEADERS ../src/pugixml.hpp ../src/pugiconfig.hpp)
set(SOURCES ${HEADERS} ../src/pugixml.cpp)

if(BUILD_SHARED_LIBS)
    add_library(pugixml SHARED ${SOURCES})
else()
    add_library(pugixml STATIC ${SOURCES})
endif()

set_target_properties(pugixml PROPERTIES VERSION 1.4 SOVERSION 1)

install(TARGETS pugixml LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ${HEADERS} DESTINATION include)
