msbuild vs2015/vs2015.sln /target:Clean /property:Configuration=Debug /property:Platform=x86
msbuild vs2015/vs2015.sln /target:Clean /property:Configuration=Debug /property:Platform=x64
msbuild vs2015/vs2015.sln /target:Clean /property:Configuration=Release /property:Platform=x86
msbuild vs2015/vs2015.sln /target:Clean /property:Configuration=Release /property:Platform=x64

msbuild vs2015/vs2015.sln /property:Configuration=Debug /property:Platform=x86
msbuild vs2015/vs2015.sln /property:Configuration=Debug /property:Platform=x64
msbuild vs2015/vs2015.sln /property:Configuration=Release /property:Platform=x86
msbuild vs2015/vs2015.sln /property:Configuration=Release /property:Platform=x64

powershell Write-NuGetPackage nuget.autopkg
