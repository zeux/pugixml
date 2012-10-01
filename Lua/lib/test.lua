assert(require 'pugilua')

local doc=pugi.xml_document()
os.execute("cd")
local res=doc:load_file [[..\..\tests\data\latintest_latin1.xml]]
print(res.description)