#!/usr/bin/perl

use Config;

sub permute
{
	my @defines = @_;
	my @result = ('');
	
	foreach $define (@defines)
	{
		push @result, map { length($_) == 0 ? $define : "$_,$define" } @result;
	}

	@result;
}

sub gcctoolset
{
	my $gccversion = `gcc -dumpversion`;
	chomp($gccversion);

	return "gcc$gccversion";
}

$fast = (shift eq 'fast');
@toolsets = ($^O =~ /MSWin/) ? (bcc, cw, dmc, ic8, mingw34, mingw44, mingw45, msvc6, msvc7, msvc71, msvc8, msvc8_x64, msvc9, msvc9_x64, msvc10, msvc10_x64) : ($^O =~ /solaris/) ? (suncc) : (&gcctoolset());
@configurations = (debug, release);
@defines = (PUGIXML_NO_XPATH, PUGIXML_NO_EXCEPTIONS, PUGIXML_NO_STL, PUGIXML_WCHAR_MODE);

if ($fast)
{
	@defines = (PUGIXML_WCHAR_MODE);
}

@definesets = permute(@defines);

print "### autotest begin " . scalar localtime() . "\n";

%results = ();

foreach $toolset (@toolsets)
{
	foreach $configuration (@configurations)
	{
		foreach $defineset (@definesets)
		{
			if ($defineset !~ /NO_XPATH/ && $defineset =~ /NO_EXCEPTIONS/) { next; }
			if ($defineset !~ /NO_XPATH/ && $defineset =~ /NO_STL/) { next; }

			print STDERR "*** testing $toolset/$configuration ($defineset) ... ***\n";

			# launch command
			my $cmdline = "jam toolset=$toolset configuration=$configuration defines=$defineset";
	
			print "### autotest launch $cmdline\n";

			my $coverage = `$cmdline coverage`;
			my $result = $? >> 8;

			# print build output
			print $coverage;

			# parse coverage
			my $coverage_pugixml = $1 if ($coverage =~ /pugixml\.cpp' executed:([^%]+)%/);
			my $coverage_pugixpath = $1 if ($coverage =~ /pugixpath\.cpp' executed:([^%]+)%/);
	
			# print build report
			print "### autotest $Config{archname} $toolset $configuration [$defineset] result $result $coverage_pugixml $coverage_pugixpath\n";
		}

		last if ($fast);
	}
}

print "### autotest end " . scalar localtime() . "\n";
