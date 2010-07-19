#!/usr/bin/perl

$prefix = join(' ', @ARGV);
$prefix .= ' ' if ($prefix ne '');

$lines = join('', <STDIN>);
$lines =~ s/File (.+)\nLines (.+)\n(.+\n)*\n/$1 $2\n/g;
$lines =~ s/.+include\/c\+\+.+\n//g;

foreach $line (split /\n/, $lines)
{
	print "$prefix$line\n";
}
