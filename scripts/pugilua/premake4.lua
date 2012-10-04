local OS=os.get()

local cmd = {
 dir = { linux = "ls", windows = "dir" }
}

local Commands={}

for i,v in pairs(cmd) do
 Commands[i]=cmd[i][OS]
end

-- Apply to current "filter" (solution/project)
function DefaultConfig()
	location "Build"
	configuration "Debug"
		defines { "DEBUG", "_DEBUG" }
		objdir "Build/obj"
		targetdir "Build/Debug"
		flags { "Symbols" }
	configuration "Release"
		defines { "RELEASE" }
		objdir "Build/obj"
		targetdir "Build/Release"
		flags { "Optimize" }
	configuration "*" -- to reset configuration filter
end

function CompilerSpecificConfiguration()
	configuration {"xcode*" }
		postbuildcommands {"$TARGET_BUILD_DIR/$TARGET_NAME"}

	configuration {"gmake"}
		postbuildcommands  { "$(TARGET)" }
		buildoptions { "-std=gnu++0x" }

	configuration {"codeblocks" }
		postbuildcommands { "$(TARGET_OUTPUT_FILE)"}

	configuration { "vs*"}
		postbuildcommands { "\"$(TargetPath)\"" }
end

----------------------------------------------------------------------------------------------------------------

newaction {
   trigger     = "run",
   description = "run lua",
   execute     = function ()
      os.execute("lua -l pugilua")
   end
}

----------------------------------------------------------------------------------------------------------------

-- A solution contains projects, and defines the available configurations
local sln=solution "pugilua"
	location "Build"
		sln.absbasedir=path.getabsolute(sln.basedir)
		configurations { "Debug", "Release" }
		platforms { "native" }
		libdirs { [[../../lua/lib]],
			  [[/usr/local/lib]]}
		includedirs {
			[[../../lua/include]],
			[[../../LuaBridge]],
			[[../../src]],
			[[/usr/local/include]]
		}
		vpaths {
			["Headers"] = {"**.h","**.hpp"},
			["Sources"] = {"**.c", "**.cpp"},
		}

----------------------------------------------------------------------------------------------------------------
   local dll=project "pugilua"
   location "Build"
		kind "SharedLib"
		DefaultConfig()
		language "C++"
		files {
			"../../contrib/pugilua/*.h",
			"../../contrib/pugilua/*.cpp",
			"../../src/*.hpp",
			"../../src/*.cpp"
		}
		if (OS=='linux') then links { "lua" }
		else  links { "lua5.1" } end
