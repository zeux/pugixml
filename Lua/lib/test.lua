assert(require 'pugilua')

local doc=pugi.xml_document()
os.execute("cd")
local res=doc:load_file [[..\..\scripts\pugilua\pugilua.vcxproj]]
print(res.description)

local node=doc:child('Project')
assert(node.valid)
print(node.valid)
print(node.name)