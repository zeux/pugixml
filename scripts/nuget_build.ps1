function Run-Command([string]$cmd)
{
	Invoke-Expression $cmd
	if ($LastExitCode) { exit $LastExitCode }
}

function Force-Copy([string]$from, [string]$to)
{
	Write-Host $from "=>" $to
	New-Item -Force $to | Out-Null
	Copy-Item -Force $from $to
	if (! $?) { exit 1 }
}

Push-Location
$scriptdir = Split-Path $MyInvocation.MyCommand.Path
cd $scriptdir

Run-Command "msbuild pugixml_vs2013.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2013.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2013.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2013.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo"

Run-Command "msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo"

Run-Command "msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo"

Run-Command "msbuild pugixml_vs2015_static.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2015_static.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2015_static.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2015_static.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo"

Run-Command "msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo"

Run-Command "msbuild pugixml_vs2017_static.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2017_static.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2017_static.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2017_static.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo"

Force-Copy "../src/pugiconfig.hpp" "nuget/build/native/include/pugiconfig.hpp"
Force-Copy "../src/pugixml.hpp" "nuget/build/native/include/pugixml.hpp"

Force-Copy "vs2013/x32/pugixml.lib" "nuget/build/native/lib/Win32/v120/dynamic/Release/pugixml.lib"
Force-Copy "vs2013/x32/pugixmld.lib" "nuget/build/native/lib/Win32/v120/dynamic/Debug/pugixml.lib"
Force-Copy "vs2013/x64/pugixml.lib" "nuget/build/native/lib/x64/v120/dynamic/Release/pugixml.lib"
Force-Copy "vs2013/x64/pugixmld.lib" "nuget/build/native/lib/x64/v120/dynamic/Debug/pugixml.lib"

Force-Copy "vs2013/x32/pugixmls.lib" "nuget/build/native/lib/Win32/v120/static/Release/pugixml.lib"
Force-Copy "vs2013/x32/pugixmlsd.lib" "nuget/build/native/lib/Win32/v120/static/Debug/pugixml.lib"
Force-Copy "vs2013/x64/pugixmls.lib" "nuget/build/native/lib/x64/v120/static/Release/pugixml.lib"
Force-Copy "vs2013/x64/pugixmlsd.lib" "nuget/build/native/lib/x64/v120/static/Debug/pugixml.lib"

Force-Copy "vs2015/Win32_Release/pugixml.lib" "nuget/build/native/lib/Win32/v140/dynamic/Release/pugixml.lib"
Force-Copy "vs2015/Win32_Debug/pugixml.lib" "nuget/build/native/lib/Win32/v140/dynamic/Debug/pugixml.lib"
Force-Copy "vs2015/x64_Release/pugixml.lib" "nuget/build/native/lib/x64/v140/dynamic/Release/pugixml.lib"
Force-Copy "vs2015/x64_Debug/pugixml.lib" "nuget/build/native/lib/x64/v140/dynamic/Debug/pugixml.lib"

Force-Copy "vs2015/Win32_ReleaseStatic/pugixml.lib" "nuget/build/native/lib/Win32/v140/static/Release/pugixml.lib"
Force-Copy "vs2015/Win32_DebugStatic/pugixml.lib" "nuget/build/native/lib/Win32/v140/static/Debug/pugixml.lib"
Force-Copy "vs2015/x64_ReleaseStatic/pugixml.lib" "nuget/build/native/lib/x64/v140/static/Release/pugixml.lib"
Force-Copy "vs2015/x64_DebugStatic/pugixml.lib" "nuget/build/native/lib/x64/v140/static/Debug/pugixml.lib"

Force-Copy "vs2017/Win32_Release/pugixml.lib" "nuget/build/native/lib/Win32/v141/dynamic/Release/pugixml.lib"
Force-Copy "vs2017/Win32_Debug/pugixml.lib" "nuget/build/native/lib/Win32/v141/dynamic/Debug/pugixml.lib"
Force-Copy "vs2017/x64_Release/pugixml.lib" "nuget/build/native/lib/x64/v141/dynamic/Release/pugixml.lib"
Force-Copy "vs2017/x64_Debug/pugixml.lib" "nuget/build/native/lib/x64/v141/dynamic/Debug/pugixml.lib"

Force-Copy "vs2017/Win32_ReleaseStatic/pugixml.lib" "nuget/build/native/lib/Win32/v141/static/Release/pugixml.lib"
Force-Copy "vs2017/Win32_DebugStatic/pugixml.lib" "nuget/build/native/lib/Win32/v141/static/Debug/pugixml.lib"
Force-Copy "vs2017/x64_ReleaseStatic/pugixml.lib" "nuget/build/native/lib/x64/v141/static/Release/pugixml.lib"
Force-Copy "vs2017/x64_DebugStatic/pugixml.lib" "nuget/build/native/lib/x64/v141/static/Debug/pugixml.lib"

Run-Command "nuget pack nuget"

Pop-Location
