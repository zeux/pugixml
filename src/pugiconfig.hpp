///////////////////////////////////////////////////////////////////////////////
//
// Pug Improved XML Parser - Version 0.41
// --------------------------------------------------------
// Copyright (C) 2006-2009, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
// This work is based on the pugxml parser, which is:
// Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
// See pugxml.xml for further information, history, etc.
// Contributions by Neville Franks (readonly@getsoft.com).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef HEADER_PUGICONFIG_HPP
#define HEADER_PUGICONFIG_HPP

// Uncomment this to disable STL
// #define PUGIXML_NO_STL

// Uncomment this to disable XPath
// #define PUGIXML_NO_XPATH

// Uncomment this to disable exceptions
// Note: you can't use XPath with PUGIXML_NO_EXCEPTIONS
// #define PUGIXML_NO_EXCEPTIONS

// Set this to control attributes for public classes/functions, i.e.:
// #define PUGIXML_API __declspec(dllexport) // to export all public symbols from DLL
// #define PUGIXML_CLASS __declspec(dllimport) // to import all classes from DLL
// #define PUGIXML_FUNCTION __fastcall // to set calling conventions to all public functions to fastcall
// In absence of PUGIXML_CLASS/PUGIXML_FUNCTION definitions PUGIXML_API is used instead

#endif
