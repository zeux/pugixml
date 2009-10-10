#!/usr/bin/perl

$lines = join('', <>);
$lines =~ s/File (.+)\nLines (.+)\n(.+\n)*\n/$1 $2\n/g;
$lines =~ s/.+include\/c\+\+.+\n//g;

print $lines;
