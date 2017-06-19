Push-Location
$scriptdir = Split-Path $MyInvocation.MyCommand.Path
cd $scriptdir

"%VS141COMNTOOLS%\VsMSBuildCmd.bat"
msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo
If(!$?){exit 1}
msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo
If(!$?){exit 1}
Write-NuGetPackage nuget.autopkg
If(!$?){exit 1}

Pop-Location
