---
layout: default
title: Home
---

> If you use pugixml, please [star it on GitHub](https://github.com/zeux/pugixml/stargazers)!

pugixml is a light-weight C++ XML processing library. It features:

* DOM-like interface with rich traversal/modification capabilities
* [Extremely fast](/benchmark.html) non-validating XML parser which constructs the DOM tree from an XML file/buffer
* XPath 1.0 implementation for complex data-driven tree queries
* Full Unicode support with Unicode interface variants and automatic encoding conversions

The library is extremely portable and easy to integrate and use. You can download it in an archive (<a href="http://github.com/zeux/pugixml/releases/download/v{{site.version}}/pugixml-{{site.version}}.zip" onclick="trackLink('download', 'pugixml-{{site.version}}.zip')">Windows</a>/<a href="http://github.com/zeux/pugixml/releases/download/v{{site.version}}/pugixml-{{site.version}}.tar.gz" onclick="trackLink('download', 'pugixml-{{site.version}}.tar.gz')">Unix</a> line endings), get it from [Git/Subversion repository](https://github.com/zeux/pugixml), install it as a package in one of the major Linux/BSD distributions ([Ubuntu](http://packages.ubuntu.com/search?keywords=pugixml), [Debian](https://tracker.debian.org/pkg/pugixml), [Fedora](https://apps.fedoraproject.org/packages/pugixml), [Gentoo](http://packages.gentoo.org/package/dev-libs/pugixml?arches=prefix), [Arch Linux](https://www.archlinux.org/packages/community/x86_64/pugixml/), [FreeBSD](http://www.freshports.org/textproc/pugixml/) and [more](http://pkgs.org/search/pugixml)), install it as a package in one of the OSX package managers ([Homebrew](http://brewformulas.org/Pugixml), [MacPorts](https://www.macports.org/ports.php?by=name&substr=pugixml)), install [a NuGet package](https://www.nuget.org/packages/pugixml/) or use one of the alternative package managers ([Conda](https://anaconda.org/conda-forge/pugixml])).

pugixml is developed and maintained since 2006 and has many users. All code is distributed under the [MIT license](/license.html), making it completely free to use in both open-source and proprietary applications.

### Recent news [(more)](/news.html)

<ul>
{% for post in site.posts limit:2 %}
  <li>
    {{ post.date | date_to_long_string }} <a href="{{ post.url }}">{{ post.title }}</a>
    {{ post.excerpt }}
  </li>
{% endfor %}
</ul>
