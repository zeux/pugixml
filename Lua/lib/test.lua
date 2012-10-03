assert(require 'pugilua')

local doc=pugi.xml_document()
os.execute("cd")
local res=doc:load_file [[..\..\scripts\pugilua\pugilua.vcxproj]]
print(res.description)
assert(res.valid)

local node=doc:child('Project')
assert(node.valid)
print(node.valid)
print(node.name)
local query1=doc:select_nodes('Project/PropertyGroup')
local query2=node:select_nodes('PropertyGroup')
assert(query1)
assert(query2)
print(query1.type,pugi.xpath_node_set.type_sorted)
query1:sort(true)
print(query1.type,pugi.xpath_node_set.type_sorted)
print(query1.size,query2.size)
n=query1.size
for i=0,n-1 do
	node=query1:get(i):node()
	print(node.valid)
end