#!/usr/bin/perl

# pretty-printing
sub prettytoolset
{
	my $toolset = shift;

	return "Borland C++ 5.82" if ($toolset eq 'bcc');
	return "Metrowerks CodeWarrior 8" if ($toolset eq 'cw');
	return "Digital Mars C++ 8.51" if ($toolset eq 'dmc');

	return "Intel C++ Compiler $1.0" if ($toolset =~ /^ic(\d+)$/);
	return "MinGW32 (GCC $1.$2)" if ($toolset =~ /^mingw(\d)(\d)$/);
	return "Microsoft Visual C++ 7.1" if ($toolset eq 'msvc71');
	return "Microsoft Visual C++ $1.0" if ($toolset =~ /^msvc(\d+)$/);
	return "Microsoft Visual C++ $1.0 x64" if ($toolset =~ /^msvc(\d+)_x64$/);

	$toolset;
}

sub prettyplatform
{
	my $platform = shift;

	return "linux64" if ($platform =~ /64-linux/);
	return "linux32" if ($platform =~ /86-linux/);

	return "freebsd64" if ($platform =~ /64-freebsd/);
	return "freebsd32" if ($platform =~ /86-freebsd/);

	return "win64" if ($platform =~ /MSWin32-x64/);
	return "win32" if ($platform =~ /MSWin32/);

	$platform;
}

# parse build log
%results = ();
%toolsets = ();
%defines = ();
%configurations = ();

sub insertindex
{
	my ($hash, $key) = @_;

	$$hash{$key} = scalar(keys %$hash) unless defined $$hash{$key};
}

while (<>)
{
	### autotest i386-freebsd-64int gcc release [wchar] result 0 97.78 98.85
	if (/^### autotest (\S+) (\S+) (\S+) \[(.*?)\] result (\S+) (\S*) (\S*)/)
	{
		my ($platform, $toolset, $configuration, $defineset, $result, $coverage_pugixml, $coverage_pugixpath) = ($1, $2, $3, $4, $5, $6, $7);

		die "Detected duplicate build information $_\n" if defined $results{"$toolset $platform"}{$configuration}{$defineset};

		my $fulltool = &prettyplatform($platform) . ' ' . &prettytoolset($toolset);
		my $fullconf = "$configuration $defineset";

		$results{$fulltool}{$fullconf}{result} = $result;
		$results{$fulltool}{$fullconf}{coverage_pugixml} = $coverage_pugixml;
		$results{$fulltool}{$fullconf}{coverage_pugixpath} = $coverage_pugixpath;

		&insertindex(\%toolsets, $fulltool);

		$defines{$_} = 1 foreach (split /,/, $defineset);
		&insertindex(\%configurations, $fullconf);
	}
}

# make arrays of toolsets and configurations
@toolsetarray = ();
@configurationarray = ();

$toolsetarray[$toolsets{$_}] = $_ foreach (keys %toolsets);
$configurationarray[$configurations{$_}] = $_ foreach (keys %configurations);

# print header
print <<END;
<html><head><title>pugixml autotest report</title></head><body>
<h3>pugixml autotest report</h3>
<table border=1 cellspacing=0 cellpadding=4>
END

# print configuration header (release/debug)
print "<tr><td align='right' colspan=2>configuration</td>";
print "<td>".(split /\s+/)[0]."</td>" foreach (@configurationarray);
print "</tr>\n";

# print defines header (one row for each define)
foreach $define (sort {$a cmp $b} keys %defines)
{
	print "<tr><td align='right' colspan=2><small>$define</small></td>";

	foreach (@configurationarray)
	{
		my $present = ($_ =~ /\b$define\b/);
		my $color = $present ? "#cccccc" : "#ffffff";
		print "<td bgcolor='$color' align='center'>" . ($present ? "+" : "&nbsp;") . "</td>";
	}
	print "</tr>\n";
}

# print data (one row for each toolset)
foreach $tool (@toolsetarray)
{
	my ($platform, $toolset) = split(/\s+/, $tool, 2);
	print "<tr><td style='border-right: none' align='right'><small>$platform</small></td><td style='border-left: none'>$toolset</td>";

	foreach (@configurationarray)
	{
		my $info = $results{$tool}{$_};

		if (!defined $$info{result})
		{
			print "<td bgcolor='#cccccc'>&nbsp;</td>";
		}
		elsif ($$info{result} == 0)
		{
			my ($coverage_pugixml, $coverage_pugixpath) = ($$info{coverage_pugixml}, $$info{coverage_pugixpath});

			print "<td bgcolor='#00ff00' align='center'>pass";
				
			if ($coverage_pugixml > 0 || $coverage_pugixpath > 0)
			{
				print "<br><font size='-2'>" . ($coverage_pugixml + 0) . "%<br>" . ($coverage_pugixpath + 0) . "%</font>";
			}

			print "</td>";
		}
		else
		{
			print "<td bgcolor='#ff0000' align='center'>fail</td>"
		}
	}

	print "</tr>\n";
}

# print footer
$date = localtime;

print <<END;
</table><br>
Generated on $date
</body></html>
END
