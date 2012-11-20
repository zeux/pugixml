#!/usr/bin/perl

use Archive::Tar;
use Archive::Zip;

my $target = shift @ARGV;
my @sources = @ARGV;

my $basedir = ($target =~ /^(.*)(\.zip|\.tar.gz|\.tgz)$/) ? "$1/" : '';

my $zip = $target =~ /\.zip$/;
my $arch = $zip ? Archive::Zip->new : Archive::Tar->new;

for $source (sort {$a cmp $b} @sources)
{
	my $contents = &readfile_contents($source);
	my $meta = &readfile_meta($source);
	my $file = $basedir . $source;

	if ($zip)
	{
		my $path = $file;
		$arch->addDirectory($path) if $path =~ s/\/[^\/]+$/\// && !defined($arch->memberNamed($path));

		my $member = $arch->addString($contents, $file);

		$member->desiredCompressionMethod(COMPRESSION_DEFLATED);
		$member->desiredCompressionLevel(9);

		$member->setLastModFileDateTimeFromUnix($$meta{mtime});
	}
	else
	{
		# tgz releases are for Unix people, Unix people like Unix newlines
		$contents =~ s/\r//g if (-T $source);

		$arch->add_data($file, $contents, $meta);
	}
}

$zip ? $arch->overwriteAs($target) : $arch->write($target, 9);

sub readfile_contents
{
	my $file = shift;

	open FILE, $file or die "Can't open $file: $!";
	binmode FILE;
	my @contents = <FILE>;
	close FILE;

	return join('', @contents);
}

sub readfile_meta
{
	my $file = shift;

    my ($dev, $ino, $mode, $nlink, $uid, $gid, $rdev, $size, $atime, $mtime, $ctime, $blksize, $blocks) = stat($file);

	return {mtime => $mtime};
}
