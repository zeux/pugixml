#!/usr/bin/perl

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

		my $fulltool = "$toolset $platform";
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
print "<tr><td align='right'>configuration</td>";
print "<td>".(split /\s+/)[0]."</td>" foreach (@configurationarray);
print "</tr>\n";

# print defines header (one row for each define)
foreach $define (sort {$a cmp $b} keys %defines)
{
	print "<tr><td align='right'><small>$define</small></td>";

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
	print "<tr><td>$tool</td>";

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
