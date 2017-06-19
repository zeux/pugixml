function Run-Command
{
	Invoke-Expression $args[0]
	if ($LastExitCode) { exit $LastExitCode }
}

Push-Location
$scriptdir = Split-Path $MyInvocation.MyCommand.Path
cd $scriptdir

Run-Command "msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2013_static.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2015.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x86 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Debug /p:Platform=x64 /v:minimal /nologo"
Run-Command "msbuild pugixml_vs2017.vcxproj /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal /nologo"
Write-NuGetPackage nuget.autopkg

Pop-Location
