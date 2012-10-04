pugilua
=======

An almost one-to-one lua binding for [pugixml](http://pugixml.org/).

Usage
-----

Use the visual studio project to compile in Visual Studio. The release build will be copied into the Lua/lib folder, where you can start
lua directly and use the library by calling `require 'pugilua'`. Follow the test and the binding definitions in contrib/pugilua/pugilua_lib.cpp for API differences from the original C++ API. Pugilua objects reside in the
pugi 'namespace' in Lua.

Example of usage:
````
require 'pugilua'


---- reading ----
local doc=pugi.xml_document()
local res=doc:load_file [[..\..\scripts\pugilua\pugilua.vcxproj]]

print(res.description)

local node1=doc:root():child('Project')
local query1=doc:root():select_nodes('Project/PropertyGroup')

local n=query1.size
for i=0,n-1 do
  local node=query1:get(i):node()
	local attribute=query1:get(i):attribute()
	print(node.valid,node.path)
	local a=node:first_attribute()
	while a.valid do
		print(a.name)
		a=a:next_attribute()
	end
end

---- creating ----
doc:reset()
--- from the tutorial
-- add node with some name
local node = doc:root():append_child("node")

-- add description node with text child
local descr = node:append_child("description")
descr:append(pugi.node_pcdata):set_value("Simple node")

-- add param node before the description
local param = node:insert_child_before("param", descr)

-- add attributes to param node
param:append_attribute("name"):set_value("version")
param:append_attribute("value"):set_value(1.1)
param:insert_attribute_after("type", param:attribute("name")):set_value("float")

doc:save_file("tutorial.xml")
````

See an [imperfect example](https://gist.github.com/3832071) of dumping an xml file in a less verbose format,
or a filter of [vcproj](https://gist.github.com/3832285) or [vcxproj](https://gist.github.com/3832290) source files into lua tables

### API differences

 * There's no explicit cast to boolean of the pugilua objects, hence the classes `xml_parse_result, xml_node and xml_document` have a boolean `property` valid
 * lua classes have no inheritance
 * Getter methods are mapped to lua properties and not methods

Dependencies
------------

 * [pugixml](https://github.com/zeux/pugixml) the original library
 * [lua](http://www.lua.org/) the language, Lua 5.1.4 from [luaforwindows](http://code.google.com/p/luaforwindows/), but it should work for every proper lua which is supported by LuaBridge
 * [LuaBridge](https://github.com/vinniefalco/LuaBridge) for the declarative bindings to Lua
 * [premake4](http://industriousone.com/premake) for generating make files

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
