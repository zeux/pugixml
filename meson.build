project('pugixml', 'cpp',
  version: '1.11.4',
  meson_version: '>=0.46.0',
  default_options: ['cpp_std=c++11'],
)

cxx = meson.get_compiler('cpp')

version = meson.project_version().split('.')
soversion = '.'.join([ version[0], version[1] ])

sources = files('src/pugixml.cpp')

if host_machine.system() == 'windows'
  win_mod = import('windows')
  sources += win_mod.compile_resources('scripts/pugixml_dll.rc')
endif

args = []
if cxx.get_id() == 'msvc'
  if get_option('default_library') == 'shared'
    args +='-DPUGIXML_API=__declspec(dllexport)'
  elif get_option('default_library') == 'both'
    error('cannot build both shared and static on MSVC due to declspec')
  endif
endif

foreach opt: ['wchar_mode', 'compact', 'no_xpath', 'no_stl', 'no_exceptions']
  if get_option(opt)
    args += '-DPUGIXML_' + opt.to_upper()
  endif
endforeach

pugixml_lib = library('pugixml', sources,
  cpp_args: args,
  version: soversion,
  install: true,
)
install_headers('src/pugiconfig.hpp', 'src/pugixml.hpp')

pugixml_dep = declare_dependency(
  link_with: pugixml_lib,
  include_directories: include_directories('src')
)

import('pkgconfig').generate(
  pugixml_lib,
  description: 'Light-weight, simple and fast XML parser for C++ with XPath support.',
  url: 'https://pugixml.org/',
)

if find_program('cmake', required: false).found()
  cmake = import('cmake')
  cmake.write_basic_package_version_file(
    name: meson.project_name(),
    version: meson.project_version(),
    compatibility: 'SameMajorVersion',
  )
  cmake.configure_package_config_file(
    input: 'scripts/pugixml-config.cmake.in',
    name: meson.project_name(),
    configuration: configuration_data(),
  )
endif

subdir('tests')
pugixml_check = executable('pugixml-check', test_sources, dependencies: pugixml_dep)
test('pugixml-check', pugixml_check, workdir: meson.current_source_dir())
