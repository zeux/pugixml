A new approach to MSVC build system, using PropertySheets.
This approach is based on MsBuild ver 4, so it concerns only latest MSVS versions (starting from Msvs2010).
New projects, solutions and props files are located in msvc directory.  

All properties and item metadata, such as compiler and linker settings are declared in PropertySheets (*.props files).

There are dozen props *.files in msvc/props directory. Each *.props file contain settings for some build configuration. The most part of *.props files are suitable for all VisualStudio versions.

Each project file should import pugixml.props file (located in msvc directory). Projects for static RTL should also import pugixml.s.props (before pugixml.props).

Advantages of this approach:
 * Each parameter stores in only one plase. So it is easier to explore the structure of projects. It's easier to change some setting.
 * Project files became smaller and easier to read.
 * It's easier to create new vcxproj files based on existing project. For example, projects for Msvc11 and Msvc12 was created using Msvc10 project.

Visual studio has a nice instrument for exploring and editing property sheets: Property Manager. If it not visible, turn it on by using menu View/OtherWindows/PropertyManager.

And yet another new feachure. User can customize building process by inserting his own property sheet into project structure (without modifying any file from pugixml directory).

For example:
  MsBuild.exe /p:PUGI_Props_External=d:\MyProps.props pugixml_vs2010.vcxproj

Best regards,
Taymanov Sergey,
Moscow.




