#!/usr/bin/perl

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

$fast = (shift eq 'fast');
@toolsets = ($^O =~ /win/i) ? (bcc, cw, dmc, ic8, mingw34, mingw44, mingw45, msvc6, msvc7, msvc71, msvc8, msvc8_x64, msvc9, msvc9_x64, msvc10) : (gcc);
@configurations = (debug, release);
@defines = (PUGIXML_NO_XPATH, PUGIXML_NO_EXCEPTIONS, PUGIXML_NO_STL, PUGIXML_WCHAR_MODE);
@definesabbr = (noxpath, noexcept, nostl, wchar);

if ($fast)
{
	@defines = (PUGIXML_WCHAR_MODE);
	@definesabbr = (wchar);
}

@definesets = permute(@defines);

$fail = 0;

system("echo ### autotest begin >autotest.log");

%results = ();

foreach $toolset (@toolsets)
{
	foreach $configuration (@configurations)
	{
		foreach $defineset (@definesets)
		{
			$defineabbr = $defineset;
			$defineabbr =~ s/,/ /g;

			for ($i = 0; $i < $#definesabbr + 1; ++$i)
			{
				$defineabbr =~ s/$defines[$i]/$definesabbr[$i]/;
			}

			if ($defineabbr !~ /noxpath/ && $defineabbr =~ /noexcept/) { next; }
			if ($defineabbr !~ /noxpath/ && $defineabbr =~ /nostl/) { next; }

			print "*** testing $toolset/$configuration ($defineabbr) ... ***\n";

			my $cmdline = "jam toolset=$toolset configuration=$configuration defines=$defineset";
			my $coverage_pugixml = 0;
			my $coverage_pugixpath = 0;
	
			system("echo ^# $cmdline run_tests >>autotest.log");
			$result = system("$cmdline run_tests >>autotest.log");

			# get coverage
			if ($result == 0 && $toolset =~ /mingw|gcc/)
			{
				$coverage = `$cmdline coverage`;

				$coverage_pugixml = $1 if ($coverage =~ /pugixml\.cpp' executed:([^%]+)%/);
				$coverage_pugixpath = $1 if ($coverage =~ /pugixpath\.cpp' executed:([^%]+)%/);
			}
	
			# record failures
			$fail = 1 if ($result != 0);

			# print build report
			my $report = "";

			if ($result == 0)
			{
				my $align = ($coverage_pugixml > 0 || $coverage_pugixpath > 0) ? 'left' : 'center';

				$report .= "<td bgcolor='#00ff00' align='$align'>passed";
				
				if ($coverage_pugixml > 0 || $coverage_pugixpath > 0)
				{
					$report .= " <font size='-2'>$coverage_pugixml% / $coverage_pugixpath%</font>";
				}

				$report .= "</td>"

			}
			else
			{
				$report .= "<td bgcolor='#ff0000' align='center'>failed</td>"
			}

			$results{"$configuration $defineabbr"}{$toolset} = $report;
		}

		last if ($fast);
	}
}

system("echo ### autotest end >>autotest.log");

$date = scalar localtime;

$report = <<END;
<html><head><title>pugixml autotest report</title></head><body>
<h3>pugixml autotest report</h3>
<table border=0 cellspacing=0 cellpadding=4>
<tr><td></td>
END

foreach $toolset (@toolsets)
{
	$report .= "<th>$toolset</th>";
}

$report .= "</tr>\n";

foreach $k (sort {$a cmp $b} keys %results)
{
	$report .= "<tr><td>$k</td>";

	foreach $toolset (@toolsets)
	{
		$report .= $results{$k}{$toolset};
	}
		
	$report .= "</tr>\n";
}

$report .= <<END;
</table><br>
Generated on $date
</body></html>
END

open FILE, ">autotest.html";
print FILE $report;
close FILE;
