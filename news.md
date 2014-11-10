---
layout: default
title: News
---

## News

<hr />
{% for post in site.posts %}
<p>
  {{ post.date | date_to_long_string }} <a href="{{ post.url }}">{{ post.title }}</a>
  {{ post.content }}
</p>
<hr />
{% endfor %}
