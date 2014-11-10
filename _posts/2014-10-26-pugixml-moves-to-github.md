---
layout: post
title: pugixml moves to GitHub
---

pugixml project used to be hosted on Google Code since time immemorial. About a year ago the active development switched to Git but the Subversion repository on Google Code was still maintained via svn-git and the primary way to report issues was via Issue Tracker on Google Code.

Since Google disabled the downloads feature on Google Code, pugixml releases are hosted on github anyway; also git-svn makes some git workflows more complicated. As of this day, [pugixml moves to GitHub](https://github.com/zeux/pugixml) and Google Code project is no longer maintained.

You can use GitHub’s [issue tracker](https://github.com/zeux/pugixml/issues), submit pull requests, use pugixml as Git submodules, etc.

Note that you can still use Subversion to access pugixml repository on github as follows:

	svn checkout https://github.com/zeux/pugixml/tags/latest

Please let me know if you find any issues with GitHub’s Subversion access.
