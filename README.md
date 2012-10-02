pugilua
=======

An almost one-to-one lua binding for [pugixm](http://pugixml.org/)

Usage
-----

Use the visual studio project to compile in Visual Studio. The release build will be copied into the Lua/lib folder, where you can start
lua directly and use the library by calling `require 'pugilua'`. Follow the test for API differences from the original C++ API. Pugilua objects reside in the
pugi 'namespace' in Lua.

A usage sketch:
````
require 'pugilua'

local doc=pugi.xml_document()
local res=doc:load_file [[..\..\scripts\pugilua\pugilua.vcxproj]]
assert(res.valid)
print(res.description)

local node=doc:child('Project')
assert(node.valid)
````

### API differences

 * There's no explicit cast to boolean of the pugilua objects, hence the classes `xml_parse_result, xml_node and xml_document` have a boolean `property` valid
 * lua classes have no inheritance
 * Getter methods are mapped to lua properties and not methods

Dependencies
------------

 * [pugixml](https://github.com/zeux/pugixml) the original library
 * [lua](http://www.lua.org/) the language, Lua 5.1.4 from [luaforwindows](http://code.google.com/p/luaforwindows/), but it should work for every proper lua which is supported by LuaBridge
 * [LuaBridge](https://github.com/vinniefalco/LuaBridge) for the declarative bindings to Lua

License
-------

This library is distributed under the MIT License:

Copyright (c) 2012 Dmitry Ledentsov

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
