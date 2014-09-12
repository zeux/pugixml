#!/bin/sh

git svn init https://pugixml.googlecode.com/svn/trunk
git update-ref refs/remotes/git-svn refs/remotes/origin/master
git svn rebase
