/*
 * Header-only variant of pugixml.
 * This file is provided to the public domain.
 * Written by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
 */

#ifndef HEADER_PUGIXML_HEADER_ONLY_HPP
#define HEADER_PUGIXML_HEADER_ONLY_HPP

/*
 * This file provides a header-only variant of pugixml.
 * This is generally not recommended because of resulting code bloat/increased link time/conflicts with local symbols,
 * so use it with caution.
 */

#include "pugixml.cpp"
#include "pugixpath.cpp"

#endif
