/**
 * pugixml parser - version 1.14
 * --------------------------------------------------------
 * Copyright (C) 2006-2024, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
 * Report bugs and download new versions at https://pugixml.org/
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 *
 * This work is based on the pugxml parser, which is:
 * Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
 */

module;

#define PUGIXML_EXPORT_MODULE

#ifndef PUGIXML_USE_STD_MODULE
#	ifdef __cpp_lib_modules
#		define PUGIXML_USE_STD_MODULE
#	endif
#endif

#include <pugiconfig.hpp>

#ifndef PUGIXML_USE_STD_MODULE
#	include <string_view>
#	include <iterator>
#	include <istream>
#	include <ostream>
#	include <string>
#	include <new>
#	include <exception>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <stdint.h>

#ifdef PUGIXML_WCHAR_MODE
#	include <wchar.h>
#endif

#ifndef PUGIXML_NO_XPATH
#	include <math.h>
#	include <float.h>
#endif

#if defined(__linux__) || defined(__APPLE__)
#	include <sys/stat.h>
#endif

export module pugixml;

#ifdef PUGIXML_USE_STD_MODULE
import std.compat;
#endif

#define PUGIXML_MODULE_EXPORT export
#if defined(__clang__)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#elif defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable:5244)
#endif
#if !defined(PUGIXML_HEADER_ONLY)
extern "C++" {
#endif
#include <pugixml.hpp>
#if !defined(PUGIXML_HEADER_ONLY)
}
#endif
#if defined(__clang__)
#	pragma clang diagnostic pop
#elif defined(_MSC_VER)
#	pragma warning(pop)
#endif

module :private;

#if defined(PUGIXML_HEADER_ONLY) && !defined(PUGIXML_SOURCE)
#	define PUGIXML_SOURCE "pugixml.cpp"
#	include PUGIXML_SOURCE
#endif
