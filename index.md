---
layout: default
title: Home
---

pugixml is a light-weight C++ XML processing library. It features:

* DOM-like interface with rich traversal/modification capabilities
* Extremely fast non-validating XML parser which constructs the DOM tree from an XML file/buffer
* XPath 1.0 implementation for complex data-driven tree queries
* Full Unicode support with Unicode interface variants and automatic encoding conversions

The library is extremely portable and easy to integrate and use.

pugixml is developed and maintained since 2006 and has many users. All code is distributed under the MIT license, making it completely free to use in both open-source and proprietary applications.

### Recent news [(more)](/news.html)

<ul>
{% for post in site.posts limit:2 %}
  <li>
    {{ post.date | date_to_long_string }} <a href="{{ post.url }}">{{ post.title }}</a>
    {{ post.excerpt }}
  </li>
{% endfor %}
</ul>
